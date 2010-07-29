/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * Hostenv.h - describe user's environment
 *
 * Fills result and returns for cwd/host/user.  Returns 0 if not found.
 *
 * Public methods:
 *
 *	HostEnv::GetCwd() - return the processes current working directory
 *	HostEnv::GetHost() - return the host name
 *	HostEnv::GetUser() - return the invoking user name
 *	HostEnv::GetTicketFile() - return the user ticket file location
 *	HostEnv::GetUid() - return the user id #, platform specific
 */

class Enviro;

class HostEnv {

    public:
	int		GetCwd( StrBuf &result, Enviro * = 0 );
	int		GetHost( StrBuf &result );
	int		GetUser( StrBuf &result, Enviro * = 0 );
	int		GetTicketFile( StrBuf &result, Enviro * = 0 );
	int		GetUid( int &result );
} ;
