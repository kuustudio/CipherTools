#include "stdafx.h"
#include<vector>
#include "AesCrypt.h"
#include <openssl/crypto.h>

using namespace CipherAes;

#ifdef _DEBUG
#pragma comment(lib,"ssleay32MDd.lib")
#pragma comment(lib,"libeay32MDd.lib")
#else
#pragma comment(lib,"ssleay32MD.lib")
#pragma comment(lib,"libeay32MD.lib")
#endif


CAesCrypt::CAesCrypt(CipherMode m) :cMode(m)
{
}

CAesCrypt::~CAesCrypt()
{
}

std::string CAesCrypt::GetSSLeay_version()
{
	const char *p = SSLeay_version(SSLEAY_VERSION);
	std::string ss;
	ss.append(p, strlen(p));
	return ss;
}

void CAesCrypt::initMode(CipherMode cm, KeyMode km, unsigned char *key, unsigned int keylen, unsigned char *iv, unsigned int ivlen)
{
	cMode = cm;
	if (km == MODE_128)
	{
		if (keylen > AES_BLOCK_SIZE) keylen = AES_BLOCK_SIZE;
		unsigned char tempsid[AES_BLOCK_SIZE] = { 0 };
		memcpy(tempsid, key, keylen);
		SetEncKey(key1, key, AES_BLOCK_SIZE * 8);
		SetDecKey(key2, key, AES_BLOCK_SIZE * 8);
	}
	else if (km == MODE_192)
	{
		if (keylen > AES_BLOCK_SIZE + 8) keylen = AES_BLOCK_SIZE + 8;
		unsigned char tempsid[AES_BLOCK_SIZE + 8] = { 0 };
		memcpy(tempsid, key, keylen);
		SetEncKey(key1, key, (AES_BLOCK_SIZE+8) * 8);
		SetDecKey(key2, key, (AES_BLOCK_SIZE+8) * 8);
	}
	else if (km == MODE_256)
	{
		if (keylen > AES_BLOCK_SIZE * 2) keylen = AES_BLOCK_SIZE * 2;
		unsigned char tempsid[AES_BLOCK_SIZE * 2] = { 0 };
		memcpy(tempsid, key, keylen);
		SetEncKey(key1, key, (AES_BLOCK_SIZE *2) * 8);
		SetDecKey(key2, key, (AES_BLOCK_SIZE *2) * 8);
	}
	SetIv(iv, ivlen);
}

void CAesCrypt::SetEncKey(AES_KEY &key, unsigned char *sid, unsigned int len)
{
	AES_set_encrypt_key(sid, len, &key);
}

void CAesCrypt::SetDecKey(AES_KEY &key, unsigned char *sid, unsigned int len)
{
	AES_set_decrypt_key(sid,  len, &key);
}

void CAesCrypt::SetIv(unsigned char *iv, unsigned int len)
{
	ASSERT(len <= AES_BLOCK_SIZE);
	memset(ivec, 0, AES_BLOCK_SIZE);
	memcpy(ivec, iv, len);
}

void CAesCrypt::Encrypt(unsigned char *src, unsigned int slen, unsigned char *dst)
{
	switch (cMode)
	{
	case MODE_ECB:
	{
		unsigned char *inputText = new unsigned char[block_size];
		unsigned char *outputText = new unsigned char[block_size];
		for (unsigned int i = 0; i<slen / block_size; i++)
		{
			memcpy(inputText, src + i * block_size, block_size);
			AES_ecb_encrypt(inputText, outputText, &key1,AES_ENCRYPT);
			memcpy(dst + i * block_size, outputText, block_size);
		}
		delete[]inputText;
		delete[]outputText;
	}break;
	case MODE_CBC:
	{
		AES_cbc_encrypt(src, dst, slen, &key1, ivec, AES_ENCRYPT);
	}break;
	case MODE_CFB:
	{
		int num = 0;
		//for (unsigned int index = 0; index < slen; ++index)
		//{
		//	AES_cfb128_encrypt(&src[index], dst + index, 1, &key1, ivec, &num, AES_ENCRYPT);
		//}
		AES_cfb8_encrypt(src, dst, slen, &key1, ivec, &num, AES_ENCRYPT);
	}break;
	}
}

void CAesCrypt::Decrypt(unsigned char *src, unsigned int slen, unsigned char *dst)
{
	switch (cMode)
	{
	case MODE_ECB:
	{
		unsigned char *inputText = new unsigned char[block_size];
		unsigned char *outputText = new unsigned char[block_size];
		for (unsigned int i = 0; i<slen / block_size; i++)
		{
			memcpy(inputText, src + i * block_size, block_size);
			AES_ecb_encrypt(inputText, outputText, &key2, AES_DECRYPT);
			memcpy(dst + i * block_size, outputText, block_size);
		}
		delete[]inputText;
		delete[]outputText;
	}break;
	case MODE_CBC:
	{
		AES_cbc_encrypt(src, dst, slen, &key2, ivec, AES_DECRYPT);
	}break;
	case MODE_CFB:
	{
		int num = 0;
		//for (unsigned int index = 0; index < slen; ++index) 
		//{
		//	AES_cfb8_encrypt(&src[index], dst+index, 1, &key2, ivec, &num, AES_DECRYPT);
		//}
		AES_cfb8_encrypt(src, dst , slen, &key1, ivec, &num, AES_DECRYPT);
	}break;
	}
}