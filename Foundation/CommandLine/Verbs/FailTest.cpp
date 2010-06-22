#include "FailTest.h"

using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

const std::string FailTest::s_Token = "fail";
const std::string FailTest::s_ShortHelp = "fail     - Causes the application to fail for testing purposes";
const std::string FailTest::s_Help = "fail\n  This command causes the application to fail.  This is strictly for testing purposes.";

FailTest::FailTest()
{
}

FailTest::~FailTest()
{
}

bool FailTest::Process( std::vector< std::string >::const_iterator& itr, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
    error = "failed due to test fail command";
    return false;
}