#pragma once
class ResTableConfig
{
public:

	ResTableConfig()
	{
	}

	virtual ~ResTableConfig()
	{
	}
public:
	/*int size;
	int imsi;
	int locale;
	int screentype;
	int input;
	int screensize;
	int version;
	int screenconfig;
	int screensizedp;
	int localescript;
	int localevariantH;
	int localevariantL;*/
	int size;
	union {
		struct {
			// Mobile country code (from SIM).  0 means "any".
			short mcc;
			// Mobile network code (from SIM).  0 means "any".
			short mnc;
		};
		int imsi;
	};
	union {
		struct {
			char language[2];
			char country[2];
		};
		int locale;
	};
	union {
		struct {
			char orientation;
			char touchscreen;
			short density;
		};
		int screenType;
	};
	union {
		struct {
			char keyboard;
			char navigation;
			char inputFlags;
			char inputPad0;
		};
		int input;
	};
	union {
		struct {
			short screenWidth;
			short screenHeight;
		};
		int screenSize;
	};
	union {
		struct {
			short sdkVersion;
			// For now minorVersion must always be 0!!!  Its meaning
			// is currently undefined.
			short minorVersion;
		};
		int version;
	};
	union {
		struct {
			char screenLayout;
			char uiMode;
			short smallestScreenWidthDp;
		};
		int screenConfig;
	};

	union {
		struct {
			short screenWidthDp;
			short screenHeightDp;
		};
		int screenSizeDp;
	};
	char localeScript[4];
	char localeVariant[8];

public:
	char* GetLanguage()
	{
		if (locale == 0){
			return "英文";
		}
		else if (locale == 1313040506){
			return "简体中文";
		}
		else if (locale == 1465149562){
			return "繁体中文";
		}
		else{
			return "娜美克星文";
		}
	}
};

