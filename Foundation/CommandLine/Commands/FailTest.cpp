#include "FailTest.h"

using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

FailTest::FailTest()
: Command( "fail", "", "This command causes the application to fail. This is strictly for testing purposes." )
{
}

FailTest::~FailTest()
{
}

bool FailTest::Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	if ( !m_OptionsMap.ParseOptions( argsBegin, argsEnd, error ) )
	{
		return false;
	}

    error = "failed due to test fail command";
    return false;
}