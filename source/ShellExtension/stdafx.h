//////////////////////////////////////////////////////////////////////
// stdafx.h - part of the CyoHash application
//
// Copyright (c) Graham Bull. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////

#pragma once

#define STRICT

#define WIN32_LEAN_AND_MEAN

#ifndef NTTI_VISTA
#   define NTTI_VISTA           0x06000000
#endif
#ifndef _WIN32_WINNT_VISTA
#   define _WIN32_WINNT_VISTA   0x0600
#endif
#ifndef _WIN32_IE_IE70
#   define _WIN32_IE_IE70       0x0700
#endif
#ifndef _WIN32_IE_VISTA
#   define _WIN32_IE_VISTA      _WIN32_IE_IE70
#endif

#define NTDDI_VERSION   NTTI_VISTA
#define WINVER          _WIN32_WINNT_VISTA
#define _WIN32_WINNT    _WIN32_WINNT_VISTA
#define _WIN32_IE       _WIN32_IE_VISTA

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS
#define _ATL_NO_UUIDOF //prevents error C2787: 'IContextMenu' : no GUID has been associated with this object

#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlenc.h>
#include <atlstr.h>
#include <atlwin.h>
using namespace ATL;

#include <comdef.h>
#include <shellapi.h>

#pragma warning(disable : 4091) //prevents warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared
#include <shlobj.h>

#include <cassert>
#include <list>
#include <memory>
#include <stdexcept>
#include <vector>

#pragma warning(disable : 4640) //prevents warning C4640: '_Static': construction of local static object is not thread-safe
#include <sstream>
#include <string>
