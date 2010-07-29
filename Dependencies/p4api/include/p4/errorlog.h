/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 */

/*
 * ErrorLog.h - report layered errors
 *
 * Class Defined:
 *
 *	ErrorLog - write errors to log/syslog (static)
 *
 * Public methods:
 *
 *	ErrorLog::Report() - blurt out the contents of the Error to stderr
 *	ErrorLog::Abort() - blurt out an error and exit
 *	ErrorLog::Fmt() - format an error message
 *
 *	ErrorLog::SetLog() - redirect Abort() and Report() to named file
 *	ErrorLog::SetTag() - replace standard tag used by Report()
 *
 *	ErrorLog::SetSyslog() - redirect error messages to syslog on UNIX.
 *	ErrorLog::UnsetSyslog() - Cancel syslog redirection. Revert to log file.
 */

class FileSys;

/*
 * class ErrorLog - write errors to log/syslog
 */

class ErrorLog {

    public:
			ErrorLog(){
			    useSyslog = 0;
			    errorTag = "Error";
			    errorFsys = 0;
			}
			ErrorLog( ErrorLog *from );
			~ErrorLog();

	void		Abort( const Error *e );
	void		Report( const Error *e ){ Report( e, 1 ); }
	void		ReportNoTag( const Error *e ){ Report( e, 0 ); }
	void		Report( const Error *e, int tagged );

	// Utility methods

	offL_t		Size();
	int		Exists() { return errorFsys != 0; }
	const		char *Name();

	// Global settings

	void		SetLog( const char *file );
	void		SetSyslog() { useSyslog = 1; }
	void		UnsetSyslog() { useSyslog = 0; }
	void		SetTag( const char *tag ) { errorTag = tag; }

    private:

	const 		char *errorTag;
	int		useSyslog;
	FileSys		*errorFsys;

} ;

/*
 * AssertError() - in case you need a global error to Abort() on
 */

extern Error AssertError;
extern ErrorLog AssertLog;

