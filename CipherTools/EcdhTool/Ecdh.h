#pragma once
#include<openssl\ec.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define ptkey_len	0x19
#define key_len		0x18
#define ECDH_SIZE 33  

class CEcdh
{
public:
	CEcdh(void);
	~CEcdh(void);
private:
	unsigned int nid;    //Õ÷‘≤≤Œ ˝
public:
	void ecdh_nid(unsigned int _nid){ nid = _nid; };
	BOOL ComputeShareKey(unsigned char *PRI_KEY, unsigned int PRI_KEY_LEN, unsigned char * PUB_KEY, unsigned int PUB_KEY_LEN, 
		unsigned char *SHARE_KEY, unsigned int *SHARE_KEY_LEN);
	BOOL GenerateECKey(unsigned char *pri_key, unsigned int *pri_key_len, unsigned char *pub_key, unsigned int *pub_key_len, bool _point_compress=true);
};

