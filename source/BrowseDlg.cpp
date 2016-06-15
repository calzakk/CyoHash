//////////////////////////////////////////////////////////////////////
// BrowseDlg.cpp - part of the CyoHash application
//
// Copyright (c) 2009-2016, Graham Bull.
// All rights reserved.
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
#include "BrowseDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction

BrowseDlg::BrowseDlg( CString& strPathname, CString& strAlgorithm )
:   m_hIcon( NULL ),
    m_strPathname( strPathname ),
    m_strAlgorithm( strAlgorithm )
{
    OSVERSIONINFOEX vi = { 0 };
    vi.dwOSVersionInfoSize = sizeof( vi );
    bool extraAlgorithms = false;
    if (   ::GetVersionEx( (LPOSVERSIONINFO)&vi )
        && (   (vi.dwMajorVersion >= 6) //Vista+
            || (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 2) //2003 or XP64
            || (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 1 && vi.wServicePackMajor >= 3))) //XP SP3+
        extraAlgorithms = true;

    int numAlgorithms = (extraAlgorithms ? 7 : 4);
    m_algorithms.resize( numAlgorithms );
    int index = 0;
    m_algorithms[ index++ ] = _T("MD5");
    m_algorithms[ index++ ] = _T("SHA1");
    m_algorithms[ index++ ] = _T("SHA1-base32");
    if (extraAlgorithms)
    {
        m_algorithms[ index++ ] = _T("SHA256");
        m_algorithms[ index++ ] = _T("SHA384");
        m_algorithms[ index++ ] = _T("SHA512");
    }
    m_algorithms[ index++ ] = _T("CRC32");
    assert( index == numAlgorithms );
}

BrowseDlg::~BrowseDlg()
{
    if (m_hIcon != NULL)
        ::DestroyIcon( m_hIcon );
}

//////////////////////////////////////////////////////////////////////
// Message Handlers

LRESULT BrowseDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    CAxDialogImpl< BrowseDlg >::OnInitDialog( uMsg, wParam, lParam, bHandled );

    if (!GetParent().IsWindow())
        CenterWindow();

    m_hIcon = (HICON)::LoadImage( ::GetModuleHandle( NULL ), MAKEINTRESOURCE( IDI_CYOHASH ), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE );
    SetIcon( m_hIcon, TRUE );
    SetIcon( m_hIcon, FALSE );

    for (Algorithms::const_iterator i = m_algorithms.begin(); i != m_algorithms.end(); ++i)
    {
        GetDlgItem( IDC_ALGORITHM ).SendMessage( CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)*i );
    }
    GetDlgItem( IDC_ALGORITHM ).SendMessage( CB_SETCURSEL, 0, 0 ); //select first
    GetDlgItem( IDOK ).EnableWindow( FALSE );
    GetDlgItem( IDCANCEL ).EnableWindow( TRUE );

    bHandled = TRUE;
    return 1;
}

LRESULT BrowseDlg::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    EndDialog( IDCANCEL );
    
    bHandled = TRUE;
    return 0;
}

LRESULT BrowseDlg::OnClickedCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
    EndDialog( IDCANCEL );

    bHandled = TRUE;
    return 0;
}

LRESULT BrowseDlg::OnClickedOK( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
    GetDlgItemText( IDC_PATHNAME, m_strPathname );

    int sel = (int)GetDlgItem( IDC_ALGORITHM ).SendMessage( CB_GETCURSEL, 0, 0 );
    ATLASSERT( 0 <= sel && sel < (int)m_algorithms.size() );
    m_strAlgorithm = m_algorithms[ sel ];

    EndDialog( IDOK );

    bHandled = TRUE;
    return 0;
}

LRESULT BrowseDlg::OnClickedBrowse( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
    const int BufferSize = (16 * 1024); //16KiB
    std::vector<TCHAR> buffer( BufferSize );
    TCHAR* pBuffer = &buffer.front();

    ::OPENFILENAME ofi = { 0 };
    ofi.lStructSize = sizeof( OPENFILENAME );
    ofi.Flags = (OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
        | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ALLOWMULTISELECT);
    ofi.hwndOwner = m_hWnd;
    ofi.lpstrFilter = _T("All files (*.*)\0\0");
    ofi.nFilterIndex = 0;
    ofi.lpstrFile = pBuffer;
    ofi.nMaxFile = BufferSize;
    if (::GetOpenFileName( &ofi ))
    {
        int count = 0;
        for (TCHAR* p = pBuffer;;)
        {
            if (*p++ == _T('\x0'))
            {
                ++count;
                if (*p++ == _T('\x0'))
                    break;
            }
        }
        if (count >= 3)
        {
            // Multiple files
            --count;
            std::vector<TCHAR> pathnames( MAX_PATH * count );
            TCHAR* pszPathnames = &pathnames.front();
            *pszPathnames = _T('\x0');
            TCHAR* next = (pBuffer + ofi.nFileOffset);
            for (int i = 0; i < count; ++i)
            {
                if (i >= 1)
                    _tcscat( pszPathnames, _T("|") );

                _tcscat( pszPathnames, pBuffer );
                _tcscat( pszPathnames, _T("\\") );
                _tcscat( pszPathnames, next );

                while (*next != _T('\x0'))
                    ++next;
                ++next;
            }
            SetDlgItemText( IDC_PATHNAME, pszPathnames );
        }
        else
        {
            // Single file
            ATLASSERT( count == 1 );
            SetDlgItemText( IDC_PATHNAME, pBuffer );
        }
    }

    bHandled = TRUE;
    return 0;
}

LRESULT BrowseDlg::OnEnChangePathname( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
    CString strPathname;
    GetDlgItemText( IDC_PATHNAME, strPathname );

    GetDlgItem( IDOK ).EnableWindow( !strPathname.IsEmpty() );

    bHandled = TRUE;
    return 0;
}
