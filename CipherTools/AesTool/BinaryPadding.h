#pragma once
#include<Windows.h>
#include<time.h>

class CBinaryPadding
{
public:
	CBinaryPadding()
	{
		dwPaddingLen = 0;
	}
	virtual ~CBinaryPadding()
	{

	}
private:
	unsigned int dwPaddingLen;
private:
	virtual const unsigned int GetBlockSize() = 0;
	void SetPaddingLen(unsigned int l){ dwPaddingLen = l; };
public:
	const unsigned int GetPaddingLen(){ return dwPaddingLen; };
public:
	void GetPadding_zero(unsigned char **srcBuffer, unsigned int *srclen)
	{
		unsigned int dstlen = 0;
		unsigned int paddinglen = 0;
		if (*srclen % GetBlockSize() == 0)
		{
			//不需要填充
			dstlen = *srclen;
			paddinglen = 0;
		}
		else
		{
			//需要填充
			dstlen = *srclen - *srclen % GetBlockSize() + GetBlockSize();
			paddinglen = dstlen - *srclen;
		}

		unsigned char paddingchar = 0x0;
		if (paddinglen == 0)
		{
			paddinglen = GetBlockSize();
			dstlen += GetBlockSize();
			paddingchar = 0x0;
		}
		*srcBuffer = (unsigned char*)realloc(*srcBuffer, dstlen);
		memset(*srcBuffer + *srclen, paddingchar, paddinglen);
		*srclen = dstlen;
		SetPaddingLen(paddinglen);
	}
	void GetPadding_pkcs5(unsigned char **srcBuffer, unsigned int *srclen)
	{
		unsigned int dstlen = 0;
		unsigned int paddinglen = 0;
		if (*srclen % GetBlockSize() == 0)
		{
			//不需要填充
			dstlen = *srclen;
			paddinglen = 0;
		}
		else
		{
			//需要填充
			dstlen = *srclen - *srclen % GetBlockSize() + GetBlockSize();
			paddinglen = dstlen - *srclen;
		}
		unsigned char paddingchar = paddinglen;
		if (paddinglen == 0)
		{
			paddingchar = GetBlockSize();
			paddinglen = GetBlockSize();
			dstlen += GetBlockSize();
		}
		*srcBuffer = (unsigned char*)realloc(*srcBuffer, dstlen);
		memset(*srcBuffer + *srclen, paddingchar, paddinglen);
		*srclen = dstlen;
		SetPaddingLen(paddinglen);
	}
	void GetPadding_pkcs7(unsigned char **srcBuffer, unsigned int *srclen)
	{
		unsigned int dstlen = 0;
		unsigned int paddinglen = 0;
		if (*srclen % GetBlockSize() == 0)
		{
			//不需要填充
			dstlen = *srclen;
			paddinglen = 0;
		}
		else
		{
			//需要填充
			dstlen = *srclen - *srclen % GetBlockSize() + GetBlockSize();
			paddinglen = dstlen - *srclen;
		}
		unsigned char paddingchar = paddinglen;
		if (paddinglen == 0)
		{
			paddingchar = GetBlockSize();
			paddinglen = GetBlockSize();
			dstlen += GetBlockSize();
		}
		*srcBuffer = (unsigned char*)realloc(*srcBuffer, dstlen);
		memset(*srcBuffer + *srclen, paddingchar, paddinglen);
		*srclen = dstlen;
		SetPaddingLen(paddinglen);
	}
	void GetPadding_iso10126(unsigned char **srcBuffer, unsigned int *srclen)
	{
		unsigned int dstlen = 0;
		unsigned int paddinglen = 0;
		if (*srclen % GetBlockSize() == 0)
		{
			//不需要填充
			dstlen = *srclen;
			paddinglen = 0;
		}
		else
		{
			//需要填充
			dstlen = *srclen - *srclen % GetBlockSize() + GetBlockSize();
			paddinglen = dstlen - *srclen;
		}
		unsigned char paddingchar = paddinglen;
		if (paddinglen == 0)
		{
			paddingchar = GetBlockSize();
			paddinglen = GetBlockSize();
			dstlen += GetBlockSize();
		}
		*srcBuffer = (unsigned char*)realloc(*srcBuffer, dstlen);
		(*srcBuffer)[dstlen - 1] = paddingchar;
		srand((int)time(0));
		for (int i = 0; i < paddinglen - 1; i++)
		{
			(*srcBuffer)[*srclen + i] = rand();
		}
		*srclen = dstlen;
		SetPaddingLen(paddinglen);
	}
	void GetPadding_ansix923(unsigned char **srcBuffer, unsigned int *srclen)
	{
		unsigned int dstlen = 0;
		unsigned int paddinglen = 0;
		if (*srclen % GetBlockSize() == 0)
		{
			//不需要填充
			dstlen = *srclen;
			paddinglen = 0;
		}
		else
		{
			//需要填充
			dstlen = *srclen - *srclen % GetBlockSize() + GetBlockSize();
			paddinglen = dstlen - *srclen;
		}
		unsigned char paddingchar = paddinglen;
		if (paddinglen == 0)
		{
			paddingchar = GetBlockSize();
			paddinglen = GetBlockSize();
			dstlen += GetBlockSize();
		}
		*srcBuffer = (BYTE*)realloc(*srcBuffer, dstlen);
		(*srcBuffer)[dstlen - 1] = paddingchar;
		memset(*srcBuffer + *srclen, 0x0, paddinglen - 1);
		*srclen = dstlen;
		SetPaddingLen(paddinglen);
	}
};

