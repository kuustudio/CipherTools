#pragma once
#include<string>


namespace CipherKey
{
	class CKeyExpand
	{
	public:
		CKeyExpand();
		~CKeyExpand();
	public:
		unsigned int GetAesKeyLen(unsigned int bits);
		unsigned int GetDesKeyLen();
		void GetKey_AES(unsigned char *userkey, unsigned int keylen,unsigned int bits, bool isEnc,unsigned char *buffer, unsigned int *len);
		void GetKey_DES(unsigned char *userkey, unsigned int keylen, bool isEnc, unsigned char *buffer, unsigned int *len);
		//完整数据还原(128)
		bool GetAesKeyFromExpand(unsigned char *RoundKey, unsigned int *len,unsigned int bits,bool isEnc, unsigned char *userkey);
		//仅用0x10,0x18,0x20数据还原
		bool GetAesKeyFromExpand128(unsigned char *RoundKey, unsigned int *len, unsigned int bits, bool isEnc, unsigned char *userkey);
		//仅用0x10,0x18,0x20数据还原
		bool GetAesKeyFromExpand192(unsigned char *RoundKey, unsigned int *len, unsigned int bits, bool isEnc, unsigned char *userkey);
		//仅用0x10,0x18,0x20数据还原
		bool GetAesKeyFromExpand256(unsigned char *RoundKey, unsigned int *len, unsigned int bits, bool isEnc, unsigned char *userkey);
		//从扩散解密密钥变换为扩散加密密钥
		void DecKeyEd2EncKeyEd(unsigned char *dec, unsigned int bits, unsigned int *enc);
	private:
		unsigned char getSBoxValue(unsigned char c);
		unsigned char getRSBoxValue(unsigned char c);
	public:
		static std::string GetSSLeay_version();
	};
}


