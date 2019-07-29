#pragma once
#include"openssl//aes.h"
#include"BinaryPadding.h"

//AES的区块长度固定为128 比特，密钥长度则可以是128，192或256比特；
//解密时使用的轮密钥顺序与加密时相反(所以设置密钥函数不同)，但仍使用同样的轮密钥。
//CFB、OFB和CTR模式中解密也都是用的加密器而非解密器,即解密密钥同样使用AES_set_encrypt_key函数

namespace CipherAes
{
	enum CipherMode
	{
		MODE_ECB,
		MODE_CBC,
		MODE_CTR,
		MODE_OFB,
		MODE_CFB
	};

	enum KeyMode
	{
		MODE_128,
		MODE_192,
		MODE_256
	};

	class CAesCrypt :public CBinaryPadding
	{
	public:
		CAesCrypt(CipherMode = MODE_ECB);
		~CAesCrypt(void);
	private:
		AES_KEY			key1;
		AES_KEY			key2;
		unsigned char	ivec[AES_BLOCK_SIZE];
		CipherMode			cMode;
	private:
		const unsigned int block_size = AES_BLOCK_SIZE;
		virtual const unsigned int GetBlockSize(){ return block_size; };
		void SetEncKey(AES_KEY&, unsigned char *sid, unsigned int len);
		void SetDecKey(AES_KEY&, unsigned char *sid, unsigned int len);
		void SetIv(unsigned char *iv, unsigned int len);
	public:
		static std::string GetSSLeay_version();
		void initMode(CipherMode cm, KeyMode km, unsigned char *key, unsigned int kenlen, unsigned char *iv, unsigned int ivlen);
		void Encrypt(unsigned char *, unsigned int, unsigned char*);
		void Decrypt(unsigned char *, unsigned int, unsigned char*);
	};
}


