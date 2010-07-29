/*
 * Copyright 1995, 2003 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * RunCommand() -- Just run a command and capture its output
 *
 * Classes:
 *
 *	RunCommand - run a command
 *	RunCommandIo - run a command controlling stdin/stoud
 *
 *	RunArgs - a StrBuf for quoting command arugments to protect them
 *		   from the shell.  See below for comments on Quoting.
 *
 * Except where notes, these are implemented for UNIX, NT, VMS, MAC,
 * MACOSX, and OS2.
 *
 * Public methods:
 *
 *	RunArgs::AddArg() - add a single argument, quoting as needed
 *
 *	RunArgs::AddCmd() - add the first argument, which may be both
 *			a command and flags.  On Windows, we try to
 *			distinguish a command with spaces in it from
 *			a command followed by flags.  We do so by looking
 *			for the - or / introducing flags.
 *
 *	RunArgs::SetArgs() - clear the command buffer and add args
 *
 *	RunCommand::Run() - run the command
 *			Used by the client for launching editor, diff.
 *
 *	RunCommand::RunInWindow() - create a window to run the command
 *			Used by p4web for launching editor, resolve.
 *			Not implemented for VMS.
 *
 *	RunCommand::RunChild() - launch a subprocess whose stdin/stdout
 *			are the given fds.  Not implemented for VMS.
 *
 *	RunCommand::WaitChild() - wait for the child launched by RunChild().
 *
 *	RunCommandIo::Run() - run the command, sending stdin, capturing 
 *			stdout. Used to run triggers for 'p4 submit'.
 *			Uses RunChild() which isn't implmented for MAC, VMS.
 *
 *	RunCommandIo::Run() - run the command, with stdin/stdout writeable
 *	RunCommandIo::Write() - write the the running command's stdin
 *	RunCommandIo::Read() - read from the running command's stdout
 * 	RunCommandIo::ReadError() - read a little and see if command failed
 *
 * Quoting:
 *
 *	If the command is built by SetArgs/AddArg/AddCmd and run by
 *	Run(), AddArg() will quote and Run() will pass the built up
 *	command to the shell.  This is how the client runs P4DIFF,
 *	P4EDITOR, etc:
 *
 *		NT: quote with " all args, and use CreateProcess()
 *		UNIX: quote with ' args with spaces, and use system()
 *
 *	If the command is pre-built and run with RunChild() or
 *	RunCommandIo::Run(), we get split behavior.  This is 
 *	how the client handled P4PORT=rsh: and how the server
 *	handles triggers:
 *
 *		NT: use CreateProcess(), which handles quoted args
 *		UNIX: split with StrOps::Words(), and use execvp().
 *			Words() handles "; execvp() doesn't.
 */

enum RunCommandOpts {

	RCO_SOLO_FD = 0x01,	// RunChild() uses same fd for I/O
	RCO_AS_SHELL = 0x02,	// RunChild() uses separate pipes, no socketPair
	RCO_USE_STDOUT = 0x04	// RunChild() preserves stdout for command

} ;

class RunArgs {

    public:

		RunArgs() {}
		RunArgs( const StrPtr &cmd ) { buf = cmd; }

	void	AddArg( const StrPtr &arg );
	void	AddArg( const char *arg );
	void	SetArgs( int argc, const char * const *argv );
	void 	AddCmd( const char *arg );

	StrBuf &SetBuf() { buf.Clear(); return buf; }

	RunArgs &operator <<( const char *a ) { AddArg( a ); return *this; }
	RunArgs &operator <<( const StrPtr &a ) { AddArg( a ); return *this; }

	char *	Text() { return buf.Text(); }

    friend class RunCommand;

    private:

	int	Argc( char **argv, int nargv );

	StrBuf	buf;
	StrBuf	argbuf;
} ;

class RunCommand {

    public:
		RunCommand();
		~RunCommand();

	int 	Run( RunArgs &cmd, Error *e );
	int 	RunInWindow( RunArgs &cmd, Error *e );
	void 	RunChild( RunArgs &cmd, int opts, int f[2], Error *e );

	int	WaitChild();

    private:

# ifdef HAVE_FORK
	pid_t	pid;
# endif

# ifdef OS_NT
	void	*pid;
# endif

} ;

class RunCommandIo : public RunCommand {

    public:
		RunCommandIo();
		~RunCommandIo();

	int 	Run( RunArgs &cmd, const StrPtr &in, StrBuf &out, Error *e );

	int 	Run( RunArgs &cmd, StrBuf &result, Error *e )
		{ return Run( cmd, StrRef::Null(), result, e ); }

	void 	Run( RunArgs &cmd, Error *e )
		{ RunChild( cmd, RCO_AS_SHELL, fds, e ); }

	void	Write( const StrPtr &in, Error *e );
	int 	Read( const StrPtr &out, Error *e );
	StrPtr *ReadError( Error *e );

    private:
	int	Read( char *buf, int length, Error *e );

	int	fds[2];
	StrBuf	errBuf;

} ;

