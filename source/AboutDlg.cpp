//////////////////////////////////////////////////////////////////////
// AboutDlg.cpp - part of the CyoHash application
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
#include "AboutDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction

AboutDlg::AboutDlg()
{
}

AboutDlg::~AboutDlg()
{
}

//////////////////////////////////////////////////////////////////////
// Message Handlers

LRESULT AboutDlg::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    EndDialog( IDCANCEL );
    
    bHandled = TRUE;
    return 0;
}

LRESULT AboutDlg::OnClickedCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
    EndDialog( IDCANCEL );

    bHandled = TRUE;
    return 0;
}

LRESULT AboutDlg::OnNMClickProjectLink( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled )
{
    wchar_t szUrl[ 256 ];
    CWindow wnd = GetDlgItem( IDC_PROJECTLINK );
    wnd.GetWindowTextW( szUrl, _countof( szUrl ));

    CStringW url = szUrl;
    if (url.Left( 3 ) == L"<a>")
        url.Delete( 0, 3 );
    if (url.Right( 4 ) == L"</a>")
        url.Truncate( url.GetLength() - 4 );

    ::ShellExecute( m_hWnd, _T("open"), url, NULL, NULL, SW_SHOWNORMAL );

    bHandled = TRUE;
    return 0;
}
