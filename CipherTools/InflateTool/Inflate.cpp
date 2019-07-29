#include "stdafx.h"
#include "Inflate.h"

#ifdef _DEBUG
#pragma comment(lib,"LzmaLibd.lib")
#else
#pragma comment(lib,"LzmaLib.lib")
#endif


CInflate::CInflate()
{
	odata = 0;
	nodata = 0;
}


CInflate::~CInflate()
{
	if (odata)
	{
		free(odata);
		odata = 0;
	}
}

int CInflate::zcompress(Bytef *data, uLong ndata, Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;
	if (data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit(&c_stream, Z_DEFAULT_COMPRESSION) != Z_OK) return -1;
		c_stream.next_in = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}
/* Compress gzip data */
int CInflate::gzcompress(Bytef *data, uLong ndata, Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;
	if (data && ndata > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			-MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}
/* Uncompress data */
int CInflate::zdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream; /* decompression stream */
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if (inflateInit(&d_stream) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK) return -1;
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}
/* HTTP gzip decompress */
int CInflate::httpgzdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] =
	{
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if (inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata)
	{
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK)
		{
			if (err == Z_DATA_ERROR)
			{
				d_stream.next_in = (Bytef*)dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
				{
					return -1;
				}
			}
			else
				return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}
/* Uncompress gzip data */
int CInflate::gzdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] =
	{
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	if (inflateInit2(&d_stream, -MAX_WBITS) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata)
	{
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK)
		{
			if (err == Z_DATA_ERROR)
			{
				d_stream.next_in = (Bytef*)dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
				{
					return -1;
				}
			}
			else
				return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;

}

int CInflate::lzmacompress(Bytef *data, uLong ndata)
{
	init();
	__int64 saveinsize = ndata;//原始数据长度长度
	nodata = saveinsize*1.1 + 1026 * 16;
	odata = (Bytef*)malloc(nodata+13);
	memset(odata, 0, nodata+13);
	unsigned char outProps[0x5] = { 0 };
	size_t outPropsSize = 5;
	unsigned int r=LzmaCompress(odata+13, (size_t*)(&nodata), data, ndata, outProps, &outPropsSize, 5, (1 << 24), 3, 0, 2, 32, 1);
	memcpy(odata, outProps, 5);
	memcpy(odata + 5, &saveinsize, sizeof(saveinsize));
	nodata += 13;
	return r;
}

int CInflate::lzmauncompress(Byte *zdata, uLong nzdata, uLong dstLen)
{
	init();
	odata = (Bytef*)malloc(dstLen);
	memset(odata, 0, dstLen);
	nodata = dstLen;
	unsigned char outProps[0x5] = { 0 };
	memcpy(outProps, zdata, 5);
	size_t outPropsSize = 5;
	__int64 saveinsize = 0;//读取压缩前原始数据长度
	memcpy(&saveinsize, zdata + 5, 8);
	size_t savoutsize = saveinsize;
	nodata = saveinsize;
	nzdata -= 13;
	return LzmaUncompress(odata, &savoutsize, zdata + 13, (size_t*)&nzdata, outProps, outPropsSize);
}


bool LzmaCompress(const char*scrfilename, const char*desfilename){

	FILE *fin;
	fopen_s(&fin, scrfilename, "rb");
	if (fin == NULL){
		printf("Open ScrFile ERR:%s\n", scrfilename);
		return false;
	}
	FILE*fout;
	fopen_s(&fout, desfilename, "wb");
	if (fout == NULL){
		printf("Open DesFile ERR:%s\n", desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin, 0, SEEK_END);
	size_t saveinsize = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	size_t saveoutsize = saveinsize*1.1 + 1026 * 16;
	unsigned char* inbuff = (unsigned char*)malloc(saveinsize);
	unsigned char* outbuff = (unsigned char*)malloc(saveoutsize);
	unsigned char props[5] = { 0 };
	size_t propsSize = 5;
	size_t readlength = fread(inbuff, 1, saveinsize, fin);
	if (readlength != saveinsize){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res = LzmaCompress(outbuff, &saveoutsize, inbuff, saveinsize,
		props, &propsSize,
		5,
		1 << 24,
		3,
		0,
		2,
		32,
		1);
	if (res != 0){
		printf("LzmaCompressErr:%d\n", res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	int zero = 0;
	fwrite(props, 1, propsSize, fout);
	fwrite(&saveinsize, 1, 4, fout);
	fwrite(&zero, 1, 4, fout);
	fwrite(outbuff, 1, saveoutsize, fout);

	fclose(fin);
	fclose(fout);
	return true;
}

bool LzmaUncompress(const char*scrfilename, const char*desfilename){
	FILE*fin;
	fopen_s(&fin, scrfilename, "rb");
	if (fin == NULL){
		printf("Open ScrFile ERR:%s\n", scrfilename);
		return false;
	}
	FILE*fout;
	fopen_s(&fout, desfilename, "wb");
	if (fout == NULL){
		printf("Open DesFile ERR:%s\n", desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin, 0, SEEK_END);
	size_t saveinsize = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	size_t saveoutsize = saveinsize*1.1 + 1026 * 16;
	unsigned char* inbuff = (unsigned char*)malloc(saveinsize);
	unsigned char props[5] = { 0 };
	size_t propsSize = 5;

	fread(props, 1, 5, fin);
	fread(&saveoutsize, 1, 4, fin);
	fseek(fin, 4, SEEK_CUR);
	unsigned char* outbuff = (unsigned char*)malloc(saveoutsize);
	size_t readlength = fread(inbuff, 1, saveinsize - 13, fin);
	if (readlength != (saveinsize - 13)){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res = LzmaUncompress(outbuff, &saveoutsize, inbuff, &readlength,
		props, propsSize);
	if (res != 0){
		printf("LzmaUncompress:%d\n", res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	fwrite(outbuff, 1, saveoutsize, fout);
	fclose(fin);
	fclose(fout);
	return true;
}