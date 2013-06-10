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
			Option( const char* token, const char* usage = TXT( "<ARG>" ), const char* help = TXT( "" ) )
				: m_Token( token )
				, m_Usage( usage )
				, m_Help( help )
			{
			}

			virtual ~Option()
			{
			}

			virtual const std::string& Token() const
			{
				return m_Token;
			}

			virtual const std::string& Usage() const
			{
				return m_Usage;
			}

			virtual const std::string& Help() const
			{
				return m_Help;
			}
	
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
			SimpleOption( T* data, const char* token, const char* usage = TXT( "<ARG>" ), const char* help = TXT( "" ) )
				: Option( token, usage, help )
				, m_Data( data )
			{
			}

			virtual ~SimpleOption()
			{
			}

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) HELIUM_OVERRIDE
			{
				if ( argsBegin != argsEnd )
				{
					const std::string& arg = (*argsBegin);
					++argsBegin;

					std::stringstream str ( arg );
					str >> *m_Data;

					return str.fail();
				}
				
				error = std::string( TXT("Missing parameter for option: ") ) + m_Token;
				return false;
			}

		};

		template <>
		bool SimpleOption<std::string>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
		{
			if ( argsBegin != argsEnd )
			{
				*m_Data = (*argsBegin);
				++argsBegin;

				//HELIUM_ASSERT( !(*m_Data).empty() );

				return true;
			}

			error = std::string( TXT( "Missing parameter for option: " ) ) + m_Token;
			return false;
		}

		template <>
		bool SimpleOption<bool>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
		{
			// TODO: use in_avail
			if ( argsBegin != argsEnd )
			{
				const std::string& arg = (*argsBegin);
				++argsBegin;

				if ( CaseInsensitiveCompareString( arg.c_str(), TXT( "false" ) ) == 0 || CaseInsensitiveCompareString( arg.c_str(), TXT( "0" ) ) == 0 )
				{
					*m_Data = false;
				}
				else if ( CaseInsensitiveCompareString( arg.c_str(), TXT( "true" ) ) == 0 || CaseInsensitiveCompareString( arg.c_str(), TXT( "1" ) ) == 0 )
				{
					*m_Data = true;
				}

				return true;
			}

			error = std::string( TXT( "Missing parameter for option: " ) ) + m_Token;
			return false;
		}

		template <>
		bool SimpleOption<std::vector< std::string >>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) HELIUM_OVERRIDE
		{
			// tokenize and push_back via m_Data
			bool result = false;

			while ( argsBegin != argsEnd )
			{
				// stop looking once we get to the optional params
				const std::string& arg = (*argsBegin);

				if ( arg.length() >= 1 )
				{
					if ( arg[ 0 ] == '-' )
					{
						break;
					}
					else
					{
						++argsBegin;

						(*m_Data).push_back( arg );

						result = true;
					}
				}

			}

			if ( !result || (*m_Data).empty() )
			{
				error = std::string( TXT( "Must pass one or more arguments to the option: " ) ) + m_Token;
				return false;
			}

			return result;
		}

		class HELIUM_APPLICATION_API FlagOption : public SimpleOption<bool>
		{
		protected:
			bool* m_Data;

		public:
			FlagOption( bool* data, const char* token, const char* help = TXT( "" ) )
				: SimpleOption( data, token, TXT( "" ), help )
				, m_Data( data )
			{
				*m_Data = false;
			}

			virtual ~FlagOption()
			{
			}

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) HELIUM_OVERRIDE
			{
				*m_Data = true;
				return true;
			}

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
			virtual ~OptionsMap();

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
			virtual ~Command();

			virtual bool Initialize( std::string& error )
			{
				return true;
			}

			virtual void Cleanup()
			{
			}

			const std::string& Token() const
			{
				return m_Token;
			}

			const std::string& ShortHelp() const
			{
				return m_ShortHelp;
			}

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
			virtual ~HelpCommand();

			void SetOwner( Processor* owner )
			{
				m_Owner = owner;
			}

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

			virtual bool Initialize( std::string& error )
			{
				return true;
			}

			const std::string& Token() const
			{
				return m_Token;
			}

			const std::string& ShortHelp() const
			{
				return m_ShortHelp;
			}

			virtual const std::string& Help() const;

			bool AddOption( const OptionPtr& option, std::string& error );
			bool ParseOptions( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );

			bool RegisterCommand( Command* command, std::string& error );
			Command* GetCommand( const std::string& token );

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );
		};
	}
}
