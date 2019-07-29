#pragma once

#define BUF_SIZE 65535

class CInflate
{
public:
	CInflate();
	~CInflate();
public:
	Bytef *odata;
	uLong nodata = 0;
private:
	void init(){
		if (odata)
			this->~CInflate();
	};
	int zcompress(Bytef *data, uLong ndata, Bytef *zdata, uLong *nzdata);
	int gzcompress(Bytef *data, uLong ndata, Bytef *zdata, uLong *nzdata);
	int zdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata);
	int httpgzdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata);
	int gzdecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata);
	//bool LzmaCompress(const char*scrfilename, const char*desfilename);
	//bool LzmaUncompress(const char*scrfilename, const char*desfilename);
public:
	int zcompress(Bytef *data, uLong ndata){
		init();
		nodata = ndata * 2;
		odata = (Bytef*)malloc(nodata);
		memset(odata, 0, nodata);
		return zcompress(data,ndata,odata,&nodata); 
	};
	int zdecompress(Byte *zdata, uLong nzdata, uLong dstLen = BUF_SIZE){
		init();
		odata = (Bytef*)malloc(dstLen);
		memset(odata, 0, dstLen);
		nodata = dstLen;
		return zdecompress(zdata, nzdata, odata, &nodata);
	};

	int gzcompress(Bytef *data, uLong ndata){ 
		init();
		nodata = ndata * 2;
		odata = (Bytef*)malloc(nodata);
		memset(odata, 0, nodata);
		return gzcompress(data, ndata, odata, &nodata); };
	int gzdecompress(Byte *zdata, uLong nzdata, uLong dstLen = BUF_SIZE){
		init();
		odata = (Bytef*)malloc(dstLen);
		memset(odata, 0, dstLen);
		nodata = dstLen;
		return gzdecompress(zdata, nzdata, odata, &nodata);
	};

	int httpgzdecompress(Byte *zdata, uLong nzdata, uLong dstLen = BUF_SIZE){
		init();
		odata = (Bytef*)malloc(dstLen);
		memset(odata, 0, dstLen);
		nodata = dstLen;
		return httpgzdecompress(zdata, nzdata, odata, &nodata); };

	int lzmacompress(Bytef *data, uLong ndata);
	int lzmauncompress(Byte *zdata, uLong nzdata, uLong dstLen = BUF_SIZE);

};

