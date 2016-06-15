//////////////////////////////////////////////////////////////////////
// Hasher.h - part of the CyoHash application
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

#pragma once

//////////////////////////////////////////////////////////////////////

enum HashAlgorithm
{
    md5hash,
    sha1hash,
    sha256hash,
    sha384hash,
    sha512hash,
    crc32
};

__interface IHasher
{
    virtual void Init() = 0;
    virtual void HashBlock( const LPBYTE pBlock, DWORD dwSize ) = 0;
    virtual void Stop() = 0;
    virtual HashAlgorithm GetAlgorithm() const = 0;
    virtual const LPCTSTR GetName() const = 0;
    virtual const char* GetHash() const = 0;
};

//////////////////////////////////////////////////////////////////////

class MD5Hasher : public IHasher
{
public:
    MD5Hasher();
    virtual ~MD5Hasher();

    virtual void Init();
    virtual void HashBlock( const LPBYTE pBlock, DWORD dwSize );
    virtual void Stop();
    virtual HashAlgorithm GetAlgorithm() const { return md5hash; }
    virtual const LPCTSTR GetName() const { return _T("MD5 hash"); }
    virtual const char* GetHash() const { return m_strHash.c_str(); }

private:
    static const DWORD MD5_HASH_SIZE = 16; //MD5 = 128 bits = 16 bytes

    HCRYPTPROV m_hProv;
    HCRYPTHASH m_hHash;
    std::string m_strHash;

    void Destroy();
};

//////////////////////////////////////////////////////////////////////

class SHAHasher : public IHasher
{
public:
    SHAHasher( HashAlgorithm alg, bool base16 = false );
    virtual ~SHAHasher();

    virtual void Init();
    virtual void HashBlock( const LPBYTE pBlock, DWORD dwSize );
    virtual void Stop();
    virtual HashAlgorithm GetAlgorithm() const { return m_alg; }
    virtual const LPCTSTR GetName() const;
    virtual const char* GetHash() const { return m_strHash.c_str(); }

private:
    static const DWORD SHA1_HASH_SIZE = 20; //SHA1 = 160 bits = 20 bytes
    static const DWORD SHA256_HASH_SIZE = 32; //SHA256 = 256 bits = 32 bytes
    static const DWORD SHA384_HASH_SIZE = 48; //SHA384 = 384 bits = 48 bytes
    static const DWORD SHA512_HASH_SIZE = 64; //SHA512 = 512 bits = 64 bytes

    HashAlgorithm m_alg;
    bool m_base16;
    DWORD m_size;
    HCRYPTPROV m_hProv;
    HCRYPTHASH m_hHash;
    std::string m_strHash;

    void Destroy();
};

//////////////////////////////////////////////////////////////////////

class CRC32Hasher : public IHasher
{
public:
    CRC32Hasher() { }
    virtual ~CRC32Hasher() { }

    virtual void Init();
    virtual void HashBlock( const LPBYTE pBlock, DWORD dwSize );
    virtual void Stop();
    virtual HashAlgorithm GetAlgorithm() const { return crc32; }
    virtual const LPCTSTR GetName() const { return _T("CRC32 checksum"); }
    virtual const char* GetHash() const { return m_strHash.c_str(); }

private:
    std::string m_strHash;
    DWORD m_table[ 256 ];
    DWORD m_crc;

    DWORD Reflect( DWORD value, int ch ) const;
};
