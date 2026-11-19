// The village movement sync packets (decision T53) keep every value, and their size on the wire stays small: a batch
// grows by the same few bytes per unit.
#include "packet_test.h"

#include "ClientPacket.h"

#ifdef X2_VILLAGE_SYNC

namespace
{

template <typename T>
T Reread(const T& value)
{
    const Bytes bytes = Serialize(value);
    KSerBuffer buf;
    buf.Write(bytes.data(), static_cast<DWORD>(bytes.size()));
    KSerializer ks;
    ks.BeginReading(&buf);
    T copy;
    REQUIRE(ks.Get(copy));
    ks.EndReading();
    return copy;
}

KFieldUnitState MakeState(float x)
{
    KFieldUnitState kState;
    kState.m_vPos = VECTOR3(x, -250.5f, 1200.f);
    kState.m_sSpeedX = -910;
    kState.m_sSpeedY = 1650;
    kState.m_sLineIndex = 300;
    kState.m_ucStateID = 21;
    kState.m_ucStateSeq = 255;
    kState.m_bIsRight = false;
    return kState;
}

void CheckSameState(const KFieldUnitState& a, const KFieldUnitState& b)
{
    CHECK(a.m_vPos.x == b.m_vPos.x);
    CHECK(a.m_vPos.y == b.m_vPos.y);
    CHECK(a.m_vPos.z == b.m_vPos.z);
    CHECK(a.m_sSpeedX == b.m_sSpeedX);
    CHECK(a.m_sSpeedY == b.m_sSpeedY);
    CHECK(a.m_sLineIndex == b.m_sLineIndex);
    CHECK(a.m_ucStateID == b.m_ucStateID);
    CHECK(a.m_ucStateSeq == b.m_ucStateSeq);
    CHECK(a.m_bIsRight == b.m_bIsRight);
}

}  // namespace

TEST_CASE("village sync packets keep their values")
{
    KEGS_FIELD_UNIT_STATE_NOT kNot;
    kNot.m_uiTick = 0xFFFFFFF0u;
    kNot.m_kState = MakeState(-15000.25f);
    const KEGS_FIELD_UNIT_STATE_NOT kNotCopy = Reread(kNot);
    CHECK(kNotCopy.m_uiTick == kNot.m_uiTick);
    CheckSameState(kNotCopy.m_kState, kNot.m_kState);

    KEGS_FIELD_UNIT_CORRECTION_NOT kCorrection;
    kCorrection.m_uiTick = 123456;
    kCorrection.m_kState = MakeState(42.f);
    kCorrection.m_ucReason = KEGS_FIELD_UNIT_CORRECTION_NOT::R_RISE;
    const KEGS_FIELD_UNIT_CORRECTION_NOT kCorrectionCopy = Reread(kCorrection);
    CHECK(kCorrectionCopy.m_uiTick == 123456u);
    CHECK(kCorrectionCopy.m_ucReason == KEGS_FIELD_UNIT_CORRECTION_NOT::R_RISE);
    CheckSameState(kCorrectionCopy.m_kState, kCorrection.m_kState);

    KEGS_FIELD_UNIT_STATE_BATCH_NOT kBatch;
    kBatch.m_uiTick = 99;
    for (int i = 0; i < 3; ++i)
    {
        KFieldUnitStateEntry kEntry;
        kEntry.m_UnitUID = 1000000000000LL + i;
        kEntry.m_uiTick = 90 + i;
        kEntry.m_kState = MakeState(static_cast<float>(i));
        kBatch.m_vecUnit.push_back(kEntry);
    }
    const KEGS_FIELD_UNIT_STATE_BATCH_NOT kBatchCopy = Reread(kBatch);
    CHECK(kBatchCopy.m_uiTick == 99u);
    REQUIRE(kBatchCopy.m_vecUnit.size() == 3);
    for (int i = 0; i < 3; ++i)
    {
        CHECK(kBatchCopy.m_vecUnit[i].m_UnitUID == kBatch.m_vecUnit[i].m_UnitUID);
        CHECK(kBatchCopy.m_vecUnit[i].m_uiTick == kBatch.m_vecUnit[i].m_uiTick);
        CheckSameState(kBatchCopy.m_vecUnit[i].m_kState, kBatch.m_vecUnit[i].m_kState);
    }
}

TEST_CASE("village sync batches grow by a fixed size per unit")
{
    KEGS_FIELD_UNIT_STATE_BATCH_NOT kBatch;
    const size_t uiEmpty = Serialize(kBatch).size();

    KFieldUnitStateEntry kEntry;
    kEntry.m_kState = MakeState(1.f);
    kBatch.m_vecUnit.push_back(kEntry);
    const size_t uiOne = Serialize(kBatch).size();
    kBatch.m_vecUnit.push_back(kEntry);
    const size_t uiTwo = Serialize(kBatch).size();

    const size_t uiPerUnit = uiOne - uiEmpty;
    CHECK(uiTwo - uiOne == uiPerUnit);
    MESSAGE("empty batch: " << uiEmpty << " bytes, per unit: " << uiPerUnit << " bytes");
    // 35 visible units (the high view limit) at 10 batches per second stay under 20 KB/s per client
    CHECK(uiPerUnit * 35 * 10 < 20 * 1024);
}

#endif  // X2_VILLAGE_SYNC
