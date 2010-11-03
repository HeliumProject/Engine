#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace CommandLine
    {
		///////////////////////////////////////////////////////////////////////
        class FOUNDATION_API Option : public Helium::RefCountBase< Option >
        {
		protected:
			tstring m_Token;
			tstring m_Usage;
			tstring m_Help;

        public:
			Option( const tchar* token, const tchar* usage = TXT( "<ARG>" ), const tchar* help = TXT( "" ) )
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

		///////////////////////////////////////////////////////////////////////
		template <class T>
		class SimpleOption : public Option
		{  
		protected:
			T* m_Data;

		public:
			SimpleOption( T* data, const tchar* token, const tchar* usage = TXT( "<ARG>" ), const tchar* help = TXT( "" ) )
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


		///////////////////////////////////////////////////////////////////////
		class FOUNDATION_API FlagOption : public SimpleOption<bool>
        {
		protected:
			bool* m_Data;

		public:
			FlagOption( bool* data, const tchar* token, const tchar* help = TXT( "" ) )
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

		///////////////////////////////////////////////////////////////////////
		//class FOUNDATION_API DelimitedListOption : public SimpleOption< std::vector< tstring > >
		//{
		//protected:
		//	std::vector< tstring >* m_Data;
		//	tstring m_Delimiter;

		//public:
		//	DelimitedListOption( std::vector< tstring >* data, const char* token, const char* usage = "<ARG> [<ARG> ...]", const char* help = "", const char* delimiter = " " )
		//		: SimpleOption( data, token, usage, help )
		//		, m_Delimiter( delimiter )
		//	{
		//	}

		//	virtual ~DelimitedListOption()
		//	{
		//	}

		//	virtual bool Parse( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE
		//	{
		//		// tokenize and push_back via m_Data
		//		bool result = false;

		//		while ( argsBegin != argsEnd )
		//		{
		//			// stop looking once we get to the optional params
		//			const tstring& arg = (*argsBegin);

		//			if ( arg.length() >= 1 )
		//			{
		//				if ( arg[ 0 ] == '-' )
		//				{
		//					break;
		//				}
		//				else
		//				{
		//					++argsBegin;

		//				    Tokenize( arg, *m_Data, m_Delimiter );

		//					result = true;
		//				}
		//			}
		//			
		//		}

		//		if ( !result || (*m_Data).empty() )
		//		{
		//			error = tstring( "Must pass one (or more) argument to the option: " ) + m_Token;
		//			return false;
		//		}

		//		return result;
		//	}
		//};

		///////////////////////////////////////////////////////////////////////
		class FOUNDATION_API OptionsMap
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
    }
}
