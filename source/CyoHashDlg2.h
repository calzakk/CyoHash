//////////////////////////////////////////////////////////////////////
// CyoHashDlg2.h - part of the CyoHash application
//
// Copyright (c) 2009-2017, Graham Bull.
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

#pragma once

#include "Resource.h"
#include "Messages.h"

//////////////////////////////////////////////////////////////////////

__interface IHasher;

class CyoHashDlg2 : public CAxDialogImpl< CyoHashDlg2 >
{
public:

    // Construction/Destruction
    CyoHashDlg2( LPCWSTR pipeName, HANDLE dialogReadyEvent, LPCWSTR pathname, LPCWSTR algorithm );
    ~CyoHashDlg2();

    enum { IDD = IDD_CYOHASH2 };

    // Message Map
BEGIN_MSG_MAP( CyoHashDlg2 )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    MESSAGE_HANDLER( WM_SIZE, OnSize )
    MESSAGE_HANDLER( WM_DROPFILES, OnDropFiles )
    MESSAGE_HANDLER( WM_CLOSE, OnClose )
    COMMAND_HANDLER( IDCANCEL, BN_CLICKED, OnClickedCancel )
    // Custom Messages
    MESSAGE_HANDLER( WM_HASHING_STARTED, OnHashingStarted )
    MESSAGE_HANDLER( WM_HASHING_PROGRESS, OnHashingProgress )
    MESSAGE_HANDLER( WM_HASHING_PAUSED, OnHashingPaused )
    MESSAGE_HANDLER( WM_HASHING_COMPLETED, OnHashingCompleted )
    MESSAGE_HANDLER( WM_HASHING_CANCELLED, OnHashingCancelled )
    // List Handlers
    NOTIFY_HANDLER( IDC_LIST, NM_DBLCLK, OnDblClkList )
    NOTIFY_HANDLER( IDC_LIST, NM_RCLICK, OnRClickList )
    NOTIFY_HANDLER( IDC_LIST, LVN_KEYDOWN, OnKeyDownList )
    // Menu Handlers
    COMMAND_HANDLER( IDC_MENU_PAUSE, BN_CLICKED, OnMenuPause )
    COMMAND_HANDLER( IDC_MENU_RESUME, BN_CLICKED, OnMenuResume )
    COMMAND_HANDLER( IDC_MENU_CANCEL, BN_CLICKED, OnMenuCancel )
    COMMAND_HANDLER( IDC_MENU_COPYHASH, BN_CLICKED, OnMenuCopyHash )
    COMMAND_HANDLER( IDC_MENU_RECALCULATE, BN_CLICKED, OnMenuRecalculate )
    COMMAND_HANDLER( IDC_MENU_CLEAR, BN_CLICKED, OnMenuClear )
    COMMAND_HANDLER( IDC_MENU_CLEARALL, BN_CLICKED, OnMenuClearAll )
    COMMAND_HANDLER( IDC_MENU_HASHFILE, BN_CLICKED, OnMenuHashFile )
    COMMAND_HANDLER( IDC_MENU_ABOUT, BN_CLICKED, OnMenuAbout )
    COMMAND_HANDLER( IDC_MENU_MD5, BN_CLICKED, OnMenuMD5 )
    COMMAND_HANDLER( IDC_MENU_SHA1, BN_CLICKED, OnMenuSHA1 )
    COMMAND_HANDLER( IDC_MENU_SHA1BASE32, BN_CLICKED, OnMenuSHA1Base32 )
    COMMAND_HANDLER( IDC_MENU_SHA256, BN_CLICKED, OnMenuSHA256 )
    COMMAND_HANDLER( IDC_MENU_SHA384, BN_CLICKED, OnMenuSHA384 )
    COMMAND_HANDLER( IDC_MENU_SHA512, BN_CLICKED, OnMenuSHA512 )
    COMMAND_HANDLER( IDC_MENU_CRC32, BN_CLICKED, OnMenuCRC32 )
    COMMAND_HANDLER( IDC_MENU_UNSORTED, BN_CLICKED, OnMenuUnsorted )
    COMMAND_HANDLER( IDC_MENU_SORTBY_FILE, BN_CLICKED, OnMenuSortByFile )
    COMMAND_HANDLER( IDC_MENU_SORTBY_ALGORITHM, BN_CLICKED, OnMenuSortByAlgorithm )
    COMMAND_HANDLER( IDC_MENU_SORTBY_HASH, BN_CLICKED, OnMenuSortByHash )
    CHAIN_MSG_MAP( CAxDialogImpl< CyoHashDlg2 >)
END_MSG_MAP()

    // Message Handlers
    LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnDropFiles( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnClickedCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    // Custom Messages
    LRESULT OnHashingStarted( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnHashingProgress( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnHashingPaused( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnHashingCompleted( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnHashingCancelled( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    // List Handlers
    LRESULT OnDblClkList( int idCtrl, LPNMHDR pnmh, BOOL& bHandled );
    LRESULT OnRClickList( int idCtrl, LPNMHDR pnmh, BOOL& bHandled );
    LRESULT OnKeyDownList( int idCtrl, LPNMHDR pnmh, BOOL& bHandled );
    // Menu Handlers
    LRESULT OnMenuPause( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuResume( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuCopyHash( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuRecalculate( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuClear( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuClearAll( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuHashFile( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuAbout( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuMD5( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSHA1( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSHA1Base32( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSHA256( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSHA384( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSHA512( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuCRC32( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuUnsorted( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSortByFile( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSortByAlgorithm( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
    LRESULT OnMenuSortByHash( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

    // Operations
    void OnPipeReady();
    void OnMessageReceived( LPBYTE start, DWORD length );

private:

    // Types
    struct ThreadData
    {
        CyoHashDlg2& dlg;
        int key;
        HANDLE readyEvent; //signalled when the thread has grabbed the key
    };
    struct HashData
    {
        CStringW pathname;
        CStringW algorithm;
        CStringW algorithmLong;
        CStringW hash;
        bool splitHash;
        bool paused;
        bool completed;
        bool cancelled;
        HANDLE thread;
        HANDLE pauseEvent;
        HANDLE resumeEvent;
        HANDLE cancelEvent;
    };
    typedef CAtlMap<int, HashData> HashMap;
    typedef std::vector<int> IntVector;
    enum SortBy
    {
        Unsorted,
        SortByFile,
        SortByAlgorithm,
        SortByHash
    };

    // Data
    int m_lastX, m_lastY, m_lastWidth, m_lastHeight;
    CStringW m_pipeName;
    HANDLE m_dialogReadyEvent;
    HANDLE m_pipeReadyEvent;
    HANDLE m_pipeThread;
    HANDLE m_exitEvent;
    HICON m_hIcon;
    HMENU m_hMenu;
    CStringW m_pathname;
    CStringW m_algorithm;
    int m_nextKey;
    ATL::CComCriticalSection m_sync;
    HashMap m_hashData;
    CWindow m_listWnd;
    CAtlList<CStringW> m_droppedFiles;
    SortBy m_sortby;

    // Implementation
    void ReadLastSettings();
    void InitList();
    int ReadIntFromRegistry( HKEY hKey, LPCWSTR name, int default_ );
    void SaveCurrentSettings();
    void WriteIntToRegistry( HKEY hKey, LPCWSTR name, int value );
    void ResizeList();
    int GetItemKey( int item );
    int FindItem( int key );
    void ClearSelectedItems();
    IntVector GetSelectedItems();
    void ShowPopupMenu( int x, int y );
    void HashDroppedFiles( LPCWSTR algorithm );
    static DWORD WINAPI StaticPipeThread( LPVOID param );
    void PipeThread();
    bool BeginHashing( LPCWSTR pathname, LPCWSTR algorithm );
    void BeginHashingFile( LPCWSTR pathname, LPCWSTR algorithm );
    void CountFilesToHash( const CStringW& path, bool recurse, int& numFiles, int& numFolders );
    void FindFilesToHash( const CStringW& path, bool recurse, LPCWSTR algorithm );
    void SortList();
    static int CALLBACK StaticCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );
    int CompareFunc( LPARAM lParam1, LPARAM lParam2 );
    static DWORD WINAPI StaticHashThread( LPVOID param );
    void HashThread( ThreadData* data );

    // Thread-safe functions
    int SafeCreateHashData( LPCWSTR pathname, LPCWSTR algorithm );
    void SafeSetThreadHandle( int key, HANDLE thread );
    HashData SafeGetHashData( int key );
    void SafeSetPaused( int key, bool paused );
    void SafeSetCompleted( int key, IHasher* hasher );
    void SafeSetCancelled( int key );
    void SafeClearAll();
    void SafeJoinAll();
};
