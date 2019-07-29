#include "stdafx.h"
#include "KeyExpand.h"
#include <openssl/crypto.h>
#include<openssl\aes.h>
#include<openssl\des.h>

using namespace CipherKey;

#ifdef _DEBUG
#pragma comment(lib,"ssleay32MDd.lib")
#pragma comment(lib,"libeay32MDd.lib")
#else
#pragma comment(lib,"ssleay32MD.lib")
#pragma comment(lib,"libeay32MD.lib")
#endif

const unsigned char sbox[256] = {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

const const char rsbox[256] =
{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

const unsigned char Rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb };

CKeyExpand::CKeyExpand()
{
}

CKeyExpand::~CKeyExpand()
{
}

unsigned int CKeyExpand::GetAesKeyLen(unsigned int bits)
{
	if (bits == 128)
		return 0xa0;
	else if (bits == 192)
		return 0xb8;
	else if (bits == 256)
		return 0xd0;
	return -1;
}

void CKeyExpand::GetKey_AES(unsigned char *userkey, unsigned int keylen,unsigned int bits, bool isEnc, unsigned char *buffer, unsigned int *len)
{
	AES_KEY key;

	unsigned char *tempsid = NULL;
	if (bits == 128)
	{
		tempsid = new unsigned char[0x10];
		memset(tempsid, 0, 0x10);
		if (keylen > 0x10)
			memcpy(tempsid, userkey, 0x10);
		else
			memcpy(tempsid, userkey, keylen);
	}
	else if (bits == 192)
	{
		tempsid = new unsigned char[0x18];
		memset(tempsid, 0, 0x18);
		if (keylen > 0x18)
			memcpy(tempsid, userkey, 0x18);
		else
			memcpy(tempsid, userkey, keylen);
	}
	else if (bits == 256)
	{
		tempsid = new unsigned char[0x20];
		memset(tempsid, 0, 0x20);
		if (keylen > 0x20)
			memcpy(tempsid, userkey, 0x20);
		else
			memcpy(tempsid, userkey, keylen);
	}

	if (isEnc)
		AES_set_encrypt_key(tempsid, bits, &key);
	else
		AES_set_decrypt_key(tempsid, bits, &key);
	if (bits == 128 && isEnc)
		memcpy(buffer, (char*)&key.rd_key + AES_BLOCK_SIZE, *len);
	else if (bits == 128 && !isEnc)
		memcpy(buffer, (char*)&key.rd_key, *len);
	else if (bits==192&&isEnc)
		memcpy(buffer, (char*)&key.rd_key + AES_BLOCK_SIZE + 0x8, *len);
	else if (bits == 192 && !isEnc)
		memcpy(buffer, (char*)&key.rd_key, *len);
	else if (bits == 256 && isEnc)
		memcpy(buffer, (char*)&key.rd_key + AES_BLOCK_SIZE + 0x10, *len);
	else if (bits == 256 && !isEnc)
		memcpy(buffer, (char*)&key.rd_key, *len);
	delete[]tempsid;
}

unsigned int CKeyExpand::GetDesKeyLen()
{
	return sizeof(DES_key_schedule);
}

void CKeyExpand::GetKey_DES(unsigned char *userkey, unsigned int keylen, bool isEnc, unsigned char *buffer, unsigned int *len)
{
	DES_key_schedule	sched;
	if (keylen > DES_KEY_SZ) keylen = DES_KEY_SZ;
	DES_cblock block;
	memset(block, 0, DES_KEY_SZ);
	memcpy(block, userkey, keylen);
	DES_set_key((const_DES_cblock*)block, &sched);
	memcpy(buffer, &sched, *len);
}

unsigned char CKeyExpand::getSBoxValue(unsigned char c)
{
	return sbox[c];
}

unsigned char CKeyExpand::getRSBoxValue(unsigned char c)
{
	return rsbox[c];
}

bool CKeyExpand::GetAesKeyFromExpand128(unsigned char *RoundKey, unsigned int *len, unsigned int bits, bool isEnc, unsigned char *userkey)
{
	unsigned char temp[AES_BLOCK_SIZE] = { 0 };
	unsigned int round = 10;
	if (isEnc)
	{
		//这里采用第一行数据还原，如果采用最后一行，那么和解密还原一样
		if (*len == 0x10 || *len == 0xa0)
		{
			memcpy(temp, RoundKey, AES_BLOCK_SIZE);
		}
		else
			return false;
		//加密还原
		for (unsigned char i = AES_BLOCK_SIZE - 1; i > 3; --i)
		{
			userkey[i] = temp[i] ^ temp[i - 4];
		}
		unsigned char tempa[4] = { 0 };
		tempa[0] = userkey[0xd];
		tempa[1] = userkey[0xe];
		tempa[2] = userkey[0xf];
		tempa[3] = userkey[0xc];

		tempa[0] = getSBoxValue(tempa[0]);
		tempa[1] = getSBoxValue(tempa[1]);
		tempa[2] = getSBoxValue(tempa[2]);
		tempa[3] = getSBoxValue(tempa[3]);
		tempa[0] = tempa[0] ^ Rcon[1];
		userkey[0] = tempa[0] ^ temp[0];
		userkey[1] = tempa[1] ^ temp[1];
		userkey[2] = tempa[2] ^ temp[2];
		userkey[3] = tempa[3] ^ temp[3];
	}
	else
	{
		//这里采用第一行还原
		if (*len == 0x10 || *len == 0xa0)
		{
			memcpy(temp, RoundKey, AES_BLOCK_SIZE);
		}
		else
			return false;
		//解密还原
		for (int k = round; k > 0; k--)
		{
			for (unsigned char i = AES_BLOCK_SIZE - 1; i > 3; --i)
			{
				userkey[i] = temp[i] ^ temp[i - 4];
			}
			unsigned char tempa[4] = { 0 };
			tempa[0] = userkey[0xd];
			tempa[1] = userkey[0xe];
			tempa[2] = userkey[0xf];
			tempa[3] = userkey[0xc];

			tempa[0] = getSBoxValue(tempa[0]);
			tempa[1] = getSBoxValue(tempa[1]);
			tempa[2] = getSBoxValue(tempa[2]);
			tempa[3] = getSBoxValue(tempa[3]);
			tempa[0] = tempa[0] ^ Rcon[k];
			userkey[0] = tempa[0] ^ temp[0];
			userkey[1] = tempa[1] ^ temp[1];
			userkey[2] = tempa[2] ^ temp[2];
			userkey[3] = tempa[3] ^ temp[3];
			memcpy(temp, userkey, AES_BLOCK_SIZE);
		}
	}
	return true;
}

bool CKeyExpand::GetAesKeyFromExpand192(unsigned char *RoundKey, unsigned int *len, unsigned int bits, bool isEnc, unsigned char *userkey)
{
	unsigned char temp[AES_BLOCK_SIZE + 8] = { 0 };
	unsigned int round = 12;
	if (isEnc)
	{
		//这里采用第一行数据还原，如果采用最后一行，那么和解密还原一样
		if (*len == 0x18 || *len == 0xb8)
		{
			memcpy(temp, RoundKey, AES_BLOCK_SIZE + 8);
		}
		else
			return false;

		for (unsigned char i = AES_BLOCK_SIZE + 8 - 1; i > 3; --i)
		{
			userkey[i] = temp[i] ^ temp[i - 4];
		}
		unsigned char tempa[4] = { 0 };
		tempa[0] = userkey[0x15];
		tempa[1] = userkey[0x16];
		tempa[2] = userkey[0x17];
		tempa[3] = userkey[0x14];

		tempa[0] = getSBoxValue(tempa[0]);
		tempa[1] = getSBoxValue(tempa[1]);
		tempa[2] = getSBoxValue(tempa[2]);
		tempa[3] = getSBoxValue(tempa[3]);
		tempa[0] = tempa[0] ^ Rcon[1];
		userkey[0] = tempa[0] ^ temp[0];
		userkey[1] = tempa[1] ^ temp[1];
		userkey[2] = tempa[2] ^ temp[2];
		userkey[3] = tempa[3] ^ temp[3];
	}
	else
	{
		//这里采用第一行数据还原
		if (*len == 0x18 || *len == 0xb8)
		{
			memcpy(temp, RoundKey, AES_BLOCK_SIZE + 8);
		}
		else
			return false;
		for (int k = round; k > 0; k--)
		{
			for (unsigned char i = AES_BLOCK_SIZE + 8 - 1; i > 3; --i)
			{
				userkey[i] = temp[i] ^ temp[i - 4];
			}
			unsigned char tempa[4] = { 0 };
			tempa[0] = userkey[0x15];
			tempa[1] = userkey[0x16];
			tempa[2] = userkey[0x17];
			tempa[3] = userkey[0x14];

			tempa[0] = getSBoxValue(tempa[0]);
			tempa[1] = getSBoxValue(tempa[1]);
			tempa[2] = getSBoxValue(tempa[2]);
			tempa[3] = getSBoxValue(tempa[3]);
			tempa[0] = tempa[0] ^ Rcon[k];
			userkey[0] = tempa[0] ^ temp[0];
			userkey[1] = tempa[1] ^ temp[1];
			userkey[2] = tempa[2] ^ temp[2];
			userkey[3] = tempa[3] ^ temp[3];
		}
	}

	

	

	return true;
}

bool CKeyExpand::GetAesKeyFromExpand256(unsigned char *RoundKey, unsigned int *len, unsigned int bits, bool isEnc, unsigned char *userkey)
{
	unsigned char temp[AES_BLOCK_SIZE *2] = { 0 };
	memcpy(temp, RoundKey, AES_BLOCK_SIZE *2);
	if (*len != 0x20) return false;

	for (unsigned char i = AES_BLOCK_SIZE - 1; i > 3; --i)
	{
		userkey[i] = temp[i] ^ temp[i - 4];
	}
	for (unsigned char i = AES_BLOCK_SIZE*2 - 1; i > 0x13; --i)
	{
		userkey[i] = temp[i] ^ temp[i - 4];
	}

	unsigned char tempa[4] = { 0 };
	tempa[0] = userkey[0x1d];
	tempa[1] = userkey[0x1e];
	tempa[2] = userkey[0x1f];
	tempa[3] = userkey[0x1c];

	tempa[0] = getSBoxValue(tempa[0]);
	tempa[1] = getSBoxValue(tempa[1]);
	tempa[2] = getSBoxValue(tempa[2]);
	tempa[3] = getSBoxValue(tempa[3]);
	tempa[0] = tempa[0] ^ Rcon[1];
	userkey[0] = tempa[0] ^ temp[0];
	userkey[1] = tempa[1] ^ temp[1];
	userkey[2] = tempa[2] ^ temp[2];
	userkey[3] = tempa[3] ^ temp[3];

	tempa[0] = RoundKey[0xc];
	tempa[1] = RoundKey[0xd];
	tempa[2] = RoundKey[0xe];
	tempa[3] = RoundKey[0xf];
	tempa[0] = getSBoxValue(tempa[0]);
	tempa[1] = getSBoxValue(tempa[1]);
	tempa[2] = getSBoxValue(tempa[2]);
	tempa[3] = getSBoxValue(tempa[3]);
	userkey[0x10] = tempa[0] ^ temp[0x10];
	userkey[0x11] = tempa[1] ^ temp[0x11];
	userkey[0x12] = tempa[2] ^ temp[0x12];
	userkey[0x13] = tempa[3] ^ temp[0x13];

	return true;
}

void CKeyExpand::DecKeyEd2EncKeyEd(unsigned char *dec, unsigned int bits, unsigned int *enc)
{
	unsigned char buffer[] = { 0x59, 0xC7, 0x43, 0xB4, 0xDA, 0x78, 0x8D, 0xE2, 0x9A, 0xBD, 0x18, 0xA7, 0xD5, 0x27, 0x3C, 0x99 };
	dec = buffer;
	unsigned int round = 0;
	if (bits == 128)
	{
		round = 10;
		for (int i = 1; i < 0xa; i++)
		{
			unsigned char tempa[4] = { 0 };
			tempa[0] = dec[AES_BLOCK_SIZE*i];
			tempa[1] = dec[AES_BLOCK_SIZE*i+1];
			tempa[2] = dec[AES_BLOCK_SIZE*i+2];
			tempa[3] = dec[AES_BLOCK_SIZE*i+3];

			tempa[0] = getSBoxValue(tempa[0]);
			tempa[1] = getSBoxValue(tempa[1]);
			tempa[2] = getSBoxValue(tempa[2]);
			tempa[3] = getSBoxValue(tempa[3]);
		}
	}
	else if (bits == 192)
	{
		round = 12;
	}
	else if (bits == 256)
	{
		round = 14;
	}
}

std::string CKeyExpand::GetSSLeay_version()
{
	const char *p = SSLeay_version(SSLEAY_VERSION);
	std::string ss;
	ss.append(p, strlen(p));
	return ss;
}