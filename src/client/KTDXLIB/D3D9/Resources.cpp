// Textures and vertex buffers of KTDXLIB's D3D9 device (decision T38): D3D9 formats, locks and their uploads.

#include "Device.h"
#include "Formats.h"
#include "Image.h"

#include <algorithm>
#include <bit>
#include <cstring>

namespace x2::d3d9 {
namespace {

// The x2::render format a D3D9 format is uploaded as: native where the device takes it, else B8G8R8A8 (converted).
render::TextureFormat RenderFormat(render::Device& device, D3DFORMAT format, render::TextureUsage usage)
{
    auto supported = [&](render::TextureFormat f) {
        return device.TextureSupportsFormat(f, render::TextureType::Texture2D, usage);
    };
    switch (format) {
    case D3DFMT_DXT1: return render::TextureFormat::Bc1Unorm;
    case D3DFMT_DXT2:
    case D3DFMT_DXT3: return render::TextureFormat::Bc2Unorm;
    case D3DFMT_DXT4:
    case D3DFMT_DXT5: return render::TextureFormat::Bc3Unorm;
    case D3DFMT_R5G6B5:
        return supported(render::TextureFormat::B5G6R5Unorm) ? render::TextureFormat::B5G6R5Unorm
                                                             : render::TextureFormat::B8G8R8A8Unorm;
    case D3DFMT_A1R5G5B5:
    case D3DFMT_X1R5G5B5:
        return supported(render::TextureFormat::B5G5R5A1Unorm) ? render::TextureFormat::B5G5R5A1Unorm
                                                               : render::TextureFormat::B8G8R8A8Unorm;
    case D3DFMT_A4R4G4B4:
    case D3DFMT_X4R4G4B4:
        return supported(render::TextureFormat::B4G4R4A4Unorm) ? render::TextureFormat::B4G4R4A4Unorm
                                                               : render::TextureFormat::B8G8R8A8Unorm;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_R8G8B8:
    case D3DFMT_A8:
    case D3DFMT_L8:
    case D3DFMT_A8L8: return render::TextureFormat::B8G8R8A8Unorm;
    default: return render::TextureFormat::Invalid;
    }
}

// A level's bytes as `to` wants them. Returns `level` itself when no conversion is needed.
std::span<const std::byte> Convert(D3DFORMAT from, render::TextureFormat to, std::span<const std::byte> level,
                                   UINT texels, std::vector<std::byte>& scratch)
{
    const bool bgra = to == render::TextureFormat::B8G8R8A8Unorm;
    if ((from == D3DFMT_A8R8G8B8 && bgra) || (Info(from).compressed) ||
        (from == D3DFMT_R5G6B5 && to == render::TextureFormat::B5G6R5Unorm) ||
        (from == D3DFMT_A1R5G5B5 && to == render::TextureFormat::B5G5R5A1Unorm) ||
        (from == D3DFMT_A4R4G4B4 && to == render::TextureFormat::B4G4R4A4Unorm)) {
        return level;
    }
    const auto* in = reinterpret_cast<const std::uint8_t*>(level.data());
    auto word = [&](UINT i) { return static_cast<unsigned>(in[i * 2] | (in[i * 2 + 1] << 8)); };
    if (!bgra) {
        // X formats on their native 16-bit format: set the unused alpha bits.
        scratch.assign(level.begin(), level.end());
        const std::uint16_t alpha = from == D3DFMT_X1R5G5B5 ? 0x8000 : 0xF000;
        for (UINT i = 0; i < texels; ++i) {
            const std::uint16_t v = static_cast<std::uint16_t>(word(i) | alpha);
            std::memcpy(&scratch[i * 2], &v, 2);
        }
        return scratch;
    }
    scratch.resize(static_cast<std::size_t>(texels) * 4);
    image::ToBgra8(from, level, texels, 1, reinterpret_cast<std::uint8_t*>(scratch.data()));
    return scratch;
}

}  // namespace

// --- Texture --------------------------------------------------------------------------------------------------------

bool Device::SupportsTextureFormat(D3DFORMAT format, DWORD usage) const
{
    const bool target = (usage & D3DUSAGE_RENDERTARGET) != 0;
    const bool autoMips = (usage & D3DUSAGE_AUTOGENMIPMAP) != 0;
    if (target && format != D3DFMT_A8R8G8B8 && format != D3DFMT_X8R8G8B8) {
        return false;
    }
    if (autoMips && Info(format).compressed) {
        return false;
    }
    render::TextureUsage renderUsage = render::TextureUsage::Sampler;
    if (target || autoMips) {
        renderUsage = renderUsage | render::TextureUsage::ColorTarget;
    }
    return RenderFormat(m_render, format, renderUsage) != render::TextureFormat::Invalid;
}

std::unique_ptr<Texture> Device::CreateTexture(UINT width, UINT height, UINT levels, DWORD usage, D3DFORMAT format,
                                               D3DPOOL pool)
{
    if (width == 0 || height == 0) {
        Fail("CreateTexture: empty size");
        return nullptr;
    }
    const bool target = (usage & D3DUSAGE_RENDERTARGET) != 0;
    if (target && format != D3DFMT_A8R8G8B8 && format != D3DFMT_X8R8G8B8) {
        Fail("CreateTexture: render targets are A8R8G8B8 or X8R8G8B8 only");
        return nullptr;
    }
    const UINT fullChain = static_cast<UINT>(std::bit_width(std::max(width, height)));
    const bool autoMips = (usage & D3DUSAGE_AUTOGENMIPMAP) != 0;
    const UINT renderLevels = levels == 0 || autoMips ? fullChain : std::min(levels, fullChain);
    render::TextureUsage renderUsage = render::TextureUsage::Sampler;
    if (target || autoMips) {
        renderUsage = renderUsage | render::TextureUsage::ColorTarget;
    }
    const render::TextureFormat renderFormat = RenderFormat(m_render, format, renderUsage);
    if (renderFormat == render::TextureFormat::Invalid) {
        Fail("CreateTexture: unsupported format " + std::to_string(static_cast<DWORD>(format)));
        return nullptr;
    }
    if (autoMips && Info(format).compressed) {
        Fail("CreateTexture: D3DUSAGE_AUTOGENMIPMAP with a compressed format");
        return nullptr;
    }
    std::unique_ptr<Texture> texture(new Texture());
    texture->m_device = this;
    texture->m_format = format;
    texture->m_usage = usage;
    texture->m_pool = pool;
    texture->m_width = width;
    texture->m_height = height;
    texture->m_renderFormat = renderFormat;
    texture->m_texture = m_render.CreateTexture({.format = renderFormat,
                                                 .usage = renderUsage,
                                                 .width = width,
                                                 .height = height,
                                                 .levelCount = renderLevels,
                                                 .name = target ? "d3d9 render target" : "d3d9 texture"});
    if (!texture->m_texture) {
        Fail("CreateTexture: " + m_render.LastError());
        return nullptr;
    }
    // D3D9 shows a texture with automatic mipmaps as one level.
    const UINT d3dLevels = autoMips ? 1 : renderLevels;
    for (UINT level = 0; level < d3dLevels; ++level) {
        Texture::Level l;
        l.width = std::max(1u, width >> level);
        l.height = std::max(1u, height >> level);
        if (!target) {
            l.bytes.resize(static_cast<std::size_t>(Pitch(format, l.width)) * Rows(format, l.height));
        }
        texture->m_levels.push_back(std::move(l));
    }
    m_liveTextures.insert(texture.get());
    return texture;
}

Texture::~Texture()
{
    if (m_device) {
        m_device->ForgetTexture(this);
        m_device->m_render.ReleaseTexture(m_texture);
    }
}

bool Texture::GetLevelDesc(UINT level, D3DSURFACE_DESC* desc) const
{
    if (level >= m_levels.size() || !desc) {
        return false;
    }
    *desc = {m_format, m_usage, m_pool, m_levels[level].width, m_levels[level].height};
    return true;
}

bool Texture::LockRect(UINT level, D3DLOCKED_RECT* locked, const RECT* rect, DWORD flags)
{
    if (!m_device || level >= m_levels.size() || !locked) {
        return false;
    }
    Level& l = m_levels[level];
    if (l.bytes.empty()) {
        return m_device->Fail("LockRect: render targets cannot be locked");
    }
    if (l.locked) {
        return m_device->Fail("LockRect: the level is already locked");
    }
    const UINT pitch = Pitch(m_format, l.width);
    std::size_t offset = 0;
    if (rect) {
        const FormatInfo info = Info(m_format);
        const UINT block = info.compressed ? 4 : 1;
        offset = static_cast<std::size_t>(rect->top / block) * pitch +
                 static_cast<std::size_t>(rect->left / block) * info.blockBytes;
    }
    l.locked = true;
    l.readOnly = (flags & D3DLOCK_READONLY) != 0;
    locked->Pitch = static_cast<INT>(pitch);
    locked->pBits = l.bytes.data() + offset;
    return true;
}

bool Texture::UnlockRect(UINT level)
{
    if (!m_device || level >= m_levels.size() || !m_levels[level].locked) {
        return false;
    }
    Level& l = m_levels[level];
    l.locked = false;
    if (l.readOnly) {
        return true;
    }
    std::vector<std::byte> scratch;
    const std::span<const std::byte> bytes = Convert(m_format, m_renderFormat, l.bytes, l.width * l.height, scratch);
    // Cycling gives a fresh texture with only this level defined: only for single-level textures.
    const bool cycle = m_levels.size() == 1 && !(m_usage & D3DUSAGE_AUTOGENMIPMAP);
    if (!m_device->UploadToTexture(m_texture, level, l.width, l.height, bytes, cycle)) {
        return false;
    }
    if ((m_usage & D3DUSAGE_AUTOGENMIPMAP) && level == 0) {
        // Mipmaps are generated outside copy passes, on the upload command buffer.
        if (m_device->m_uploadPass) {
            m_device->m_uploadPass->End();
            m_device->m_uploadPass = nullptr;
        }
        m_device->m_uploads->GenerateMipmaps(m_texture);
    }
    return true;
}

bool Device::UpdateTexture(Texture* source, Texture* destination)
{
    if (source == nullptr || destination == nullptr || source == destination ||
        source->m_width != destination->m_width || source->m_height != destination->m_height ||
        source->m_format != destination->m_format || source->m_levels.empty() || source->m_levels[0].bytes.empty()) {
        return false;
    }
    const std::size_t levels = std::min(source->m_levels.size(), destination->m_levels.size());
    for (std::size_t level = 0; level < levels; ++level) {
        const Texture::Level& from = source->m_levels[level];
        if (!destination->m_levels[level].bytes.empty()) {
            destination->m_levels[level].bytes = from.bytes;
        }
        std::vector<std::byte> scratch;
        const std::span<const std::byte> bytes =
            Convert(source->m_format, destination->m_renderFormat, from.bytes, from.width * from.height, scratch);
        const bool cycle = destination->m_levels.size() == 1;
        if (!UploadToTexture(destination->m_texture, static_cast<std::uint32_t>(level), from.width, from.height, bytes,
                             cycle)) {
            return false;
        }
    }
    return true;
}

// --- Vertex buffer --------------------------------------------------------------------------------------------------

std::unique_ptr<VertexBuffer> Device::CreateVertexBuffer(UINT length, DWORD usage, DWORD fvf, D3DPOOL /*pool*/)
{
    if (length == 0) {
        Fail("CreateVertexBuffer: empty buffer");
        return nullptr;
    }
    std::unique_ptr<VertexBuffer> buffer(new VertexBuffer());
    buffer->m_device = this;
    buffer->m_usage = usage;
    buffer->m_fvf = fvf;
    buffer->m_bytes.resize(length);
    buffer->m_buffer = m_render.CreateBuffer({.usage = render::BufferUsage::Vertex, .size = length,
                                              .name = "d3d9 vertex buffer"});
    if (!buffer->m_buffer) {
        Fail("CreateVertexBuffer: " + m_render.LastError());
        return nullptr;
    }
    m_liveVertexBuffers.insert(buffer.get());
    return buffer;
}

VertexBuffer::~VertexBuffer()
{
    if (m_device) {
        m_device->ForgetVertexBuffer(this);
        m_device->m_render.ReleaseBuffer(m_buffer);
    }
}

bool VertexBuffer::Lock(UINT offset, UINT size, void** data, DWORD flags)
{
    if (!m_device || !data || m_locked) {
        return false;
    }
    if (size == 0) {
        size = Size() > offset ? Size() - offset : 0;   // D3D9: 0 locks the rest of the buffer
    }
    if (offset + size > Size()) {
        return m_device->Fail("VertexBuffer::Lock: range out of the buffer");
    }
    m_locked = true;
    m_lockOffset = offset;
    m_lockSize = size;
    m_lockFlags = flags;
    *data = m_bytes.data() + offset;
    return true;
}

bool VertexBuffer::Unlock()
{
    if (!m_device || !m_locked) {
        return false;
    }
    m_locked = false;
    if (m_lockFlags & D3DLOCK_READONLY) {
        return true;
    }
    // NOOVERWRITE: the caller promises not to touch data in use; DISCARD: the rest is undefined, so a fresh buffer
    // with the locked range. Without flags the old data must stay for draws already recorded: a fresh buffer holding
    // the whole CPU copy.
    if (m_lockFlags & D3DLOCK_NOOVERWRITE) {
        return m_device->UploadToBuffer(m_buffer, m_lockOffset,
                                        std::span(m_bytes).subspan(m_lockOffset, m_lockSize), false);
    }
    if (m_lockFlags & D3DLOCK_DISCARD) {
        return m_device->UploadToBuffer(m_buffer, m_lockOffset,
                                        std::span(m_bytes).subspan(m_lockOffset, m_lockSize), true);
    }
    return m_device->UploadToBuffer(m_buffer, 0, m_bytes, true);
}

}  // namespace x2::d3d9
