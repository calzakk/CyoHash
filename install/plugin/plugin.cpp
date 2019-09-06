//////////////////////////////////////////////////////////////////////
// plugin.cpp - part of the CyoHash application
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

#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS
#include <atlbase.h>
#include <ShellAPI.h>
#include <Richedit.h>
#include <VersionHelpers.h>
#include <string>
#include <fstream>

//////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain( HANDLE hInstDLL, DWORD dwReason, LPVOID lpReserved )
{
#ifdef _DEBUG
    ::MessageBoxA( NULL, "Debug version", "CyoHash installer plugin", MB_OK );
#endif //_DEBUG

    return TRUE;
}

//////////////////////////////////////////////////////////////////////

namespace
{
    //from nsis:system:plugin.h {
    typedef struct _stack_t {
        struct _stack_t *next;
        char text[1]; // this should be the length of string_size
    } stack_t;

    #define PLUGINFUNCTION(name) void name( HWND, int string_size, char *variables, stack_t** stacktop ) { \
        g_stringsize=string_size; \
        g_stacktop=stacktop; \
        g_variables=variables;

    #define PLUGINFUNCTIONEND }
    //}

    //from nsis:system:plugin.c {
    int g_stringsize;
    stack_t **g_stacktop;
    char *g_variables;

    char *AllocString()
    {
        return (char*) GlobalAlloc(GPTR,g_stringsize);
    }

    char* popstring()
    {
        char *str;
        stack_t *th;

        if (!g_stacktop || !*g_stacktop) return NULL;
        th=(*g_stacktop);

        str = AllocString();
        lstrcpyA(str,th->text); //was lstrcpy

        *g_stacktop = th->next;
        GlobalFree((HGLOBAL)th);
        return str;
    }

    char *pushstring(char *str)
    {
        stack_t *th;
        if (!g_stacktop) return str;
        th=(stack_t*)GlobalAlloc(GPTR,sizeof(stack_t)+g_stringsize);
        lstrcpynA(th->text,str,g_stringsize); //was lstrcpyn
        th->next=*g_stacktop;
        *g_stacktop=th;
        return str;
    }
    //}
}

//////////////////////////////////////////////////////////////////////

namespace detail
{
    bool IsWindowsX64()
    {
        bool bWindowsX64 = false;

        HMODULE hLib = ::LoadLibrary( _T("KERNEL32.DLL") );
        if (hLib != NULL)
        {
            typedef void (WINAPI *GNSI)( LPSYSTEM_INFO );
            GNSI GetNativeSystemInfo = (GNSI)::GetProcAddress( hLib, "GetNativeSystemInfo" );
            if (GetNativeSystemInfo != NULL) //GetNativeSystemInfo isn't available on Win2000
            {
                SYSTEM_INFO si = { 0 };
                GetNativeSystemInfo( &si );
                bWindowsX64 = (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
            }

            ::FreeLibrary( hLib );
            hLib = NULL;
        }

        return bWindowsX64;
    }

    bool SwitchDllsOnReboot( const std::string& instdir )
    {
        // Delete the existing dll...
        if (!::MoveFileExA( (instdir + "\\CyoHash.dll").c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT ))
            return false;

        // Rename the new dll...
        if (!::MoveFileExA( (instdir + "\\CyoHash0.dll").c_str(), (instdir + "\\CyoHash.dll").c_str(), MOVEFILE_DELAY_UNTIL_REBOOT ))
            return false;

        // Success
        return true;
    }

    bool InitRichEditControl( const std::string& wnd, const std::string& pathname )
    {
        HWND hWnd = (HWND)atoi( wnd.c_str() );

        ::SendMessageW( hWnd, EM_SETEVENTMASK, 0, (LPARAM)ENM_LINK );

        std::ifstream file( pathname.c_str() );
        if (!file.is_open())
            return false;

        CHARFORMAT2 cfDefault;
        cfDefault.cbSize = sizeof( cfDefault );
        ::SendMessageW( hWnd, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cfDefault );

        CHARFORMAT2 cfHeader1 = cfDefault;
        cfHeader1.dwMask |= (CFM_BOLD | CFM_SIZE);
        cfHeader1.dwEffects |= CFE_BOLD;
        cfHeader1.yHeight = (cfHeader1.yHeight * 5) / 4; //a quarter bigger

        CHARFORMAT2 cfHeader2 = cfDefault;
        cfHeader2.dwMask |= (CFM_BOLD | CFM_SIZE);
        cfHeader2.dwEffects |= CFE_BOLD;
        cfHeader2.yHeight = (cfHeader2.yHeight * 9) / 8; //an eighth bigger

        CHARFORMAT2 cfLink = cfDefault;
        cfLink.dwMask |= CFM_LINK;
        cfLink.dwEffects |= CFE_LINK;

        while (true)
        {
            std::string line;
            std::getline( file, line );
            if (file.fail())
                break;

            line += "\n";

            if (line.length() >= 3 && line[ 0 ] == '-' && line[ 1 ] == '-' && line[ 2 ] == '-')
            {
                ::SendMessageA( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfHeader1 );
                ::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)line.substr( 3 ).c_str() );
                ::SendMessageA( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfDefault );
            }
            else if (line.length() >= 2 && line[ 0 ] == '-' && line[ 1 ] == '-')
            {
                ::SendMessageA( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfHeader2 );
                ::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)line.substr( 2 ).c_str() );
                ::SendMessageA( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfDefault );
            }
            else
            {
                while (!line.empty())
                {
                    std::string::size_type linkStart = line.find( '[' );
                    if (linkStart == std::string::npos)
                        break;
                    std::string::size_type linkEnd = line.find( ']', linkStart );
                    if (linkEnd == std::string::npos)
                        break;

                    std::string line1 = line.substr( 0, linkStart );
                    if (!line1.empty())
                        ::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)line1.c_str() );

                    std::string link = line.substr( linkStart + 1, linkEnd - linkStart - 1 );
                    if (!link.empty())
                    {
                        ::SendMessageA( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfLink );
                        ::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)link.c_str() );
                        ::SendMessageA( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfDefault );
                    }

                    line.erase( 0, linkEnd + 1 );
                }

                if (!line.empty())
                    ::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)line.c_str() );
            }
        }

        // Success
        return true;
    }

    void OnNotify( const std::string& wnd, const std::string& code, const std::string& nmhdr )
    {
        if (atoi( code.c_str() ) != EN_LINK)
            return;

        ENLINK* enlink = (ENLINK*)atoi( nmhdr.c_str() );
        if (enlink->msg != WM_LBUTTONDOWN)
            return;

        HWND hWnd = (HWND)atoi( wnd.c_str() );

        TEXTRANGEA tr = { 0 };
        tr.chrg = enlink->chrg;
        char szText[ 1024 ];
        tr.lpstrText = szText;
        LRESULT lresult = ::SendMessageA( hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr );

        ::ShellExecuteA( NULL, "open", szText, NULL, NULL, SW_SHOWNORMAL );
    }

} //namespace

//////////////////////////////////////////////////////////////////////

extern "C" {

PLUGINFUNCTION( IsWindowsX64 )
{
    if (detail::IsWindowsX64())
        pushstring( "1" );
    else
        pushstring( "0" );
}
PLUGINFUNCTIONEND

PLUGINFUNCTION( ValidatePlatform )
{
    if (::IsWindows7OrGreater())
        pushstring( "1" );
    else
        pushstring( "0" );
}
PLUGINFUNCTIONEND

PLUGINFUNCTION( SwitchDllsOnReboot )
{
    if (detail::SwitchDllsOnReboot( popstring() ))
        pushstring( "1" );
    else
        pushstring( "0" );
}
PLUGINFUNCTIONEND

PLUGINFUNCTION( InitRichEditControl )
{
    const std::string wnd = popstring();
    const std::string pathname = popstring();
    if (detail::InitRichEditControl( wnd, pathname ))
        pushstring( "1" );
    else
        pushstring( "0" );
}
PLUGINFUNCTIONEND

PLUGINFUNCTION( OnNotify )
{
    const std::string wnd = popstring();
    const std::string code = popstring();
    const std::string nmhdr = popstring();
    detail::OnNotify( wnd, code, nmhdr );
}
PLUGINFUNCTIONEND

}
