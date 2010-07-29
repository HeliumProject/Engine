#include "FailTest.h"

using namespace Helium;
using namespace Helium::CommandLine;

FailTest::FailTest()
: Command( TXT( "fail" ), TXT( "" ), TXT( "This command causes the application to fail. This is strictly for testing purposes." ) )
{
}

FailTest::~FailTest()
{
}

bool FailTest::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
	if ( !ParseOptions( argsBegin, argsEnd, error ) )
	{
		return false;
	}

    error = TXT( "failed due to test fail command" );
    return false;
}
