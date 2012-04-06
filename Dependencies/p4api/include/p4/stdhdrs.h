/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * Standard headers
 *
 * Note: where both a NEED_XXX and HAVE_XXX are recognized, the form to
 * use is:
 *
 *   # ifdef OS_YYY
 *   #   define HAVE_XXX
 *   #   ifdef NEED_XXX
 *   #     include <xxx.h>
 *   #   endif
 *   # endif
 *
 *  This causes the HAVE_XXX macro to be defined even if the NEED_XXX macro 
 *  is not; this protects us from problems caused by #ifdef HAVE_XXX in
 *  header files.
 */

# ifdef OS_VMS
# define _POSIX_EXIT  // to get exit status right from stdlib.h
# endif	

# include <stdio.h>
# include <string.h>
# include <stdlib.h>

# if !defined( OS_QNX ) && !defined( OS_VMS )
# include <memory.h>
# endif
# ifdef OS_NEXT
# include <libc.h>
# endif

/*
 * NEED_ACCESS - access()
 * NEED_BRK - brk()/sbrk()
 * NEED_CHDIR - chdir()
 * NEED_DBGBREAK - DebugBreak(), just Windows for now
 * NEED_EBCDIC - __etoa, __atoe
 * NEED_ERRNO - errno, strerror
 * NEED_FILE - write(), unlink(), etc
 * NEED_FCNTL - O_XXX flags
 * NEED_FLOCK - LOCK_UN, etc
 * NEED_FORK - fork(), waitpid()
 * NEED_FSYNC - fsync()
 * NEED_GETPID - getpid()
 * NEED_GETUID - getuid(),setuid() etc.
 * NEED_IOCTL - ioctl() call and flags for UNIX
 * NEED_MKDIR - mkdir()
 * NEED_MMAP - mmap()
 * NEED_OPENDIR - opendir(), etc
 * NEED_POPEN - popen(), pclose()
 * NEED_SIGNAL - signal()
 * NEED_SLEEP - Sleep()
 * NEED_SMARTHEAP - Smartheap Initialization
 * NEED_STAT - stat()
 * NEED_STATFS - statfs()
 * NEED_STATVFS - statvfs()
 * NEED_SOCKETPAIR - pipe(), socketpair()
 * NEED_SYSLOG - syslog()
 * NEED_TIME - time(), etc
 * NEED_TYPES - off_t, etc (always set)
 * NEED_UTIME - utime()
 */

# if defined( NEED_ACCESS ) || \
	defined( NEED_CHDIR ) || \
	defined( NEED_EBCDIC ) || \
	defined( NEED_FILE ) || \
	defined( NEED_FSYNC ) || \
	defined( NEED_FORK ) || \
	defined( NEED_GETCWD ) || \
	defined( NEED_GETPID ) || \
	defined( NEED_GETPWUID ) || \
	defined( NEED_GETUID ) || \
	defined( NEED_BRK ) || \
	defined( NEED_SLEEP )

# ifndef OS_NT
# include <unistd.h>
# endif

# ifdef OS_VAXVMS
# include <unixio.h>
# endif

# endif

# if defined( NEED_BRK )
# if !defined( OS_NT ) && !defined( MAC_MWPEF ) && \
     !defined( OS_AS400 ) && !defined( OS_MVS ) && \
     !defined( OS_LINUX ) && !defined( OS_DARWIN )
# define HAVE_BRK
# endif
# endif

# if defined( NEED_GETUID )
# ifdef unix 
# define HAVE_GETUID
# endif
# endif

# if defined( NEED_EBCDIC ) 
# if defined( OS_AS400 )
# include <ebcdic.h>
# endif
# endif

# if defined( NEED_ACCESS ) || defined( NEED_CHDIR )
# if defined( OS_NT ) || defined( OS_OS2 )
# include <direct.h>
# endif
# endif

# if defined( NEED_ERRNO )
# ifdef OS_AS400
extern int errno;
# endif
# include <errno.h>
# endif

# if defined(NEED_FILE) || defined(NEED_FSYNC)
# ifdef OS_NT
# include <io.h>
# endif
# endif

# ifdef NEED_FCNTL
# include <fcntl.h>
# endif

// This must be one of the first occurances for including windows.h
// so that _WIN32_WINNT will flavor definitions.
//
# ifdef OS_NT
# define HAVE_DBGBREAK
# ifdef NEED_DBGBREAK
# define WIN32_LEAN_AND_MEAN
# define _WIN32_WINNT 0x0501
# include <windows.h>
# endif // NEED_DBGBREAK
# endif // OS_NT

// This must be one of the first occurances for including windows.h
// so that _WIN32_WINNT will flavor definitions.
//
# ifdef NEED_SMARTHEAP
# if defined( USE_SMARTHEAP )
# ifdef OS_NT
# define WIN32_LEAN_AND_MEAN
# define _WIN32_WINNT 0x0501
# include <windows.h>
# endif // OS_NT
# include <smrtheap.h>
# define HAVE_SMARTHEAP
# endif // USE_SMARTHEAP
# endif // NEED_SMARTHEAP

# ifdef NEED_FLOCK
# ifdef OS_NT
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
# endif
# ifdef unix
# include <sys/file.h>
# ifdef LOCK_UN
// 2.96 linux ia64 declares it with a __THROW
# ifndef __THROW
extern "C" int flock( int, int );
# endif
# endif
# endif
# endif

# if !defined( OS_OS2 ) && !defined( MAC_MWPEF ) && \
     !defined( OS_NT )  && !defined( OS_AS400 ) && \
     !defined( OS_VMS )
# define HAVE_FORK
# ifdef NEED_FORK
# ifdef OS_MACHTEN
# include "/usr/include/sys/wait.h"
# endif 
# include <sys/wait.h>
# endif
# endif

# if !defined( OS_BEOS ) && !defined( OS_NT ) && \
     !defined( OS_OS2 )
# define HAVE_FSYNC
# endif

# ifdef NEED_GETCWD
# ifdef OS_NEXT
# define getcwd( b, s ) getwd( b )
# endif
# if defined(OS_OS2) || defined(OS_NT)
extern "C" char *getcwd( char *buf, size_t size );
# endif
# ifdef OS_VMS
# include <unixlib.h>
# endif
# endif 

# if !defined(OS_OS2)
# define HAVE_GETHOSTNAME

# ifdef NEED_GETHOSTNAME

# ifdef OS_BEOS
# include <net/netdb.h>
# endif

# ifdef OS_VMS
# include <socket.h>
# endif

# if defined(OS_PTX) || \
	defined(OS_QNX) || \
	defined(OS_AIX32) || \
	defined(OS_NCR) || \
	defined(OS_UNIXWARE2)

extern "C" int gethostname( char * name, int namelen );

# endif

# if defined(OS_NT) 
extern "C" int __stdcall gethostname( char * name, int namelen );
# endif

# endif /* NEED_GETHOSTNAME */

# endif

# ifdef NEED_GETPID
# ifdef OS_NT
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# else
# if defined(OS_OS2)
# include <process.h>
# endif /* OS2 */
# endif	/* NT */
# endif	/* GETPID */

# if !defined( OS_VMS ) && !defined( OS_NT ) && !defined( OS_BEOS ) && \
	!defined( MAC_MWPEF ) && !defined( OS_OS2 )
# define HAVE_GETPWUID
# ifdef NEED_GETPWUID
# include <pwd.h>
# endif 
# endif /* UNIX */

# ifdef NEED_IOCTL
# ifndef OS_NT
# include <sys/ioctl.h>
# endif /* NT */
# endif /* IOCTL */

# if defined(NEED_MKDIR) || defined(NEED_STAT) || defined(NEED_CHMOD)

# ifdef OS_OS2
# include <direct.h>
# endif

# include <sys/stat.h>

# ifndef S_ISLNK /* NCR */
# define S_ISLNK(m) (((m)&S_IFMT)==S_IFLNK)
# endif

# ifndef S_ISDIR /* NEXT */
# define S_ISDIR(m) (((m)&S_IFMT)==S_IFDIR)
# define S_ISREG(m) (((m)&S_IFMT)==S_IFREG)
# endif

# ifdef OS_NT
# define PERM_0222 (S_IWRITE)
# define PERM_0266 (S_IWRITE)
# define PERM_0666 (S_IRUSR|S_IWRITE)
# define PERM_0777 (S_IRUSR|S_IWRITE|S_IEXEC)
# ifndef S_IRUSR
# define S_IRUSR S_IREAD
# define S_IWUSR S_IWRITE
# define S_IXUSR S_IEXEC
# endif	/* S_IRUSR */
# endif

# ifndef PERM_0222
# define PERM_0222 (S_IWUSR | S_IWGRP | S_IWOTH)
# define PERM_0266 (S_IWUSR | (S_IRGRP|S_IWGRP) | (S_IROTH|S_IWOTH))
# define PERM_0666 ((S_IRUSR|S_IWUSR) | (S_IRGRP|S_IWGRP) | (S_IROTH|S_IWOTH))
# define PERM_0777 (S_IRWXU | S_IRWXG | S_IRWXO)
# endif

# endif

# if defined(NEED_STATVFS)

# ifdef OS_NT
# else
# include <sys/statvfs.h>
# endif

# ifdef OS_SOLARIS
# define HAVE_STATVFS_BASETYPE
# endif

# endif

# if defined(NEED_STATFS)

# ifdef OS_LINUX
# define HAVE_STATFS
# include <sys/statfs.h>
# endif

# if defined(OS_DARWIN80) || defined(OS_DARWIN90) || defined(OS_DARWIN100) \
  || defined(OS_FREEBSD)
# define HAVE_STATFS
# define HAVE_STATFS_FSTYPENAME
# include <sys/param.h>
# include <sys/mount.h>
# endif

# endif /* NEED_STATFS */

/* Many users don't define NEED_MMAP -- so we always find out */
/* added AIX 5.3 - mmap region getting corrupted */

# if !defined( OS_AS400 ) && !defined( OS_BEOS ) && \
	!defined( OS_HPUX68K ) && \
	!defined( OS_MACHTEN ) && !defined( OS_MVS ) && \
	!defined( OS_VMS62 ) && !defined( OS_OS2 ) && \
	!defined( OS_NEXT ) && !defined( OS_NT ) && \
	!defined( OS_QNX ) && !defined( OS_UNICOS ) && \
	!defined( OS_MPEIX ) && !defined( OS_QNXNTO ) && \
	!defined( OS_MACOSX ) && !defined( OS_ZETA ) && \
	!defined( OS_AIX53 ) && !defined( OS_LINUXIA64 )

# define HAVE_MMAP
# ifdef NEED_MMAP
# ifdef OS_HPUX9
extern "C" caddr_t mmap(const caddr_t, size_t, int, int, int, off_t);
extern "C" int munmap(const caddr_t, size_t);
# endif /* HPUX9 */
# include <sys/mman.h>
# endif /* NEED_MMAP */
# endif /* HAVE_MMAP */

# ifdef NEED_OPENDIR
# include <dirent.h>
# endif

# ifdef NEED_POPEN
# ifdef OS_NT
# define popen _popen
# define pclose _pclose
# endif
# ifdef OS_MVS
extern "C" int pclose(FILE *);
extern "C" FILE *popen(const char *, const char *);
# endif
# endif

# ifdef NEED_SIGNAL
# ifdef OS_OSF
# include "/usr/include/sys/signal.h"
# else
# include <signal.h>
# endif
# if defined( OS_NEXT ) || defined( OS_MPEIX )
// broken under gcc 2.5.8
# undef SIG_IGN
# undef SIG_DFL
# define SIG_DFL         (void (*)(int))0
# define SIG_IGN         (void (*)(int))1
# endif
# endif

/*
 * This definition differs from the conventional approach because we test
 * on AF_UNIX and that's not defined until after we include socket.h. So,
 * we use the old scheme of only defining HAVE_SOCKETPAIR if NEED_SOCKETPAIR
 * is set and the call exists.
 */
# ifdef NEED_SOCKETPAIR
# if defined( OS_NT )
# define WINDOWS_LEAN_AND_MEAN
# include <windows.h>
# include <process.h>
# elif defined( OS_BEOS )
# include <net/socket.h>
# else
# include <sys/socket.h>
# endif
# if defined( AF_UNIX ) && \
    !defined( OS_AS400 ) && \
    !defined( OS_NT ) && \
    !defined( OS_QNXNTO ) && \
    !defined( OS_OS2 ) && \
    !defined( OS_VMS )
# define HAVE_SOCKETPAIR
# if defined(OS_MACHTEN) || defined(OS_AIX32) || defined(OS_MVS)
extern "C" int socketpair(int, int, int, int*);
# endif
# endif
# endif

# ifdef NEED_SYSLOG
#  if defined( unix )
#   define HAVE_SYSLOG
#   include <syslog.h>
#  elif defined( OS_NT )
#   define HAVE_EVENT_LOG
#   include <windows.h>
#  endif
# endif

# if defined(NEED_TIME) || defined(NEED_UTIME)
# include <time.h>
# endif

# if defined(NEED_TYPES) || 1

# if defined ( MAC_MWPEF )
# include <stat.h>
// because time_t is __std(time_t)
using namespace std;
# else
# include <sys/types.h>
# endif

# endif

# ifndef OS_VMS
# define HAVE_UTIME
# ifdef NEED_UTIME
# if ( defined( OS_NT ) || defined( OS_OS2 ) ) && !defined(__BORLANDC__)
# include <sys/utime.h>
# else
# include <utime.h>
# endif
# endif
# endif

# ifdef NEED_SLEEP
# ifdef OS_NT
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
# define usleep(x) Sleep(x)
# ifndef OS_MINGW
typedef unsigned long useconds_t;
# endif
# endif
# endif

/*
 * HAVE_TRUNCATE -- working truncate() call
 * HAVE_SYMLINKS -- OS supports SYMLINKS
 */

# define HAVE_SYMLINKS
# if defined( OS_OS2 ) || \
	defined ( MAC_MWPEF ) || \
	defined( OS_VMS ) || \
	defined( OS_INTERIX )
# undef HAVE_SYMLINKS
# endif

# define HAVE_TRUNCATE
# if defined( OS_OS2 ) || \
	defined( OS_NT ) || \
	defined( MAC_MWPEF ) || \
	defined( OS_BEOS ) || \
	defined( OS_QNX ) || \
	defined( OS_SCO ) || \
	defined( OS_VMS ) || \
	defined( OS_INTERIX ) || \
	defined( OS_MVS ) || \
	defined( OS_MPEIX ) || \
	defined( OS_AS400 )
# undef HAVE_TRUNCATE
# endif

/* These systems have no memccpy() or a broken one */

# if defined( OS_AS400 ) || defined( OS_BEOS ) || defined( OS_FREEBSD ) || \
	defined( OS_MACHTEN ) || defined( OS_MVS ) || \
	defined( OS_VMS62 ) || defined( OS_RHAPSODY ) || defined( OS_ZETA )
	
# define BAD_MEMCCPY
extern "C" void *memccpy(void *, const void *, int, size_t);
# endif

/* SUNOS has old headers, bless its heart */

# ifdef OS_SUNOS
# define memmove(d, s, c) bcopy(s, d, c)

extern "C" {
void bcopy(const void *src, void *dst, size_t len);
int closelog();
int fsync(int fd);
int ftruncate(int fd, off_t length);
int gethostname(char *name, int namelen);
int getrusage(int who, struct rusage *rusage);
int gettimeofday(struct timeval *tp, struct timezone *tzp);
int lstat(const char *path, struct stat *sb);
int munmap(void *addr, size_t len);
int openlog(const char *ident, int logopt, int facility);
int readlink(const char *path, char *buf, int bufsiz);
caddr_t sbrk(int inc);
int socketpair(int d, int type, int protocol, int *sv);
int symlink(const char *name1, const char *name2);
int syslog(int priority, const char *message ... );
int tolower(int c);
int toupper(int c);
int truncate(const char *path, off_t length);
} ;

# endif

/*
 * MT_STATIC - static multithreaded data
 */

# ifdef OS_NT
# define MT_STATIC static __declspec(thread)
# else
# define MT_STATIC static
# endif

/*
 * Illegal characters in a filename, includes %
 * as escape character.  Used when creating an
 * archive file in the spec depot
 */

# ifdef OS_NT
# define BadSpecFileCharList "%/<>:|"
# else
# define BadSpecFileCharList "%/"
# endif

/*
 * LineType - LF (raw), CR, CRLF, lfcrlf (LF/CRLF)
 */

enum LineType { LineTypeRaw, LineTypeCr, LineTypeCrLf, LineTypeLfcrlf };

# ifdef USE_CR
# define LineTypeLocal LineTypeCr
# endif
# ifdef USE_CRLF
# define LineTypeLocal LineTypeCrLf
# endif
# ifndef LineTypeLocal
# define LineTypeLocal LineTypeRaw
# endif

/*
 * P4INT64 - a 64 bit int
 */

# if !defined( OS_MVS ) && \
     !defined( OS_OS2 ) && \
     !defined( OS_QNX )
# define HAVE_INT64
# ifdef OS_NT
# define P4INT64 __int64
# else
# define P4INT64 long long
# endif
# endif

# ifndef P4INT64
# define P4INT64 int
# endif

/*
 * offL_t - size of files or offsets into files
 */

typedef P4INT64 offL_t;

# ifdef OS_MACOSX
# define FOUR_CHAR_CONSTANT(_a, _b, _c, _d)       \
        ((UInt32)                                 \
        ((UInt32) (_a) << 24) |                   \
        ((UInt32) (_b) << 16) |                   \
        ((UInt32) (_c) <<  8) |                   \
        ((UInt32) (_d)))
# endif

/* 
 * B&R's NTIA64 build machine doesn't define vsnprintf, 
 * but it does define _vsnprintf. Use that one instead.
 */
# ifdef OS_NTIA64
# define vsnprintf _vsnprintf 
# endif

