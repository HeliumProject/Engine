#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/API.h"
#include "Platform/Compiler.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Nocturnal
{
    namespace CommandLine
    {
		///////////////////////////////////////////////////////////////////////
        class FOUNDATION_API Option : public Nocturnal::RefCountBase< Option >
        {
		protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_Help;

        public:
			Option( const char* token, const char* usage = "<ARG>", const char* help = "" )
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
		typedef Nocturnal::SmartPtr< Option > OptionPtr;
		typedef std::vector< OptionPtr > V_OptionPtr;
		typedef std::map< std::string, OptionPtr > M_StringToOptionPtr;


		///////////////////////////////////////////////////////////////////////
		template <class T>
		class SimpleOption : public Option
		{  
		protected:
			T* m_Data;

		public:
			SimpleOption( T* data, const char* token, const char* usage = "<ARG>", const char* help = "" )
				: Option( token, usage, help )
				, m_Data( data )
			{
			}

			virtual ~SimpleOption()
			{
			}

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE
			{
				if ( argsBegin != argsEnd )
				{
					const std::string& arg = (*argsBegin);
					++argsBegin;

					std::istringstream str ( arg );
					str >> *m_Data;

					return str.fail();
				}
				
				error = std::string( "Missing parameter for option: " ) + m_Token;
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

				//NOC_ASSERT( !(*m_Data).empty() );

				return true;
			}

			error = std::string( "Missing parameter for option: " ) + m_Token;
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

				if ( stricmp( arg.c_str(), "false" ) == 0 || stricmp( arg.c_str(), "0" ) == 0 )
				{
					*m_Data = false;
				}
				else if ( stricmp( arg.c_str(), "true" ) == 0 || stricmp( arg.c_str(), "1" ) == 0 )
				{
					*m_Data = true;
				}

				return true;
			}

			error = std::string( "Missing parameter for option: " ) + m_Token;
			return false;
		}

		template <>
		bool SimpleOption<std::vector< std::string >>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE
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
				error = std::string( "Must pass one or more arguments to the option: " ) + m_Token;
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
			FlagOption( bool* data, const char* token, const char* help = "" )
				: SimpleOption( data, token, "", help )
				, m_Data( data )
			{
				*m_Data = false;
			}

            virtual ~FlagOption()
            {
            }

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE
			{
				*m_Data = true;
				return true;
			}

        };


		///////////////////////////////////////////////////////////////////////
		//class FOUNDATION_API DelimitedListOption : public SimpleOption< std::vector< std::string > >
		//{
		//protected:
		//	std::vector< std::string >* m_Data;
		//	std::string m_Delimiter;

		//public:
		//	DelimitedListOption( std::vector< std::string >* data, const char* token, const char* usage = "<ARG> [<ARG> ...]", const char* help = "", const char* delimiter = " " )
		//		: SimpleOption( data, token, usage, help )
		//		, m_Delimiter( delimiter )
		//	{
		//	}

		//	virtual ~DelimitedListOption()
		//	{
		//	}

		//	virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE
		//	{
		//		// tokenize and push_back via m_Data
		//		bool result = false;

		//		while ( argsBegin != argsEnd )
		//		{
		//			// stop looking once we get to the optional params
		//			const std::string& arg = (*argsBegin);

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
		//			error = std::string( "Must pass one (or more) argument to the option: " ) + m_Token;
		//			return false;
		//		}

		//		return result;
		//	}
		//};

		///////////////////////////////////////////////////////////////////////
		class FOUNDATION_API OptionsMap
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
    }
}
