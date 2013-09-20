#pragma once

#include "Foundation/SmartPtr.h"
#include "Foundation/String.h"

#include "Application/API.h"

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Helium
{
	namespace CommandLine
	{
		class HELIUM_APPLICATION_API Option : public Helium::RefCountBase< Option >
		{
		protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_Help;

		public:
			Option( const char* token, const char* usage = TXT( "<ARG>" ), const char* help = TXT( "" ) );
			
			virtual const std::string& Token() const;
			virtual const std::string& Usage() const;
			virtual const std::string& Help() const;
	
			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) = 0;
		};
		typedef Helium::SmartPtr< Option > OptionPtr;
		typedef std::vector< OptionPtr > V_OptionPtr;
		typedef std::map< std::string, OptionPtr > M_StringToOptionPtr;

		template <class T>
		class SimpleOption : public Option
		{  
		protected:
			T* m_Data;

		public:
			SimpleOption( T* data, const char* token, const char* usage = TXT( "<ARG>" ), const char* help = TXT( "" ) );

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) HELIUM_OVERRIDE;
		};

		template <>
		HELIUM_APPLICATION_API bool SimpleOption<std::string>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

		template <>
		HELIUM_APPLICATION_API bool SimpleOption<bool>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

		template <>
		HELIUM_APPLICATION_API bool SimpleOption< std::vector< std::string > >::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

		class HELIUM_APPLICATION_API FlagOption : public SimpleOption<bool>
		{
		protected:
			bool* m_Data;

		public:
			FlagOption( bool* data, const char* token, const char* help = TXT( "" ) );

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) HELIUM_OVERRIDE;
		};

		class HELIUM_APPLICATION_API OptionsMap
		{
		public:
			mutable std::string m_Usage;
			mutable std::string m_Help;

			M_StringToOptionPtr m_OptionsMap;
			V_OptionPtr m_Options;

		public:
			OptionsMap();

			const std::string& Usage() const;
			const std::string& Help() const;

			bool AddOption( const OptionPtr& option, std::string& error );
			bool ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );
		};

		class HELIUM_APPLICATION_API Command
		{
		protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_ShortHelp;
			mutable std::string m_Help;

			OptionsMap m_OptionsMap;

		public:
			Command( const char* token, const char* usage = TXT( "[OPTIONS]" ), const char* shortHelp = TXT( "" ) );

			virtual bool Initialize( std::string& error );
			virtual void Cleanup();

			virtual const std::string& Token() const;
			virtual const std::string& ShortHelp() const;
			virtual const std::string& Help() const;

			bool AddOption( const OptionPtr& option, std::string& error );
			bool ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) = 0;
		};

		typedef std::map< std::string, Command* > M_StringToCommandDumbPtr;

		class Processor;

		class HELIUM_APPLICATION_API HelpCommand : public Command
		{
		protected:
			Processor* m_Owner;
			std::string m_CommandName;

		public:
			HelpCommand( Processor* owner = NULL );

			inline void SetOwner( Processor* owner );

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) HELIUM_OVERRIDE;
		};
		
		class HELIUM_APPLICATION_API Processor
		{
		protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_ShortHelp;
			mutable std::string m_Help;

			OptionsMap m_OptionsMap;
			M_StringToCommandDumbPtr m_Commands;

		public:
			Processor( const char* token, const char* usage = TXT( "COMMAND [ARGS]" ), const char* shortHelp = TXT( "" ) );
			virtual ~Processor();

			virtual bool Initialize( std::string& error );
			virtual void Cleanup();

			virtual const std::string& Token() const;
			virtual const std::string& ShortHelp() const;
			virtual const std::string& Help() const;

			bool AddOption( const OptionPtr& option, std::string& error );
			bool ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

			bool RegisterCommand( Command* command, std::string& error );
			Command* GetCommand( const std::string& token );

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );
		};
	}
}

#include "Application/CmdLineProcessor.inl"