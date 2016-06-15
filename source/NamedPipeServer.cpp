//////////////////////////////////////////////////////////////////////
// CyoHashDlg2.cpp - part of the CyoHash application
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
#include "NamedPipeServer.h"

//////////////////////////////////////////////////////////////////////
// Construction

NamedPipeServer::NamedPipeServer()
:   m_hPipe( INVALID_HANDLE_VALUE )
{
}

NamedPipeServer::~NamedPipeServer()
{
}

//////////////////////////////////////////////////////////////////////
// Operations

void NamedPipeServer::Run( LPCWSTR pipeName, INamedPipeServerCallback* callback, HANDLE exitEvent )
{
    try
    {
        HANDLE hEvent = ::CreateEventW( NULL, TRUE, TRUE, NULL );
        ATLASSERT( hEvent != NULL );

        OVERLAPPED ov = { 0 };
        ov.hEvent = hEvent;

        CreatePipe( pipeName );
        bool pendingIO = WaitForClientToConnect( ov );

        callback->OnPipeReady();

        HANDLE handles[ 2 ] = { ov.hEvent, exitEvent };

        for (int i = 0; ; ++i)
        //while (true)
        {
            DWORD res = ::WaitForMultipleObjectsEx( 2, handles, FALSE, INFINITE, TRUE );
            if (::WaitForSingleObject( exitEvent, 0 ) == WAIT_OBJECT_0)
                break;
            switch (res)
            {
            case WAIT_OBJECT_0:
                {
                    if (pendingIO)
                    {
                        DWORD bytesRead;
                        if (!::GetOverlappedResult( m_hPipe, &ov, &bytesRead, FALSE ))
                        {
                            //::OutputDebugStringW( L"ERROR: GetOverlappedResult failed\n" );
                            throw;
                        }
                    }

                    PipeData* pipeData = (PipeData*)::GlobalAlloc( GPTR, sizeof( PipeData ));
                    if (pipeData == NULL)
                    {
                        //::OutputDebugStringW( L"ERROR: GlobalAlloc failed\n" );
                        throw;
                    }
                    pipeData->hPipe = m_hPipe;
                    pipeData->callback = callback;
                    if (!::ReadFileEx( m_hPipe, pipeData->buffer, BufferSize, (LPOVERLAPPED)pipeData, &MessageReceived ))
                    {
                        DestroyPipe( pipeData );
                        pipeData = NULL;
                    }

                    CreatePipe( pipeName );
                    pendingIO = WaitForClientToConnect( ov );
                }
                break;

            case WAIT_IO_COMPLETION:
                break;

            default:
                //::OutputDebugStringW( L"ERROR: WaitForMultipleObjectEx failed\n" );
                throw;
            }
        }
    }
    catch (...)
    {
        //::OutputDebugStringW( L"Unhandled exception in NamedPipeServer!\n" );
    }
}

//////////////////////////////////////////////////////////////////////
// Implementation

void NamedPipeServer::CreatePipe( LPCWSTR pipeName )
{
    m_hPipe = ::CreateNamedPipeW( pipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BufferSize, BufferSize, 5000, NULL );
    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        //::OutputDebugStringW( L"ERROR: CreateNamedPipe failed\n" );
        throw;
    }
}

bool NamedPipeServer::WaitForClientToConnect( OVERLAPPED& ov ) const
{
    if (::ConnectNamedPipe( m_hPipe, &ov )) //this should return FALSE!
    {
        //::OutputDebugStringW( L"ERROR: ConnectNamedPipe failed\n" );
        throw;
    }

    DWORD error = ::GetLastError();
    switch (error)
    {
    case ERROR_IO_PENDING:
        return true;

    case ERROR_PIPE_CONNECTED:
        if (::SetEvent( ov.hEvent ))
            return false;

    default:
        //::OutputDebugStringW( L"ERROR: ConnectNamedPipe failed\n" );
        throw;
    }
}

void NamedPipeServer::DestroyPipe( PipeData* pipeData )
{
    ::DisconnectNamedPipe( m_hPipe );
    ::CloseHandle( m_hPipe );
    m_hPipe = INVALID_HANDLE_VALUE;

    ::GlobalFree( (HGLOBAL)pipeData );
}

void WINAPI NamedPipeServer::MessageReceived( DWORD error, DWORD bytesRead, LPOVERLAPPED ov )
{
    PipeData* pipeData = (PipeData*)ov;
    pipeData->callback->OnMessageReceived( pipeData->buffer, bytesRead );
}
