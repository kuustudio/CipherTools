#include "StdAfx.h"
#include <openssl\err.h>
#include <openssl\ecdsa.h>
#include <openssl\ec.h>
#include <openssl\ecdh.h>
#include<openssl\ssl.h>

#ifdef _DEBUG
#pragma comment(lib,"ssleay32MDd.lib")
#pragma comment(lib,"libeay32MDd.lib")
#else
#pragma comment(lib,"ssleay32MD.lib")
#pragma comment(lib,"libeay32MD.lib")
#endif

#include "Ecdh.h"


CEcdh::CEcdh(void)
{
	nid = NID_secp192k1;
}


CEcdh::~CEcdh(void)
{
}

BOOL CEcdh::ComputeShareKey(unsigned char *PRI_KEY, unsigned int PRI_KEY_LEN, unsigned char * PUB_KEY, unsigned int PUB_KEY_LEN,
	unsigned char *SHARE_KEY, unsigned int *SHARE_KEY_LEN)
{
	EC_KEY *ec_key = EC_KEY_new_by_curve_name(nid);

	BIGNUM *privatekey = 0;
	privatekey = BN_bin2bn(PRI_KEY, PRI_KEY_LEN, privatekey);
	int ret=EC_KEY_set_private_key(ec_key, privatekey);
	if (ret)
	{
		const EC_GROUP *ec_group = EC_KEY_get0_group(ec_key);
		EC_POINT *ec_point = EC_POINT_new(ec_group);
		if (EC_POINT_oct2point(ec_group, ec_point, PUB_KEY, PUB_KEY_LEN, NULL) == 1)
		{
			*SHARE_KEY_LEN = ECDH_compute_key(SHARE_KEY, *SHARE_KEY_LEN, ec_point, ec_key, 0);
			if (*SHARE_KEY_LEN != 0)
			{
				EC_KEY_free(ec_key);
				EC_POINT_free(ec_point);
				return TRUE;
			}
		}
		EC_POINT_free(ec_point);
	}
	EC_KEY_free(ec_key);
	return FALSE;
}

BOOL CEcdh::GenerateECKey(unsigned char *pri_key, unsigned int *pri_key_len, unsigned char *pub_key, unsigned int *pub_key_len, bool _point_compress)
{
	point_conversion_form_t point_form_t;
	if (_point_compress)
		point_form_t = POINT_CONVERSION_COMPRESSED;
	else
		point_form_t = POINT_CONVERSION_UNCOMPRESSED;

	EC_KEY *ec_key = EC_KEY_new_by_curve_name(nid);
	const EC_GROUP *ec_group = EC_KEY_get0_group(ec_key);

	if (EC_KEY_generate_key(ec_key) == 1)
	{
		const BIGNUM *privateKey = EC_KEY_get0_private_key(ec_key);
		//*pri_key_len = i2d_ECPrivateKey(ec_key, &pri_key);//保存整个ec_key数据
		*pri_key_len= BN_bn2bin(privateKey, pri_key);
		const EC_POINT *publicKey = EC_KEY_get0_public_key(ec_key);
		*pub_key_len = EC_POINT_point2oct(ec_group, publicKey, point_form_t, pub_key, *pub_key_len, NULL); //stKey为0x19数据
		return TRUE;
	}
	EC_KEY_free(ec_key);
	return FALSE;
}