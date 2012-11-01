#pragma once

#include "Application/API.h"
#include "Foundation/SmartPtr.h"

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
			tstring m_Token;
			tstring m_Usage;
			tstring m_Help;

		public:
			Option( const tchar_t* token, const tchar_t* usage = TXT( "<ARG>" ), const tchar_t* help = TXT( "" ) )
				: m_Token( token )
				, m_Usage( usage )
				, m_Help( help )
			{
			}

			virtual ~Option()
			{
			}

			virtual const tstring& Token() const
			{
				return m_Token;
			}

			virtual const tstring& Usage() const
			{
				return m_Usage;
			}

			virtual const tstring& Help() const
			{
				return m_Help;
			}
	
			virtual bool Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) = 0;
		};
		typedef Helium::SmartPtr< Option > OptionPtr;
		typedef std::vector< OptionPtr > V_OptionPtr;
		typedef std::map< tstring, OptionPtr > M_StringToOptionPtr;

		template <class T>
		class SimpleOption : public Option
		{  
		protected:
			T* m_Data;

		public:
			SimpleOption( T* data, const tchar_t* token, const tchar_t* usage = TXT( "<ARG>" ), const tchar_t* help = TXT( "" ) )
				: Option( token, usage, help )
				, m_Data( data )
			{
			}

			virtual ~SimpleOption()
			{
			}

			virtual bool Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE
			{
				if ( argsBegin != argsEnd )
				{
					const tstring& arg = (*argsBegin);
					++argsBegin;

					tstringstream str ( arg );
					str >> *m_Data;

					return str.fail();
				}
				
				error = tstring( TXT("Missing parameter for option: ") ) + m_Token;
				return false;
			}

		};

		template <>
		bool SimpleOption<tstring>::Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
		{
			if ( argsBegin != argsEnd )
			{
				*m_Data = (*argsBegin);
				++argsBegin;

				//HELIUM_ASSERT( !(*m_Data).empty() );

				return true;
			}

			error = tstring( TXT( "Missing parameter for option: " ) ) + m_Token;
			return false;
		}

		template <>
		bool SimpleOption<bool>::Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
		{
			// TODO: use in_avail
			if ( argsBegin != argsEnd )
			{
				const tstring& arg = (*argsBegin);
				++argsBegin;

				if ( _tcsicmp( arg.c_str(), TXT( "false" ) ) == 0 || _tcsicmp( arg.c_str(), TXT( "0" ) ) == 0 )
				{
					*m_Data = false;
				}
				else if ( _tcsicmp( arg.c_str(), TXT( "true" ) ) == 0 || _tcsicmp( arg.c_str(), TXT( "1" ) ) == 0 )
				{
					*m_Data = true;
				}

				return true;
			}

			error = tstring( TXT( "Missing parameter for option: " ) ) + m_Token;
			return false;
		}

		template <>
		bool SimpleOption<std::vector< tstring >>::Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE
		{
			// tokenize and push_back via m_Data
			bool result = false;

			while ( argsBegin != argsEnd )
			{
				// stop looking once we get to the optional params
				const tstring& arg = (*argsBegin);

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
				error = tstring( TXT( "Must pass one or more arguments to the option: " ) ) + m_Token;
				return false;
			}

			return result;
		}

		class HELIUM_APPLICATION_API FlagOption : public SimpleOption<bool>
		{
		protected:
			bool* m_Data;

		public:
			FlagOption( bool* data, const tchar_t* token, const tchar_t* help = TXT( "" ) )
				: SimpleOption( data, token, TXT( "" ), help )
				, m_Data( data )
			{
				*m_Data = false;
			}

			virtual ~FlagOption()
			{
			}

			virtual bool Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE
			{
				*m_Data = true;
				return true;
			}

		};

		class HELIUM_APPLICATION_API OptionsMap
		{
		public:
			mutable tstring m_Usage;
			mutable tstring m_Help;

			M_StringToOptionPtr m_OptionsMap;
			V_OptionPtr m_Options;

		public:
			OptionsMap();
			virtual ~OptionsMap();

			const tstring& Usage() const;
			const tstring& Help() const;

			bool AddOption( const OptionPtr& option, tstring& error );
			bool ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );
		};

		class HELIUM_APPLICATION_API Command
		{
		protected:
			tstring m_Token;
			tstring m_Usage;
			tstring m_ShortHelp;
			mutable tstring m_Help;

			OptionsMap m_OptionsMap;

		public:
			Command( const tchar_t* token, const tchar_t* usage = TXT( "[OPTIONS]" ), const tchar_t* shortHelp = TXT( "" ) );
			virtual ~Command();

			virtual bool Initialize( tstring& error )
			{
				return true;
			}

			virtual void Cleanup()
			{
			}

			const tstring& Token() const
			{
				return m_Token;
			}

			const tstring& ShortHelp() const
			{
				return m_ShortHelp;
			}

			virtual const tstring& Help() const;

			bool AddOption( const OptionPtr& option, tstring& error );
			bool ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) = 0;
		};

		typedef std::map< tstring, Command* > M_StringToCommandDumbPtr;

		class Processor;

		class HELIUM_APPLICATION_API HelpCommand : public Command
		{
		protected:
			Processor* m_Owner;
			tstring m_CommandName;

		public:
			HelpCommand( Processor* owner = NULL );
			virtual ~HelpCommand();

			void SetOwner( Processor* owner )
			{
				m_Owner = owner;
			}

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE;
		};
		
		class HELIUM_APPLICATION_API Processor
		{
		protected:
			tstring m_Token;
			tstring m_Usage;
			tstring m_ShortHelp;
			mutable tstring m_Help;

			OptionsMap m_OptionsMap;
			M_StringToCommandDumbPtr m_Commands;

		public:
			Processor( const tchar_t* token, const tchar_t* usage = TXT( "COMMAND [ARGS]" ), const tchar_t* shortHelp = TXT( "" ) );
			virtual ~Processor();

			virtual bool Initialize( tstring& error )
			{
				return true;
			}

			const tstring& Token() const
			{
				return m_Token;
			}

			const tstring& ShortHelp() const
			{
				return m_ShortHelp;
			}

			virtual const tstring& Help() const;

			bool AddOption( const OptionPtr& option, tstring& error );
			bool ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );

			bool RegisterCommand( Command* command, tstring& error );
			Command* GetCommand( const tstring& token );

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );
		};
	}
}
