#pragma once
#include<string>
#include"openssl//des.h"
#include"../AesTool/BinaryPadding.h"

//DES的区块长度固定为64比特，密钥也是64位（实际用到了56位，第8、16、24、32、40、48、56、64位是校验位，
//使得每个密钥都有奇数个1）


namespace CipherDes
{
	enum CipherMode
	{
		MODE_ECB,
		MODE_CBC,
		MODE_CTR,
		MODE_OFB,
		MODE_CFB
	};

	class CDesCrypt :public CBinaryPadding
	{
	public:
		CDesCrypt(CipherMode = MODE_ECB);
		~CDesCrypt(void);
	private:
		DES_key_schedule	sched1;
		DES_cblock			ivec;
		CipherMode				mode;
	private:
		const unsigned int block_size = sizeof(DES_cblock);
		virtual const unsigned int GetBlockSize(){ return block_size; };
	public:
		static std::string GetSSLeay_version();
		void SetMode(CipherMode m){ mode = m; };
		void SetKey(unsigned char *sid, unsigned int len);
		void SetIv(unsigned char *iv, unsigned int len);
		void Encrypt(unsigned char *, unsigned int, unsigned char*);
		void Decrypt(unsigned char *, unsigned int, unsigned char*);
	};
}
