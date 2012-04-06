/*
 * Copyright 1995, 1997 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * enviro.h - get/set environment variables/registry entries
 *
 * Note that there is no longer a global environment.  If
 * multiple threads wish to share the same enviroment, they'll
 * have to call Reload() to see any changes.  On UNIX, there
 * is no setting the environment so that isn't an issue.
 *
 * Public methods:
 *
 *	Enviro::BeServer() - get and set "system level"/service(NT) variables
 *	Enviro::Get() - get a variable from the environment
 *	Enviro::Set() - set a variable in the environment (NT only)
 *	Enviro::Config() - load $P4CONFIG file (if set)
 *	Enviro::List() - list variables in the environment 
 *	Enviro::Reload() - flush values cached from NT registry
 *	Enviro::GetConfig() - get the name of the $P4CONFIG file (if set)
 */

class EnviroTable;
struct EnviroItem;
class Error;
class StrBuf;
class StrPtr;
struct KeyPair;

class Enviro {

    public:
			Enviro();
			~Enviro();

	enum ItemType { 
		NEW,	// not looked up yet
		UNSET,	// looked up and is empty
		UPDATE,	// set via the Update call
		ENV,	// set in environment
		CONFIG,	// via P4CONFIG
		SVC,	// set in service-specific registry
		USER,	// set in user registry
		SYS 	// set is machine registry
	};

	void		BeServer( const StrPtr *name = 0 );
	const char      *ServiceName();
	void		OsServer();

	void		List();
	int		FormatVariable( int i, StrBuf *sb );
	int		HasVariable( int i );
	int		IsKnown( const char *nm );
	void		GetVarName( int i, StrBuf &sb );
	void		GetVarValue( int i, StrBuf &sb );
	void		Format( const char *var, StrBuf *sb );

	void		Print( const char *var );
	char		*Get( const char *var );
	void		Set( const char *var, const char *value, Error *e );
	void		Update( const char *var, const char *value );

	ItemType	GetType( const char *var );
	int		FromRegistry( const char *var );

	void		Config( const StrPtr &cwd );
	void		LoadConfig( const StrPtr &cwd, int checkSyntax = 1 );

	void		Reload();

	void		Save( const char *const *vars, Error *e );

	void		SetCharSet( int );	// for i18n support
	int			GetCharSet();
	
	const StrPtr	&GetConfig();

    private:

	EnviroTable	*symbolTab;
	EnviroItem	*GetItem( const char *var );

	bool		ReadItemPlatform( ItemType type, const char *var, EnviroItem * item );

	StrBuf		configFile;
	StrBuf		serviceName;

# ifdef OS_NT
	KeyPair		*setKey;
	KeyPair		*serviceKey;
	StrBuf		serviceKeyName;
	int		charset;
# elif defined ( OS_MACOSX ) || defined ( OS_DARWIN )
	ItemType        domain; // set to Enviro::USER or Enviro::SYS
#endif /* OS_NT, OS_MACOSX, OS_DARWIN */

} ;

