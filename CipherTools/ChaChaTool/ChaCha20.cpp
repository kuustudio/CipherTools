#include "stdafx.h"
#include "ChaCha20.h"

#ifdef _DEBUG
#pragma comment(lib,"libsodiumD.lib")
#else
#pragma comment(lib,"libsodium.lib")
#endif

ChaCha20::ChaCha20()
{
	crypto_aead_chacha20poly1305_keygen(key);
	randombytes_buf(nonce, sizeof nonce);
}


ChaCha20::~ChaCha20()
{
}

std::string ChaCha20::Getsodium_version()
{
	const char *p = sodium_version_string();
	std::string ss;
	ss.append(p, strlen(p));
	return ss;
}

void ChaCha20::SetKey(const unsigned char *k, const unsigned int len1,const unsigned char *n,const unsigned int len2)
{
	if (len1)
	{
		if (len1 <= crypto_aead_chacha20poly1305_KEYBYTES)
		{
			memset(key, 0, crypto_aead_chacha20poly1305_KEYBYTES);
			memcpy(key, k, len1);
		}
		else
			memcpy(key, k, crypto_aead_chacha20poly1305_KEYBYTES);
	}
	if (len2)
	{
		if (len2 <= crypto_aead_chacha20poly1305_NPUBBYTES&&len2 != 0)
		{
			memset(nonce, 0, crypto_aead_chacha20poly1305_NPUBBYTES);
			memcpy(nonce, n, len2);
		}
		else
			memcpy(nonce, n, crypto_aead_chacha20poly1305_NPUBBYTES);
	}
}

void ChaCha20::Encrypt(unsigned char *srcBuffer, unsigned long long srcLen, unsigned char **dstBuffer, unsigned long long &dstLen)
{
	*dstBuffer = (unsigned char*)malloc(srcLen + 0x10);
	if (crypto_aead_chacha20poly1305_encrypt(*dstBuffer, &dstLen,
		srcBuffer, srcLen,
		0, 0,
		NULL, nonce, key) != 0)
	{
		/* message forged! */
	}
}

void ChaCha20::Decrypt(unsigned char *srcBuffer, unsigned long long srcLen, unsigned char **dstBuffer, unsigned long long &dstLen)
{
	*dstBuffer = (unsigned char*)malloc(srcLen);
	if (crypto_aead_chacha20poly1305_decrypt(*dstBuffer, &dstLen,
		NULL,
		srcBuffer, srcLen,
		0,
		0,
		nonce, key) != 0) {
		/* message forged! */
	}

}

void ChaCha20::DecryptAndNoVerify(unsigned char *srcBuffer, unsigned long long srcLen, unsigned char **dstBuffer, unsigned long long &dstLen)
{
	*dstBuffer = (unsigned char*)malloc(srcLen);

	if (srcLen < crypto_aead_chacha20poly1305_ABYTES) {
		return;
	}
	crypto_stream_chacha20_xor_ic
		(*dstBuffer, srcBuffer, srcLen - crypto_aead_chacha20poly1305_ABYTES, nonce, 1U, key);
	dstLen = srcLen - crypto_aead_chacha20poly1305_ABYTES;

}

void ChaCha20::_u64_le_from_ull(unsigned char out[8U], unsigned long long x)
{
	out[0] = (unsigned char)(x & 0xff); x >>= 8;
	out[1] = (unsigned char)(x & 0xff); x >>= 8;
	out[2] = (unsigned char)(x & 0xff); x >>= 8;
	out[3] = (unsigned char)(x & 0xff); x >>= 8;
	out[4] = (unsigned char)(x & 0xff); x >>= 8;
	out[5] = (unsigned char)(x & 0xff); x >>= 8;
	out[6] = (unsigned char)(x & 0xff); x >>= 8;
	out[7] = (unsigned char)(x & 0xff);
}

void ChaCha20::FreeBuffer(unsigned char **buffer)
{
	if (*buffer != 0)
	{
		free(*buffer);
		*buffer = 0;
	}
}