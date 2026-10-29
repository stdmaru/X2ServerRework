//#pragma once  // 여러번 include 가능. 대신 #include "ImportKncSDK.h"를 명시한 헤더를 한번만 include되도록 설정하자.

#ifndef KNCLIBNAME
#error ImportKOGSDK.h를 include 하기 전에 KNCLIBNAME 매크로를 정의해 주어야 합니다.
#endif

#define CLVERSION   "71"

#ifdef _MT
#   ifdef _DLL
#       ifdef _DEBUG
#           define CRTTYPE  "MDd"   // _MDd
#       else
#           define CRTTYPE  "MD"    // _MD
#       endif
#   else
#       ifdef _DEBUG
#           define CRTTYPE  "MTd"   // _MTd
#       else
#           define CRTTYPE  "MT"    // _MT
#       endif
#   endif
#else
#   ifdef _DEBUG
#       define CRTTYPE  "MLd"       // _MLd
#   else
#       define CRTTYPE  "ML"        // _ML
#   endif
#endif

#include <iostream>     // a Standard C++ Library Header

#pragma comment( lib, KNCLIBNAME CLVERSION CRTTYPE )
#pragma message( KNCLIBNAME CLVERSION CRTTYPE ".lib is imported" )

#undef KNCLIBNAME
#undef CLVERSION
#undef CRTTYPE
