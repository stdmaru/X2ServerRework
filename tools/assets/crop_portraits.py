"""Lines up the HUD portraits' frames in a client_data's .tet files (docs/tools/client-data.md).

Each class's portrait in the player's gauge (CX2Data's CIT_MyGage table and the berserk-mode faces of
CX2*MyGageUI::GetBerserkModeMyStateImage) is a face frame with a dark band below it, behind the wake orbs, that reaches
further left. In most the frame starts 2 texels in from the key's left edge and at its top row; in some 3 texels and a
row lower (Ara, Elesis, Add, Code: Sariel), and M_KNIGHT_STATE's key starts a column short, inside its band. The gauge
draws every portrait at the same point, so they did not line up, and none could touch the screen corner. This moves
each key's left edge so the frame starts FRAME_X texels in, and its top edge to the frame's first row; size follows
(SetSizeAsTextureSize). DLG_PVP_Game_My_State_NEW.lua then puts the dialog FRAME_X units left and 2 up.
Their textures' transparent texels are white, so linear filtering drew a light seam along the frame at the screen's
edge; each fully transparent texel within BLEED texels of a visible one takes the average color of its visible
neighbours (alpha stays 0: nothing new shows). The .tga files stay uncompressed 32-bit, only those color bytes change.
Running it again changes nothing.
    python3 tools/assets/crop_portraits.py <client_data> [--dry-run]
"""

import argparse
import os
import struct
import sys

import numpy as np
from PIL import Image

# 61 holds Chung's faces: frameless art that fades out at the edge, left as they are
TEXTURES = [16, 21, 23, 25, 45, 66, 68, 70, 74, 76, 77, 80, 81]
KEYS = """
ELSWORD_STATE L_KNIGHT_STATE M_KNIGHT_STATE R_SLAYER_STATE S_KNIGHT_STATE T_KNIGHT_STATE
AISHA_STATE B_MAGICIAN_STATE D_MAGICIAN_STATE D_WITCH_STATE E_MASTER_STATE H_MAGICIAN_STATE V_PRINCESS_STATE
C_RANGER_STATE G_ARCHER_STATE N_ARCHER_STATE RENA_STATE S_RANGER_STATE T_RANGER_STATE W_SNEAKER_STATE
B_MASTER_STATE O_TAKER_STATE RAVEN_STATE R_FIST_STATE S_TAKER_STATE V_COMMANDER_STATE W_TAKER_STATE
B_SERAPH_STATE C_EMPRESS_STATE C_NEMESIS_STATE CODE_A_STATE CODE_E_STATE CODE_L_STATE EVE_STATE
I_SWORD_FACE T_TROOPER_FACE T_TROOPER_RAGE ARA_FACE ARA_LH_FACE ARA_SD_FACE ARA_FACE_RAGE ARA_LH_FACE_RAGE
ARA_SD_FACE_RAGE L_DEVIL_STATE Y_RAJA_STATE L_DEVIL_STATE_RAGE Y_RAJA_STATE_RAGE
EL_FACE EL_SPK_FACE EL_SSK_FACE EL_SBH_FACE EL_SGM_FACE ADD_FACE ADD_ALP_FACE ADD_APT_FACE
""".split()

FRAME_X = 2   # where the frame starts in most portraits, kept
BLEED = 2
HEADER = struct.Struct("<III")
UV = struct.Struct("<10f")   # leftTop, rightBottom, leftBottom, rightTop, rectSize (texels)


def solid_start(alpha: np.ndarray) -> int:
    """The first line (row of alpha) with at least half its texels at alpha >= 200."""
    for i in range(alpha.shape[0]):
        if np.count_nonzero(alpha[i] >= 200) * 2 >= alpha.shape[1]:
            return i
    return 0


def bleed_colors(tga_path: str, dry_run: bool) -> int:
    """Gives transparent texels near visible ones their neighbours' color; returns how many changed."""
    data = bytearray(open(tga_path, "rb").read())
    if data[2] != 2 or data[16] != 32:
        print(f"{tga_path}: not an uncompressed 32-bit TGA, left as it is")
        return 0
    width, height = struct.unpack_from("<HH", data, 12)
    start = 18 + data[0]
    pixels = np.frombuffer(data, dtype=np.uint8, count=width * height * 4, offset=start).reshape(height, width, 4)
    color = pixels[:, :, :3].astype(np.float64)
    known = pixels[:, :, 3] > 0
    out = pixels.copy()
    for _ in range(BLEED):
        total = np.zeros_like(color)
        count = np.zeros((height, width))
        for dy, dx in ((-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)):
            shifted = np.zeros_like(known)
            shifted_color = np.zeros_like(color)
            ys, yd = (slice(max(dy, 0), height + min(dy, 0)), slice(max(-dy, 0), height + min(-dy, 0)))
            xs, xd = (slice(max(dx, 0), width + min(dx, 0)), slice(max(-dx, 0), width + min(-dx, 0)))
            shifted[yd, xd] = known[ys, xs]
            shifted_color[yd, xd] = color[ys, xs]
            total += shifted_color * shifted[:, :, None]
            count += shifted
        grow = ~known & (count > 0)
        color[grow] = total[grow] / count[grow][:, None]
        known = known | grow
    near = (pixels[:, :, 3] == 0) & known
    out[near, :3] = np.rint(color[near]).astype(np.uint8)
    changed = int(np.count_nonzero(np.any(out != pixels, axis=2)))
    if changed and not dry_run:
        data[start:start + width * height * 4] = out.tobytes()
        open(tga_path, "wb").write(data)
    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("client_data")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    files = {}
    for folder, _, names in os.walk(args.client_data):
        if ".kom" not in folder.split(os.sep):
            for name in names:
                files[name.lower()] = os.path.join(folder, name)

    remaining = set(KEYS)
    for number in TEXTURES:
        stem = f"dlg_ui_common_texture{number}_new"
        tet_path, tga_path = files.get(stem + ".tet"), files.get(stem + ".tga")
        if tet_path is None or tga_path is None:
            continue
        data = bytearray(open(tet_path, "rb").read())
        count = HEADER.unpack_from(data, 0)[2]
        alpha = np.asarray(Image.open(tga_path).convert("RGBA"))[:, :, 3]
        changed = False
        for i in range(count):
            offset = struct.unpack_from("<I", data, HEADER.size + 4 * i)[0]
            length = struct.unpack_from("<H", data, offset)[0]
            key = data[offset + 2:offset + 2 + 2 * length].decode("utf-16-le")
            if key.upper() not in remaining:
                continue
            remaining.discard(key.upper())
            at = HEADER.size + 4 * count + UV.size * i
            ltx, lty, rbx, rby, lbx, lby, rtx, rty, w, h = UV.unpack_from(data, at)
            if (lbx, lby, rtx, rty) != (ltx, rby, rbx, lty):
                print(f"{key}: rotated or unusual rect, left as it is")
                continue
            rect = alpha[int(lty):int(rby), int(ltx):int(rbx)]
            # left: where the face's frame starts in the upper half (the dark band behind the wake orbs reaches further)
            left, top = solid_start(rect[:rect.shape[0] // 2].T), solid_start(rect)
            left -= FRAME_X
            print(f"{key:20s} texture{number}: left edge {left:+d}, top edge {top:+d}")
            if left or top:
                UV.pack_into(data, at, ltx + left, lty + top, rbx, rby, lbx + left, lby, rtx, rty + top,
                             w - left, h - top)
                changed = True
        if changed and not args.dry_run:
            open(tet_path, "wb").write(data)
        print(f"texture{number}: {bleed_colors(tga_path, args.dry_run)} transparent texels recolored")
    for key in sorted(remaining):
        print(f"{key}: not found")
    return 0


if __name__ == "__main__":
    sys.exit(main())
