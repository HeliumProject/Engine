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

bool FailTest::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
    error = "failed due to test fail command";
    return false;
}

bool FailTest::Process( std::string& error )
{
    error = "failed due to test fail command";
    return false;
}