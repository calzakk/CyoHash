//////////////////////////////////////////////////////////////////////
// CyoHash.cpp - part of the CyoHash application
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
#include "CyoHashDlg2.h"
#include "Hasher.h"

//////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
{
    try
    {
        CStringW strPathname = L"";
        CStringW strAlgorithm = L"";

        if (__argc == 3)
        {
            strPathname = __wargv[ 1 ];
            strAlgorithm = __wargv[ 2 ];

            CStringW algUpper = strAlgorithm;
            algUpper.MakeUpper();
            if (algUpper.Find( L"/ALG=" ) == 0)
                strAlgorithm.Delete( 0, 5 );
        }
        else
        {
            BrowseDlg dlg( strPathname, strAlgorithm );
            if (dlg.DoModal() != IDOK)
                return 1;
        }

        const LPCWSTR c_pszPipeName = L"\\\\.\\pipe\\CyoHash-1392D898-DCE7-4B8A-A43F-4CDA7FD87297";

        const LPCWSTR c_pszDialogReady = L"CyoHash-6638A302-2551-453E-BE1E-393BFE8B58D3";
        HANDLE hDialogReadyEvent = ::CreateEventW( NULL, TRUE, FALSE, c_pszDialogReady );

        const LPCWSTR c_pszMutexName = L"CyoHash-922802A2-5476-4988-8D62-8ADDE8FE9F5E";
        HANDLE hMutex = ::CreateMutexW( NULL, TRUE, c_pszMutexName );

        //if (hMutex != NULL && ::GetLastError() != ERROR_ALREADY_EXISTS)
        if (::WaitForSingleObject( hMutex, 0 ) == WAIT_OBJECT_0)
        {
            //::OutputDebugStringW( L"Creating dialog...\n" );

            // The dialog doesn't already exist, create it...
            CyoHashDlg2 dlg( c_pszPipeName, hDialogReadyEvent, strPathname, strAlgorithm );
            dlg.DoModal();

            // The dialog was closed, allow another to be recreated...
            ::ReleaseMutex( hMutex );
            hMutex = NULL;
        }
        else
        {
            // The dialog already exists, wait until it's ready (up to one minute)...
            if (::WaitForSingleObject( hDialogReadyEvent, 60000 ) != WAIT_OBJECT_0)
            {
                ::MessageBoxW( NULL, L"The CyoHash server is not responding!", L"CyoHash", MB_OK | MB_ICONSTOP );
                return 1;
            }

            //::OutputDebugStringW( L"Dialog already exists\n" );

            // Synchronize pipe connection attempts...
            const LPCWSTR c_pszConnectionMutexName = L"CyoHash-E9AC27BD-4E86-4201-83D6-DFF0249C57D9";
            HANDLE hConnectionMutex = ::CreateMutexW( NULL, FALSE, c_pszConnectionMutexName );
            if (::WaitForSingleObject( hConnectionMutex, 60000 ) != WAIT_OBJECT_0)
            {
                ::MessageBoxW( NULL, L"Unable to connect to the CyoHash server!", L"CyoHash", MB_OK | MB_ICONSTOP );
                return 1;
            }

            // Connect to the pipe...
            HANDLE hPipe = INVALID_HANDLE_VALUE;
            while (true)
            {
                hPipe = ::CreateFileW( c_pszPipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
                if (hPipe != INVALID_HANDLE_VALUE)
                {
                    //::OutputDebugStringW( L"Connected to pipe\n" );
                    break;
                }
                if ((::GetLastError() != ERROR_PIPE_BUSY) || !::WaitNamedPipeW( c_pszPipeName, 30000 ))
                {
                    //::OutputDebugStringW( L"ERROR: Unable to connect to pipe" );
                    return 1;
                }
            }

            //::OutputDebugStringW( L"Sending data...\n" );

            // Change to message-read mode...
            DWORD dwMode = PIPE_READMODE_MESSAGE;
            if (!::SetNamedPipeHandleState( hPipe, &dwMode, NULL, NULL ))
                return 1;

            // Send a message detailing the file to be hashed and the hash algorithm...
            CStringW data = (strPathname + L"\n" + strAlgorithm);
            CStrBufW buf( data );
            DWORD dwBytes = (data.GetLength() * sizeof( wchar_t ));
            DWORD dwBytesWritten = 0;
            if (!::WriteFile( hPipe, (LPCWSTR)buf, dwBytes, &dwBytesWritten, NULL ))
                return 1;

            ::CloseHandle( hPipe );
            hPipe = NULL;

            //::OutputDebugStringW( L"Pipe closed\n" );

            ::Sleep( 200 );

            ::ReleaseMutex( hConnectionMutex ); //allow another instance to connect to the pipe
            hConnectionMutex = NULL;
        }

        return 0;
    }
    catch (const std::runtime_error& ex)
    {
        ::MessageBoxA( NULL, ex.what(), "CyoHash", MB_OK | MB_ICONSTOP );
        return 1;
    }
}
