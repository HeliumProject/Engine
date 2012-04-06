/*
 * Copyright 2003 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * I18NAPI.h - API support for charset conversion identifiers
 *
 */

class CharSetApi {
public:
    /*
     * The CharSet enum provides the values for the ClientApi::SetTrans
     * api call.  You may need to cast to (int)
     */

    enum CharSet {
	NOCONV = 0, UTF_8, ISO8859_1, UTF_16, SHIFTJIS, EUCJP,
	WIN_US_ANSI, WIN_US_OEM, MACOS_ROMAN, ISO8859_15, ISO8859_5,
	KOI8_R, WIN_CP_1251, UTF_16_LE, UTF_16_BE,
	UTF_16_LE_BOM, UTF_16_BE_BOM, UTF_16_BOM, UTF_8_BOM, UTF_32,
	UTF_32_LE, UTF_32_BE, UTF_32_LE_BOM, UTF_32_BE_BOM, UTF_32_BOM,
	UTF_8_UNCHECKED, UTF_8_UNCHECKED_BOM, CP949, CP936, CP950
    };

    static CharSet Lookup(const char *);

    static const char *Name(CharSet);

    static int Granularity(CharSet);

    static int isUnicode(CharSet);
};
