/*
 * Copyright 2001 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

class CharSetUTF8Valid;
class CharStep;
class StrPtr;
class StrBuf;

/*
 * CharSetCvt.h - Character set converters
 */

class CharSetCvt : public CharSetApi {
public:
    enum Errors {
	NONE = 0, NOMAPPING, PARTIALCHAR
    };

    static CharSetCvt *FindCvt(CharSet from, CharSet to);

    virtual ~CharSetCvt();

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

    virtual int LastErr();

    virtual void ResetErr();

    /* convert buffer into an allocated buffer, caller must free result */
    virtual char *CvtBuffer(const char *, int len, int *retlen = 0);

    /* convert buffer into an managed buffer, caller must copy result
       out before calling this again */
    virtual const char *FastCvt(const char *, int len, int *retlen = 0);

    /* convert buffer into an managed buffer, caller must copy result
       out before calling this again - substitute '?' for bad mappings */
    virtual const char *FastCvtQues(const char *, int len, int *retlen = 0);

    virtual void IgnoreBOM();

    void ResetCnt() { linecnt = 1; charcnt = 0; }
    int LineCnt() { return linecnt; }
    int CharCnt() { return charcnt; }

    static int Utf8Fold( const StrPtr *, StrBuf * );

    struct MapEnt {
	unsigned short cfrom, cto;
    };

    static char bytesFromUTF8[];
    static unsigned long offsetsFromUTF8[];
    static unsigned long minimumFromUTF8[];

protected:
    CharSetCvt() : lasterr(0), linecnt(1), charcnt(0), fastbuf(0), fastsize(0)
	    {}

    int lasterr;
    int linecnt;
    int charcnt;

    void doverify( MapEnt *, int, MapEnt *, int );
    void dodump( MapEnt *, int );
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
    virtual CharStep *FromCharStep(char *);

    static unsigned short MapThru( unsigned short, const MapEnt *,
		int, unsigned short );
private:
    char *fastbuf;
    int fastsize;

    CharSetCvt(const CharSetCvt &);	// to prevent copys
    void operator =(const CharSetCvt &);	// to prevent assignment
};

class CharSetCvtFromUTF8 : public CharSetCvt {
 protected:
    CharSetCvtFromUTF8() : checkBOM(0) {}

    virtual void IgnoreBOM();

    virtual CharStep *FromCharStep( char * );

    int checkBOM;
};

class CharSetCvtUTF8UTF8 : public CharSetCvtFromUTF8 {
    public:
	CharSetCvtUTF8UTF8(int dir, int f);
	~CharSetCvtUTF8UTF8();

// Direction 1 to client, -1 to server
// flags are...
#define UTF8_WRITE_BOM		1
#define UTF8_VALID_CHECK	2

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
    private:
	int direction;
	int flags;
	CharSetUTF8Valid *validator;
};

class CharSetCvtUTF16 : public CharSetCvtFromUTF8 {
  protected:
    CharSetCvtUTF16(int, int);

    int invert, fileinvert;
    int bom;

    virtual void IgnoreBOM();
};

class CharSetCvtUTF816 : public CharSetCvtUTF16 {

 public:
    CharSetCvtUTF816(int i = -1, int b = 0) : CharSetCvtUTF16(i, b) {}

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
};

class CharSetCvtUTF168 : public CharSetCvtUTF16 {

 public:
    CharSetCvtUTF168(int i = -1, int b = 0) : CharSetCvtUTF16(i, b) {}

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
};

class CharSetCvtUTF832 : public CharSetCvtUTF16 {

 public:
    CharSetCvtUTF832(int i = -1, int b = 0) : CharSetCvtUTF16(i, b) {}

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
};

class CharSetCvtUTF328 : public CharSetCvtUTF16 {

 public:
    CharSetCvtUTF328(int i = -1, int b = 0) : CharSetCvtUTF16(i, b) {}

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
};

class CharSetCvtUTF8to8859_1 : public CharSetCvtFromUTF8 {
 public:
    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
};

class CharSetCvt8859_1toUTF8 : public CharSetCvt {
 public:
    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
};

class CharSetCvtUTF8toShiftJis : public CharSetCvtFromUTF8 {
 public:
    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

private:
    static MapEnt UCS2toShiftJis[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport(MapEnt *, int);
    void mapreport();
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
    static int MapCount();
};

class CharSetCvtShiftJistoUTF8 : public CharSetCvt {
 public:
    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

    virtual CharStep *FromCharStep( char * );

private:
    static MapEnt ShiftJistoUCS2[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport(MapEnt *, int);
    void mapreport();
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
    static int MapCount();
};

class CharSetCvtUTF8toEUCJP : public CharSetCvtFromUTF8 {
 public:
    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

private:
    static MapEnt UCS2toEUCJP[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport(MapEnt *, int);
    void mapreport();
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
    static int MapCount();
};

class CharSetCvtEUCJPtoUTF8 : public CharSetCvt {
 public:
    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

    virtual CharStep *FromCharStep( char * );

private:
    static MapEnt EUCJPtoUCS2[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport(MapEnt *, int);
    void mapreport();
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
    static int MapCount();
};

struct SimpleCharSet {
    const CharSetCvt::MapEnt	*toMap;
    int		toMapSize;
    const unsigned short *fromMap;
    int		fromOffset;
};

class CharSetCvtUTF8toSimple : public CharSetCvtFromUTF8 {
public:
    CharSetCvtUTF8toSimple(int);
    CharSetCvtUTF8toSimple(const SimpleCharSet *s) : charinfo(s) {}

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
private:
    const SimpleCharSet *charinfo;
};

class CharSetCvtSimpletoUTF8 : public CharSetCvt {
public:
    CharSetCvtSimpletoUTF8(int);
    CharSetCvtSimpletoUTF8(const SimpleCharSet *s) : charinfo(s) {}

    virtual CharSetCvt *Clone();

    virtual CharSetCvt *ReverseCvt();

    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);
private:
    const SimpleCharSet *charinfo;
};

class CharSetCvtUTF8toCp : public CharSetCvtFromUTF8 {
 protected:
    CharSetCvtUTF8toCp( const MapEnt *tMap, int toSz )
	: toMap(tMap), toMapSize(toSz) {}

 public:
    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

private:
    const MapEnt *toMap;
    int toMapSize;
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
};

class CharSetCvtUTF8toCp949 : public CharSetCvtUTF8toCp
{
    public:
	CharSetCvtUTF8toCp949() : CharSetCvtUTF8toCp( UCS2toCp949, MapCount() ) {}

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	static int MapCount();

    private:
	static MapEnt UCS2toCp949[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport( MapEnt *, int );
    void mapreport();
};

class CharSetCvtUTF8toCp936 : public CharSetCvtUTF8toCp
{
    public:
	CharSetCvtUTF8toCp936() : CharSetCvtUTF8toCp( UCS2toCp936, MapCount() ) {}

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	static int MapCount();

    private:
	static MapEnt UCS2toCp936[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport( MapEnt *, int );
    void mapreport();
};

class CharSetCvtUTF8toCp950 : public CharSetCvtUTF8toCp
{
    public:
	CharSetCvtUTF8toCp950() : CharSetCvtUTF8toCp( UCS2toCp950, MapCount() ) {}

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	static int MapCount();

    private:
	static MapEnt UCS2toCp950[];

    friend void verifymaps();
    friend void dumpmaps();
    void mapreport( MapEnt *, int );
    void mapreport();
};

class CharSetCvtCptoUTF8 : public CharSetCvt {
 protected:
    CharSetCvtCptoUTF8( const MapEnt *tMap, int toSz )
	: toMap(tMap), toMapSize(toSz) {}

 public:
    virtual int Cvt(const char **sourcestart, const char *sourceend,
		    char **targetstart, char *targetend);

 private:
    const MapEnt *toMap;
    int toMapSize;
    virtual int isDoubleByte( int leadByte ) = 0;
    virtual void printmap( unsigned short, unsigned short, unsigned short );
    virtual void printmap( unsigned short, unsigned short );
};

class CharSetCvtCp949toUTF8 : public CharSetCvtCptoUTF8
{
    public:
	CharSetCvtCp949toUTF8() : CharSetCvtCptoUTF8( Cp949toUCS2, MapCount() ) {}

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	static int MapCount();

	CharStep *FromCharStep( char * );

    private:
	static MapEnt Cp949toUCS2[];

    friend void verifymaps();
    friend void dumpmaps();
    virtual int isDoubleByte( int leadByte );
    void mapreport(MapEnt *, int);
    void mapreport();
};

class CharSetCvtCp936toUTF8 : public CharSetCvtCptoUTF8
{
    public:
	CharSetCvtCp936toUTF8() : CharSetCvtCptoUTF8( Cp936toUCS2, MapCount() ) {}

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	static int MapCount();

	CharStep *FromCharStep( char * );

    private:
	static MapEnt Cp936toUCS2[];

    friend void verifymaps();
    friend void dumpmaps();
    virtual int isDoubleByte( int leadByte );
    void mapreport(MapEnt *, int);
    void mapreport();
};

class CharSetCvtCp950toUTF8 : public CharSetCvtCptoUTF8
{
    public:
	CharSetCvtCp950toUTF8() : CharSetCvtCptoUTF8( Cp950toUCS2, MapCount() ) {}

	virtual CharSetCvt *Clone();

	virtual CharSetCvt *ReverseCvt();

	static int MapCount();

	CharStep *FromCharStep( char * );

    private:
	static MapEnt Cp950toUCS2[];

    friend void verifymaps();
    friend void dumpmaps();
    virtual int isDoubleByte( int leadByte );
    void mapreport(MapEnt *, int);
    void mapreport();
};
