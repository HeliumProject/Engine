#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

#include "Platform/Path.h"
#include "Platform/Windows/Windows.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT(Version);

void Version::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Version::m_Source, TXT( "m_Source" ) );
    comp.AddField( &Version::m_SourceVersion, TXT( "m_SourceVersion" ) );
}

Reflect::Version::Version()
{
    DetectVersion();
}

Reflect::Version::Version(const tchar_t* source, const tchar_t* sourceVersion)
: m_Source (source)
, m_SourceVersion (sourceVersion)
{

}

bool Reflect::Version::IsCurrent()
{
    return true;
}

bool Reflect::Version::ConvertToInts( int* ints )
{
    bool return_val = false;

    if ( !m_SourceVersion.empty() )
    {
        return_val = true;

        tstring digit;
        int tmp;

        size_t cur = 0, mark = -1;
        for ( int i = 0; i < 4; ++i )
        {
            ints[i] = 0;

            mark = i == 3 ? m_SourceVersion.length() : m_SourceVersion.find( '.', cur );
            if ( mark == -1 )
            {
                return_val = false;
                break;
            }

            digit = m_SourceVersion.substr( cur, mark - cur );

            if ( 1 == _stscanf( digit.c_str(), TXT( "%d" ), &tmp) )
            {
                ints[i] = tmp;
            }
            else
            {
                return_val = false;
                break;
            }

            cur = mark + 1;
        }
    }

    return return_val;
}

void Reflect::Version::DetectVersion()
{
    HMODULE moduleHandle = GetModuleHandle( NULL );

    tchar_t exeFilename[ MAX_PATH + 1 ];
    GetModuleFileName( moduleHandle, exeFilename, MAX_PATH );

    m_Source = exeFilename;

    // if we found an executable, get its version
    if ( !m_Source.empty() )
    {
        Helium::GetVersionInfo( m_Source.c_str(), m_SourceVersion );

        m_Source = Helium::Path( m_Source ).Filename();
    }
}
