//////////////////////////////////////////////////////////////////////
// ShellExtension.h - part of the CyoHash application
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

#include "Resource.h"
#include "CyoHash.h"

//////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE CShellExtension : 
    public CComObjectRootEx< CComSingleThreadModel >,
    public CComCoClass< CShellExtension, &CLSID_ShellExtension >,
    public IDispatchImpl< IShellExtension, &IID_IShellExtension, &LIBID_CyoHashLib >,
    public IShellExtInit,
    public IContextMenu
{
    // Construction/Destruction
public:
    CShellExtension() { }

DECLARE_REGISTRY_RESOURCEID( IDR_SHELLEXTENSION )

BEGIN_COM_MAP( CShellExtension )
    COM_INTERFACE_ENTRY( IShellExtension )
    COM_INTERFACE_ENTRY( IDispatch )
    COM_INTERFACE_ENTRY( IShellExtInit )
    COM_INTERFACE_ENTRY( IContextMenu )
END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct();
    void FinalRelease();

public:
    // IShellExtInit
    STDMETHOD( Initialize )( LPCITEMIDLIST, LPDATAOBJECT pDataObj, HKEY );

    // IContextMenu
    STDMETHOD( GetCommandString )( UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax );
    STDMETHOD( InvokeCommand )( LPCMINVOKECOMMANDINFO pCmdInfo );
    STDMETHOD( QueryContextMenu )( HMENU hMenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags );

private:
    // Types
    typedef std::list<std::wstring> stringlist;

    // Data
    HBITMAP m_hBitmap;
    stringlist m_files;
    HMENU m_hSubMenu;
    UINT m_uidFirstCmd;
    bool m_extraAlgorithms;

    // Implementation
    void StoreLastAlgorithm(std::wstring algorithm) const;
    std::wstring GetLastAlgorithm() const;
    HMENU CreateSubMenu(UINT uidFirstCmd, UINT& nextCmd) const;
};

OBJECT_ENTRY_AUTO( __uuidof( ShellExtension ), CShellExtension )
