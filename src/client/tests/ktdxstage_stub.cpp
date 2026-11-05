// CKTDXStage's out-of-line members, for the tests of KTDXLIB pieces built before the engine. The real ones
// (KTDXStage.cpp, KTDXDeviceHolder.cpp) need the device manager, which is not ported yet; these do what they would do
// for a stage that never loaded a device.

#include "KTDXStage.h"

struct CKTDXDeviceData {};

CKTDXDeviceHolder::CKTDXDeviceHolder() = default;
CKTDXDeviceHolder::~CKTDXDeviceHolder() = default;

CKTDXStage::~CKTDXStage()
{
    m_eStageState = SS_NULL;
}
