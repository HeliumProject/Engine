/*
 * Copyright 2010 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

#ifndef __CLIENTRESOLVEA__
#define __CLIENTRESOLVEA__

#include "clientmerge.h"

/*
 * ClientResolveA - client side "action resolve" controller
 *
 *	An "action resolve" is a multiple-choice resolve that (generally)
 *	modifies the way that the client file is open, not its contents.
 *
 */

class ClientUser;

class ClientResolveA
{
    public:
	// User interface: Resolve() is called to present a command-line
        // dialog via ClientUser, presenting the Merge/Yours/Their actions
        // and asking the user to pick one.
	
        // AutoResolve() returns the server-suggested action.  This is
        // displayed by Resolve() as well.

	MergeStatus AutoResolve( MergeForce force ) const;
	MergeStatus Resolve( int preview, Error *e );

	const Error &GetType() const { return type; }

	const Error &GetMergeAction() const { return mergeA; }
	const Error &GetYoursAction() const { return yoursA; }
	const Error &GetTheirAction() const { return theirA; }

	// For the CLI interface, probably not of interest to others

	const Error &GetMergePrompt() const { return mergeP; }
	const Error &GetYoursPrompt() const { return yoursP; }
	const Error &GetTheirPrompt() const { return theirP; }

	const Error &GetMergeOpt() const { return mergeO; }
	const Error &GetYoursOpt() const { return yoursO; }
	const Error &GetTheirOpt() const { return theirO; }
	const Error &GetSkipOpt()  const { return skipO;  }
	const Error &GetHelpOpt()  const { return helpO;  }
	const Error &GetAutoOpt()  const { return autoO;  }

	const Error &GetPrompt() const { return prompt; }
	const Error &GetTypePrompt() const { return typeP; }
	const Error &GetUsageError() const { return error; }
	const Error &GetHelp() const { return help; }

	// Called by clientservice while talking to the server

	ClientResolveA( ClientUser *ui );

	void SetMergeAction( const Error &msg ) { mergeA = msg; }
	void SetYoursAction( const Error &msg ) { yoursA = msg; }
	void SetTheirAction( const Error &msg ) { theirA = msg; }

	void SetMergePrompt( const Error &msg ) { mergeP = msg; }
	void SetYoursPrompt( const Error &msg ) { yoursP = msg; }
	void SetTheirPrompt( const Error &msg ) { theirP = msg; }

	void SetMergeOpt( const Error &msg ) { mergeO = msg; }
	void SetYoursOpt( const Error &msg ) { yoursO = msg; }
	void SetTheirOpt( const Error &msg ) { theirO = msg; }
	void SetHelpOpt ( const Error &msg ) { helpO  = msg; }
	void SetSkipOpt ( const Error &msg ) { skipO  = msg; }
	void SetAutoOpt ( const Error &msg ) { autoO  = msg; }

	void SetTypePrompt( const Error &msg ) { typeP = msg; }
	void SetType( const Error &msg ) { type = msg; }

	void SetPrompt( const Error &msg ) { prompt = msg; }
	void SetHelp( const Error &msg ) { help = msg; }
	void SetUsageError( const Error &msg ) { error = msg; }

	void SetAuto( MergeStatus s ) { suggest = s; }

    private:

	ClientUser *ui;

	Error mergeA;
	Error yoursA;
	Error theirA;

	Error mergeP;
	Error yoursP;
	Error theirP;

	Error mergeO; // "am"
	Error yoursO; // "ay"
	Error theirO; // "at"
	Error helpO;  // "?"
	Error skipO;  // "s"
	Error autoO;  // "a"

	Error type;
	Error typeP;
	Error help;
	Error prompt;
	Error error;

	MergeStatus suggest;
};


# endif /* __CLIENTRESOLVEA__ */
