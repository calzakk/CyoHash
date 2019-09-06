//////////////////////////////////////////////////////////////////////
// Hasher.cpp - part of the CyoHash application
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
#include "Hasher.h"
#include "Utils.h"
#include "cyoencode/CyoEncode.h"

//////////////////////////////////////////////////////////////////////
// MD5Hasher

MD5Hasher::MD5Hasher()
:   m_hProv( NULL ),
    m_hHash( NULL )
{
}

MD5Hasher::~MD5Hasher()
{
    Destroy();

    assert( m_hProv == NULL );
    assert( m_hHash == NULL );
}

void MD5Hasher::Init()
{
    if (!::CryptAcquireContext( &m_hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ))
        throw std::runtime_error( "Unable to initialise crypto provider" );

    if (!::CryptCreateHash( m_hProv, CALG_MD5, 0, 0, &m_hHash ))
        throw std::runtime_error( "Unable to create MD5 hash" );
}

void MD5Hasher::HashBlock( const LPBYTE pBlock, DWORD dwSize )
{
    assert( m_hHash != NULL );

    if (!::CryptHashData( m_hHash, pBlock, dwSize, 0 ))
        throw std::runtime_error( "Unable to hash data" );
}

void MD5Hasher::Stop()
{
    assert( m_hHash != NULL );

    DWORD dwHashSize = 0;
    DWORD dwDataLen = sizeof( DWORD );
    if (!::CryptGetHashParam( m_hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwDataLen, 0 ))
        throw std::runtime_error( "Unable to determine hash length" );
    utils::ensure< std::runtime_error >( dwHashSize == MD5_HASH_SIZE );

    std::auto_ptr< BYTE > hash( new BYTE[ dwHashSize ]);
    if (!::CryptGetHashParam( m_hHash, HP_HASHVAL, hash.get(), &dwHashSize, 0 ))
        throw std::runtime_error( "Unable to determine hash value" );

    size_t size = cyoBase16EncodeGetLength( dwHashSize );
    std::auto_ptr< char > strHash( new char[ size ]);
    size = cyoBase16Encode( strHash.get(), hash.get(), dwHashSize );
    m_strHash = strHash.get();

    Destroy();
}

void MD5Hasher::Destroy()
{
    if (m_hHash != NULL)
    {
        ::CryptDestroyHash( m_hHash );
        m_hHash = NULL;
    }

    if (m_hProv != NULL)
    {
        ::CryptReleaseContext( m_hProv, 0 );
        m_hProv = NULL;
   } 
}

//////////////////////////////////////////////////////////////////////
// SHAHasher

SHAHasher::SHAHasher( HashAlgorithm alg, bool base16 )
:   m_alg( alg ),
    m_base16( base16 ),
    m_size( 0 ),
    m_hProv( NULL ),
    m_hHash( NULL )
{
    switch (m_alg)
    {
    case sha1hash:   m_size = SHA1_HASH_SIZE;   break;
    case sha256hash: m_size = SHA256_HASH_SIZE; break;
    case sha384hash: m_size = SHA384_HASH_SIZE; break;
    case sha512hash: m_size = SHA512_HASH_SIZE; break;
    default: throw;
    }
}

SHAHasher::~SHAHasher()
{
    Destroy();

    assert( m_hProv == NULL );
    assert( m_hHash == NULL );
}

const LPCTSTR SHAHasher::GetName() const
{
    switch (m_alg)
    {
    case sha1hash:   return (m_base16 ? _T("SHA1 hash") : _T("SHA1 hash (base32)"));
    case sha256hash: return _T("SHA256 hash");
    case sha384hash: return _T("SHA384 hash");
    case sha512hash: return _T("SHA512 hash");
    default: throw;
    }
}

void SHAHasher::Init()
{
    if (!::CryptAcquireContext(&m_hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)
        && !::CryptAcquireContext(&m_hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        throw std::runtime_error("Unable to initialise crypto provider");

    ALG_ID algId;
    switch (m_alg)
    {
    case sha1hash:   algId = CALG_SHA1; break;
    case sha256hash: algId = CALG_SHA_256; break;
    case sha384hash: algId = CALG_SHA_384; break;
    case sha512hash: algId = CALG_SHA_512; break;
    default: throw;
    }
    if (!::CryptCreateHash( m_hProv, algId, 0, 0, &m_hHash ))
        throw std::runtime_error( "Unable to create SHA hash" );
}

void SHAHasher::HashBlock( const LPBYTE pBlock, DWORD dwSize )
{
    assert( m_hHash != NULL );

    if (!::CryptHashData( m_hHash, pBlock, dwSize, 0 ))
        throw std::runtime_error( "Unable to hash data" );
}

void SHAHasher::Stop()
{
    assert( m_hHash != NULL );

    DWORD dwHashSize = 0;
    DWORD dwDataLen = sizeof( DWORD );
    if (!::CryptGetHashParam( m_hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwDataLen, 0 ))
        throw std::runtime_error( "Unable to determine hash length" );
    utils::ensure< std::runtime_error >( dwHashSize == m_size );

    std::auto_ptr< BYTE > hash( new BYTE[ dwHashSize ]);
    if (!::CryptGetHashParam( m_hHash, HP_HASHVAL, hash.get(), &dwHashSize, 0 ))
        throw std::runtime_error( "Unable to determine hash value" );

    size_t size;
    if (m_base16)
        size = cyoBase16EncodeGetLength( dwHashSize );
    else
        size = cyoBase32EncodeGetLength( dwHashSize );
    std::auto_ptr< char > strHash( new char[ size ]);
    if (m_base16)
        size = cyoBase16Encode( strHash.get(), hash.get(), dwHashSize );
    else
        size = cyoBase32Encode( strHash.get(), hash.get(), dwHashSize );
    m_strHash = strHash.get();

    Destroy();
}

void SHAHasher::Destroy()
{
    if (m_hHash != NULL)
    {
        ::CryptDestroyHash( m_hHash );
        m_hHash = NULL;
    }

    if (m_hProv != NULL)
    {
        ::CryptReleaseContext( m_hProv, 0 );
        m_hProv = NULL;
   } 
}

//////////////////////////////////////////////////////////////////////
// CRC32Hasher

void CRC32Hasher::Init()
{
    const DWORD polynomial = 0x04C11DB7;

    for (int i = 0; i < 256; ++i)
    {
        m_table[ i ] = Reflect( i, 8 ) << 24;
        for (int j = 0; j < 8; ++j)
        {
            m_table[ i ] = (m_table[ i ] << 1) ^ ((m_table[ i ] & (1 << 31)) ? polynomial : 0);
        }
        m_table[ i ] = Reflect( m_table[ i ], 32 );
    }

    m_crc = 0xffffffff;
}

DWORD CRC32Hasher::Reflect( DWORD value, int ch ) const
{
    DWORD result = 0;
    for (int i = 1; i < (ch + 1); ++i)
    {
        if (value & 1)
        {
            result |= (1 << (ch - i));
        }
        value >>= 1;
    }
    return result;
}

void CRC32Hasher::HashBlock( const LPBYTE pBlock, DWORD dwSize )
{
    const LPBYTE pEnd = (pBlock + dwSize);
    for (LPBYTE pNext = pBlock; pNext < pEnd; ++pNext)
    {
		 m_crc = ((m_crc >> 8) ^ m_table[ (m_crc & 0xff) ^ *pNext ]);
	}
}

void CRC32Hasher::Stop()
{
    m_crc ^= 0xffffffff;

    char str[ 9 ] = "";
    sprintf_s( str, "%08X", m_crc );
    m_strHash = str;
}
