//////////////////////////////////////////////////////////////////////
// ShellExtension.cpp - part of the CyoHash application
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

#include "stdafx.h"
#include "ShellExtension.h"

//////////////////////////////////////////////////////////////////////
// CShellExtension

HRESULT CShellExtension::FinalConstruct()
{
    m_hBitmap = NULL;
    m_hSubMenu = NULL;

    m_hBitmap = (HBITMAP)::LoadImage( ::GetModuleHandle( _T("CyoHash.dll") ),
        MAKEINTRESOURCE( IDB_CYOHASH ), IMAGE_BITMAP, 16, 16, LR_LOADMAP3DCOLORS );

    return S_OK;
}

void CShellExtension::FinalRelease()
{
    if (m_hBitmap != NULL)
    {
        ::DeleteObject( m_hBitmap );
        m_hBitmap = NULL;
    }

    if (m_hSubMenu != NULL)
    {
        ::DestroyMenu( m_hSubMenu );
        m_hSubMenu = NULL;
    }
}

//////////////////////////////////////////////////////////////////////
// IShellExtInit

STDMETHODIMP CShellExtension::Initialize( LPCITEMIDLIST,
                                          LPDATAOBJECT pDataObj,
                                          HKEY )
{
    struct SGlobalLock
    {
        SGlobalLock( STGMEDIUM& stg ) : _stg( stg ) { }
        virtual ~SGlobalLock() { ::GlobalUnlock( _stg.hGlobal ); ::ReleaseStgMedium( &_stg ); }
    private:
        STGMEDIUM& _stg;
    };

    // Look for CF_HDROP data in the data object...
    FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg = { TYMED_HGLOBAL };
    if (FAILED( pDataObj->GetData( &fmt, &stg )))
        return E_INVALIDARG;

    // Get a pointer to the actual data...
    HDROP hDrop = (HDROP)::GlobalLock( stg.hGlobal );
    if (hDrop == NULL)
        return E_INVALIDARG;
    SGlobalLock globalLock( stg );

    // How many files are selected?
    UINT count = ::DragQueryFileW( hDrop, 0xFFFFFFFF, NULL, 0 );
    if (count == 0)
        return E_INVALIDARG;

    // Get the pathnames for each selected file...
    m_files.clear();
    for (UINT i = 0; i < count; ++i)
    {
        wchar_t szPathname[ MAX_PATH ];
        ::DragQueryFileW( hDrop, i, szPathname, MAX_PATH );
        m_files.push_back( szPathname );
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IContextMenu

STDMETHODIMP CShellExtension::GetCommandString( UINT_PTR idCmd,
                                                UINT uFlags,
                                                UINT* pwReserved,
                                                LPSTR pszName,
                                                UINT cchMax )
{
    if (idCmd == 0)
    {
        if ((uFlags & GCS_HELPTEXT) != 0)
        {
            LPCTSTR pszText = _T("Hash the selected file");
            if ((uFlags & GCS_UNICODE) != 0)
                ::lstrcpynW( (LPWSTR)pszName, CT2CW( pszText ), cchMax );
            else
                ::lstrcpynA( pszName, CT2CA( pszText ), cchMax );

            return S_OK;
        }
    }

    return E_INVALIDARG;
}

STDMETHODIMP CShellExtension::InvokeCommand( LPCMINVOKECOMMANDINFO pCmdInfo )
{
    if (HIWORD( pCmdInfo->lpVerb ) == 0)
    {
        try
        {
            std::wstring algorithm = L"";

            switch (LOWORD(pCmdInfo->lpVerb))
            {
            case 0: algorithm = GetLastAlgorithm(); break;
            case 1: algorithm = L"MD5"; break;
            case 2: algorithm = L"SHA1"; break;
            case 3: algorithm = L"SHA1-BASE32"; break;
            case 4: algorithm = L"SHA256"; break;
            case 5: algorithm = L"SHA384"; break;
            case 6: algorithm = L"SHA512"; break;
            case 7: algorithm = L"CRC32"; break;
            }

            if (!algorithm.empty())
            {
                StoreLastAlgorithm(algorithm.c_str());

                ATL::CRegKey reg;
                if (reg.Open( HKEY_LOCAL_MACHINE, _T("Software\\CyoHash"), KEY_READ ) != ERROR_SUCCESS)
                    throw std::runtime_error( "Cannot open registry key" );
                ULONG size = 0;
                if (reg.QueryStringValue( NULL, NULL, &size ) != ERROR_SUCCESS)
                    throw std::runtime_error( "Cannot determine registry value's size" );
                std::auto_ptr<TCHAR> buffer( new TCHAR[ size ]);
                if (reg.QueryStringValue( NULL, buffer.get(), &size ) != ERROR_SUCCESS)
                    throw std::runtime_error( "Cannot read registry value" );

                int succeeded = 0;
                int failed = 0;
                for (stringlist::const_iterator i = m_files.begin(); i != m_files.end(); ++i)
                {
                    std::wostringstream cmdline;
                    cmdline << L"\"" << (LPCWSTR)ATL::CT2W( buffer.get() ) << L"\\CyoHash.exe\" \""
                        << *i << L"\" /alg=" << algorithm;

                    STARTUPINFOW si = { 0 };
                    si.cb = sizeof( si );
                    PROCESS_INFORMATION pi = { 0 };
                    if (::CreateProcessW( NULL, (LPWSTR)cmdline.str().c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ))
                        ++succeeded;
                    else
                        ++failed;
                }
                if (failed >= 1 && succeeded == 0)
                    throw std::runtime_error( "Unable to spawn CyoHash.exe" );

                return S_OK;
            }
        }
        catch (const std::runtime_error& ex)
        {
            ::MessageBoxA( pCmdInfo->hwnd, ex.what(), "CyoHash", MB_OK | MB_ICONERROR );
            return E_UNEXPECTED;
        }
    }

    return E_INVALIDARG;
}

STDMETHODIMP CShellExtension::QueryContextMenu( HMENU hMenu,
                                                UINT uMenuIndex,
                                                UINT uidFirstCmd,
                                                UINT uidLastCmd,
                                                UINT uFlags )
{
    if ((uFlags & CMF_DEFAULTONLY) != 0)
        return MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

    UINT nextPos = uMenuIndex;
    UINT nextCmd = m_uidFirstCmd = uidFirstCmd;

    // Provide a one-click hashing option...
    UINT posOneClick = -1;
    BOOL bOneClick = FALSE;
    std::wstring lastAlgorithm = GetLastAlgorithm();
    if (!lastAlgorithm.empty())
    {
        CStringW str;
        str.Format(L"Cyo&Hash (%s)", lastAlgorithm.c_str());
        posOneClick = nextPos++;
        if (!::InsertMenuW(hMenu, posOneClick, MF_BYPOSITION | MF_STRING, nextCmd, str))
            posOneClick = -1;
    }
    ++nextCmd;

    // Destroy any existing submenu...
    if (m_hSubMenu != NULL)
        ::DestroyMenu(m_hSubMenu);

    // Create the new submenu...
    UINT posMain = -1;
    m_hSubMenu = CreateSubMenu(uidFirstCmd, nextCmd);
    if (m_hSubMenu != NULL)
    {
        posMain = nextPos++;
        if (!::InsertMenuW(hMenu, posMain, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)m_hSubMenu, L"Cyo&Hash"))
            posMain = -1;
    }

    // Add icons...
    if (m_hBitmap != NULL)
    {
        MENUITEMINFO info = { 0 };
        info.cbSize = sizeof( info );
        info.fMask = MIIM_BITMAP;
        info.hbmpItem = (HBITMAP)m_hBitmap;
        int offset = 0;
        if (posOneClick != -1)
            ::SetMenuItemInfo(hMenu, posOneClick, MF_BYPOSITION, &info);
        if (posMain != -1)
            ::SetMenuItemInfo(hMenu, posMain, MF_BYPOSITION, &info);
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, nextCmd - uidFirstCmd + 1);
}

//////////////////////////////////////////////////////////////////////
// Implementation

void CShellExtension::StoreLastAlgorithm(std::wstring algorithm) const
{
    ATL::CRegKey reg;
    if (reg.Open(HKEY_CURRENT_USER, _T("Software\\CyoHash"), KEY_SET_VALUE) == ERROR_SUCCESS)
        reg.SetStringValue(_T("last"), ATL::CW2T(algorithm.c_str()));
}

std::wstring CShellExtension::GetLastAlgorithm() const
{
    ATL::CRegKey reg;
    if (reg.Open(HKEY_CURRENT_USER, _T("Software\\CyoHash"), KEY_QUERY_VALUE) != ERROR_SUCCESS)
        return L"";
    ULONG size = 0;
    if (reg.QueryStringValue(_T("last"), NULL, &size) != ERROR_SUCCESS)
        return L"";
    std::vector<TCHAR> buffer(size); //size includes space for a terminator
    if (reg.QueryStringValue(_T("last"), &buffer.front(), &size) != ERROR_SUCCESS)
        return L"";
    return std::wstring(ATL::CT2W(&buffer.front()), size);
}

HMENU CShellExtension::CreateSubMenu(UINT uidFirstCmd, UINT& nextCmd) const
{
    HMENU hSubMenu = ::CreatePopupMenu();
    if (hSubMenu == NULL)
        return NULL;

    int index = 0;
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("MD5"));
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("SHA1"));
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("SHA1(base32)"));
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("SHA256"));
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("SHA384"));
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("SHA512"));
    ::InsertMenu(hSubMenu, index++, MF_BYPOSITION | MF_STRING, nextCmd++, _T("CRC32"));

    return hSubMenu;
}
