/*
 * Copyright 1995, 2003 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

class StrBuf;
class ErrorLog;

enum P4DebugType {
	DT_DB,		// DbOpen
	DT_DIFF,	// Diff
	DT_DM,		// Dm
	DT_DMC,		// Dm commands
	DT_FTP,		// Ftp Server
	DT_HANDLE,	// Handles
	DT_LBR,		// Lbr
	DT_MAP,		// MapTable
	DT_NET,		// Net
	DT_OPTIONS,	// Optional behavior
	DT_RCS,		// RCS
	DT_RECORDS,	// VarRecords
	DT_RPC,		// Rpc
	DT_SERVER,	// Server
	DT_SPEC,	// Spec
	DT_TRACK,	// Track
	DT_ZEROCONF,	// ZeroConf
	DT_OB,		// Offline Broker
	DT_VIEWGEN,     // Streamw view generator
	DT_RPL,		// Distributed functionality related
	DT_LAST
}  ;

class P4Tunable {

    public:

	void		Set( const char *set );
	void		Unset( const char *set );
    	int		Get( int t ) const { return list[t].value; }
    	int		GetLevel( const char *n ) const;
	const char	*GetName( int t ) const { return list[t].name; }
	int		IsSet( int t ) const { return list[t].isSet; }
	int		IsSet( const char * n ) const;
	int		IsKnown( const char * n );
	int		IsNumeric( const char * n );

    protected:

	static struct tunable {
	    const char *name;
	    int isSet;		
	    int value;
	    int minVal;
	    int maxVal;
	    int modVal;
	    int k;		// what's 1k? 1000 or 1024?
	    int original;
	} list[];

} ;

class P4DebugConfig {
    public:
	P4DebugConfig();
	virtual ~P4DebugConfig();
	virtual void Output();
	virtual StrBuf *Buffer();
	virtual int Alloc( int );
	void Install();
	void SetErrorLog( ErrorLog *e ) { elog = e; }

    protected:
	StrBuf *buf;
	int msz;
	ErrorLog *elog;
};

class P4Debug : private P4Tunable {

    public:

	void		SetLevel( int l );
	void		SetLevel( const char *set );
	void		SetLevel( P4DebugType t, int l ) { list[t].value = l ;}

	int		GetLevel( P4DebugType t ) const { return Get(t); }

	void		ShowLevels( int showAll, StrBuf &buf );

	void		Event();
	void		printf( const char *fmt, ... );

    private:

	void		Unbuffer();

};

extern P4Debug p4debug;
extern P4Tunable p4tunable;
