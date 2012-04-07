/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * FileSys.h - OS specific file manipulation
 *
 * Public classes:
 *
 *	FileSys - a file handle, with all the trimmings
 *
 * Static Public methods:
 *
 *	FileSys::Create() - create a FileSys, given its file type
 *	FileSys::CreateTemp() - create, destructor deletes the file
 *	FileSys::CreateGloablTemp() - Temp, constructor makes a global name
 *	FileSys::Perm() - translate string perm to enum
 *
 * Public methods:
 *
 *	FileSys::Set() - set file name
 *	FileSys::Name() - get file name
 *	FileSys::GetType() - get type previously set
 *	FileSys::IsTextual() - return if type is one of text types
 *	FileSys::IsExec() - return if type indicates executable bit set
 *	FileSys::DoIndirectWrites() - updates should write temp/rename
 *
 *	FileSys::MakeGlobalTemp() - make a temp name in a global directory
 *	FileSys::MakeLocalTemp() - make a temp name in same dir as file
 *
 *	FileSys::IsDeleteOnClose() - will file be removed on close?
 *	FileSys::SetDeleteOnClose() - file will be removed
 *	FileSys::ClearDeleteOnClose() - file won't be removed
 *
 *	FileSys::Perms() - set file permission for close after write
 *	FileSys::ModTime() - set mod time for close after write
 *	FileSys::ChmodTime() - use modTime value to change mod time directly
 *
 *	FileSys::Open() - open named file according to mode
 *	FileSys::Write() - write a block into file
 *	FileSys::Read() - read a block from file
 *	FileSys::ReadLine() - read a line into string
 *	FileSys::ReadWhole() - read whole file into string
 *	FileSys::Close() - close file description
 *
 *	FileSys::Stat() - return flags if file exists, writable
 *	FileSys::Truncate() - set file to zero length if it exists
 *	FileSys::Unlink() - remove single file
 *
 *	FileSys::GetFd() - return underlying int fd, FST_BINARY only
 *	FileSys::GetSize() - return file size, FST_BINARY,TEXT,ATEXT only
 *	FileSys::GetDiskSpace() - fill in data about filesystem space usage.
 *	FileSys::Seek() - seek to offset, FST_BINARY,TEXT,ATEXT only
 *	FileSys::Tell() - file position, FST_BINARY,TEXT,ATEXT only
 *
 *	FileSys::ScanDir() - return a list of directory contents
 *	FileSys::MkDir() - make a directory for the current file
 *	FileSys::RmDir() - remove the directory of the current file
 *	FileSys::Rename() - rename file to target
 *	FileSys::ReadFile() - open, read whole file into string, close
 *	FileSys::WriteFile() - open, write whole file from string, close
 *	FileSys::Chmod() - change permissions
 *	FileSys::Compare() - compare file against target
 *	FileSys::Copy - copy one file to another
 *	FileSys::Digest() - return a fingerprint of the file contents
 *	FileSys::Chmod2() - copy a file to get ownership and set perms
 *
 *	FileSys::CheckType() - look at the file and see if it is binary, etc
 */

enum FileSysType
{
	// Base types

	FST_TEXT =	0x0001,	// file is text
	FST_BINARY =	0x0002,	// file is binary
	FST_GZIP =	0x0003,	// file is gzip
	FST_DIRECTORY =	0x0005,	// it's a directory
	FST_SYMLINK =	0x0006,	// it's a symlink
	FST_RESOURCE =	0x0007,	// Macintosh resource file
	FST_SPECIAL =	0x0008,	// not a regular file
	FST_MISSING =	0x0009,	// no file at all
	FST_CANTTELL =	0x000A,	// can read file to find out
	FST_EMPTY =	0x000B,	// file is empty
	FST_UNICODE =	0x000C,	// file is unicode
	FST_GUNZIP =	0x000D,	// stream is gzip
	FST_UTF16 =	0x000E,	// stream is utf8 convert to utf16

	FST_MASK =	0x000F,	// mask for types

	// Modifiers

	FST_M_APPEND =	0x0010,	// open always append
	FST_M_EXCL =	0x0020,	// open exclusive create
	FST_M_SYNC =	0x0040,	// fsync on close

	FST_M_EXEC = 	0x0100,	// file is executable
	FST_M_APPLE =	0x0200,	// apple single/double encoding
	FST_M_COMP =	0x0400, // file is somehow compressed

	FST_M_MASK =	0x0ff0,	// mask for modifiers

	// Line ending types, loosely mapped to LineType

	FST_L_LOCAL =	0x0000,	// LineTypeLocal
	FST_L_LF =	0x1000,	// LineTypeRaw
	FST_L_CR =	0x2000,	// LineTypeCr
	FST_L_CRLF =	0x3000,	// LineTypeCrLf
	FST_L_LFCRLF =	0x4000,	// LineTypeLfcrlf

	FST_L_MASK =	0xf000,	// mask for LineTypes

	// Composite types, for filesys.cc

	FST_ATEXT =	0x0011,	// append-only text
	FST_XTEXT =	0x0101,	// executable text
	FST_RTEXT =	0x1001,	// raw text
	FST_RXTEXT =	0x1101,	// executable raw text
	FST_CBINARY =	0x0402,	// pre-compressed binary
	FST_XBINARY =	0x0102,	// executable binary
	FST_APPLETEXT =	0x0201,	// apple format text
	FST_APPLEFILE =	0x0202,	// apple format binary
	FST_XAPPLEFILE =	0x0302,	// executable apple format binary
	FST_XUNICODE =	0x010C,	// executable unicode text
	FST_XUTF16 =	0x010E,	// stream is utf8 convert to utf16
	FST_RCS =	0x1041 	// RCS temporary file: raw text, sync on close

};

enum FileStatFlags {
	FSF_EXISTS 	= 0x01,	// file exists
	FSF_WRITEABLE	= 0x02,	// file is user-writable
	FSF_DIRECTORY	= 0x04,	// file is a directory
	FSF_SYMLINK	= 0x08,	// file is symlink
	FSF_SPECIAL	= 0x10,	// file is not regular
	FSF_EXECUTABLE	= 0x20,	// file is executable
	FSF_EMPTY	= 0x40,	// file is empty
	FSF_HIDDEN	= 0x80	// file is invisible (hidden)
} ;

enum FileOpenMode {
	FOM_READ,		// open for reading
	FOM_WRITE		// open for writing
} ;

enum FilePerm {
	FPM_RO,		// leave file read-only
	FPM_RW,		// leave file read-write
	FPM_ROO		// leave file read-only (owner)
} ;

class StrArray;
class CharSetCvt;
class MD5;
class StrBuf;

class DiskSpaceInfo {

    public:

	    		DiskSpaceInfo();
			~DiskSpaceInfo();

	P4INT64		blockSize;
	P4INT64		totalBytes;
	P4INT64		usedBytes;
	P4INT64		freeBytes;
	int		pctUsed;
	StrBuf		*fsType;
} ;

class FileSys {

    public:
	// Creators

	static FileSys *Create( FileSysType type );

	static FileSys *CreateTemp( FileSysType type ) {
				FileSys *f = Create( type );
				f->SetDeleteOnClose();
				return f;
			}

	static FileSys *CreateGlobalTemp( FileSysType type ) {
				FileSys *f = Create( type );
				f->SetDeleteOnClose();
				f->MakeGlobalTemp();
				return f;
			}

	static FilePerm Perm( const char *p );

	static int	BufferSize();

	int		IsUnderPath( const StrPtr &path );

	static int	SymlinksSupported()
# ifdef OS_NT
		;		// Have to probe the system to decide
# else
# ifdef HAVE_SYMLINKS
				{ return 1; }
# else
				{ return 0; }
# endif
# endif

	// Get/set perms, modtime

	void		Perms( FilePerm p ) { perms = p; }
	void		ModTime( StrPtr *u ) { modTime = u->Atoi(); }
	void		ModTime( time_t t ) { modTime = (int)t; }

	// Set filesize hint for NT fragmentation avoidance

	void		SetSizeHint( offL_t l ) { sizeHint = l; }
	offL_t		GetSizeHint() { return sizeHint; }

	// Initialize digest

	void		SetDigest( MD5 *m ) { checksum = m; }

	// Get type info

	FileSysType 	GetType() { return type; }
	int		IsExec() { return ( type & FST_M_EXEC ); }
	int		IsTextual() { 
				return ( type & FST_MASK ) == FST_TEXT || 
				       ( type & FST_MASK ) == FST_UNICODE ||
				       ( type & FST_MASK ) == FST_UTF16;
			}
	int		IsUnicode() { 
				return ( type & FST_MASK ) == FST_UNICODE ||
				       ( type & FST_MASK ) == FST_UTF16;
			}
	int             IsSymlink() {
	                        return ( type & FST_MASK ) == FST_SYMLINK;
	}


	// Read/write file access, provided by derived class

			FileSys();
	virtual		~FileSys();

	virtual void	Set( const StrPtr &name );
	virtual StrPtr	*Path() { return &path; }
	virtual int	DoIndirectWrites();
	virtual void	Translator( CharSetCvt * );

	virtual void	Open( FileOpenMode mode, Error *e ) = 0;
	virtual void	Write( const char *buf, int len, Error *e ) = 0;
	virtual int	Read( char *buf, int len, Error *e ) = 0;
	virtual void	Close( Error *e ) = 0;

	virtual int	Stat() = 0;
	virtual int	StatModTime() = 0;
	virtual void	Truncate( Error *e ) = 0;
	virtual void	Unlink( Error *e = 0 ) = 0;
	virtual void	Rename( FileSys *target, Error *e ) = 0;
	virtual void	Chmod( FilePerm perms, Error *e ) = 0;
	virtual void	ChmodTime( Error *e ) = 0;

	// NB: these for ReadFile only; interface will likely change

	virtual int	GetFd();
	virtual offL_t	GetSize();
	virtual void	Seek( offL_t offset, Error * );
	virtual offL_t	Tell();

	// Convenience wrappers for above

	void		Chmod( Error *e ) { Chmod( perms, e ); }
	void		Chmod( const char *perms, Error *e )
			{ Chmod( Perm( perms ), e ); } 

	char *		Name() { return Path()->Text(); }
	void		Set( const char *name ) { Set( StrRef( name ) ); }

	void		Write( const StrPtr &b, Error *e ) 
			{ Write( b.Text(), b.Length(), e ); }

	void		Write( const StrPtr *b, Error *e ) 
			{ Write( b->Text(), b->Length(), e ); }

	// Tempfile support

	void		MakeGlobalTemp();
	virtual void	MakeLocalTemp( char *file );
	int		IsDeleteOnClose() { return isTemp; }
	void		SetDeleteOnClose() { isTemp = 1; }
	void		ClearDeleteOnClose() { isTemp = 0; }

	// Meta operations

	virtual StrArray *ScanDir( Error *e );

	virtual void	MkDir( const StrPtr &p, Error *e );
	void		MkDir( Error *e ) { MkDir( path, e ); }

	virtual void	RmDir( const StrPtr &p, Error *e );
	void		RmDir( Error *e = 0 ) { RmDir( path, e ); }

	FileSysType	CheckType();

# if defined ( OS_MACOSX )
	FileSysType	CheckTypeMac();
# endif

	// Type generic operations

	virtual int	ReadLine( StrBuf *buf, Error *e );
	void		ReadWhole( StrBuf *buf, Error *e );

	// Type generic, whole file operations

	void		ReadFile( StrBuf *buf, Error *e );
	void		WriteFile( const StrPtr *buf, Error *e );
	int		Compare( FileSys *other, Error *e );
	void 		Copy( FileSys *targetFile, FilePerm perms, Error *e );
	virtual void	Digest( StrBuf *digest, Error *e );
	void		Chmod2( FilePerm perms, Error *e );
	void		Chmod2( const char *p, Error *e )
			{ Chmod2( Perm( p ), e ); }

	void		Cleanup();

	// Character Set operations

	void		SetCharSetPriv( int x = 0 ) { charSet = x; }
	int		GetCharSetPriv() { return charSet; }
	void		SetContentCharSetPriv( int x = 0 ) { content_charSet = x; }
	int		GetContentCharSetPriv() { return content_charSet; }

	void		GetDiskSpace( DiskSpaceInfo *info, Error *e );

	void		LowerCasePath();
    protected:

	FileOpenMode	mode;		// read or write
	FilePerm	perms;		// leave read-only or read-write
	int		modTime;	// stamp file mod date on close
	offL_t		sizeHint;       // how big will the file get ?
	StrBuf		path;
	FileSysType 	type;
	MD5		*checksum;      // if verifying file transfer

    private:
	void		TempName( char *buf );

	int		isTemp;

	int		charSet;
	int		content_charSet;

} ;
