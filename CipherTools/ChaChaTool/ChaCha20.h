#pragma once
#include<sodium.h>
#include<string>
//
//#define crypto_aead_chacha20poly1305_KEYBYTES	0x20
//#define crypto_aead_chacha20poly1305_NPUBBYTES	0x8
//#define crypto_aead_chacha20poly1305_ABYTES		0x0

class ChaCha20
{
public:
	ChaCha20();
	~ChaCha20();
private:
	unsigned char key[crypto_aead_chacha20poly1305_KEYBYTES];
	unsigned char nonce[crypto_aead_chacha20poly1305_NPUBBYTES];
public:
	static std::string Getsodium_version();
	void SetKey(const unsigned char *key, const unsigned int len1,const unsigned char *nonce, const unsigned int len2);
	void Encrypt(unsigned char *srcBuffer,unsigned long long srcLen,unsigned char **dstBuffer,unsigned long long &dstLen);
	void Decrypt(unsigned char *srcBuffer, unsigned long long srcLen, unsigned char **dstBuffer, unsigned long long &dstLen);
	void DecryptAndNoVerify(unsigned char *srcBuffer, unsigned long long srcLen, unsigned char **dstBuffer, unsigned long long &dstLen);
	void _u64_le_from_ull(unsigned char out[8U], unsigned long long x);
	void FreeBuffer(unsigned char **buffer);
};

