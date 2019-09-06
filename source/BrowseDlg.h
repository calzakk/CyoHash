//////////////////////////////////////////////////////////////////////
// BrowseDlg.h - part of the CyoHash application
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

#include "resource.h"

//////////////////////////////////////////////////////////////////////

class BrowseDlg : public CAxDialogImpl< BrowseDlg >
{
public:

    // Construction/Destruction
    BrowseDlg( CString& strPathname, CString& strAlgorithm );
    ~BrowseDlg();

    enum { IDD = IDD_BROWSE };

    // Message Map
BEGIN_MSG_MAP( BrowseDlg )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    MESSAGE_HANDLER( WM_CLOSE, OnClose )
    COMMAND_HANDLER( IDCANCEL, BN_CLICKED, OnClickedCancel )
    COMMAND_HANDLER( IDOK, BN_CLICKED, OnClickedOK )
    COMMAND_HANDLER( IDC_BROWSE, BN_CLICKED, OnClickedBrowse )
    COMMAND_HANDLER( IDC_PATHNAME, EN_CHANGE, OnEnChangePathname )
    CHAIN_MSG_MAP( CAxDialogImpl< BrowseDlg >)
END_MSG_MAP()

    // Message Handlers
    LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnClickedCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnClickedOK( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnClickedBrowse( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnEnChangePathname( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

private:

    // Types
    typedef std::vector<CString> Algorithms;

    // Data
    HICON m_hIcon;
    CString& m_strPathname;
    CString& m_strAlgorithm;
    Algorithms m_algorithms;
};
