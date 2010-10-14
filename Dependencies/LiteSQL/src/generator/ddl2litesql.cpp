#include "litesql.hpp"
#include "logger.hpp"
#include "objectmodel.hpp"
#include <fstream>

using namespace litesql;

const LITESQL_Char* help = 
 LITESQL_L( "Usage: ddl2litesql [options] <my-database.xml>\n\n" )
 LITESQL_L( "Options:\n" )
 LITESQL_L( " -v, --verbose                      verbosely report code generation\n" )
 LITESQL_L( " --help                             print help\n" )

 LITESQL_L( " --output-dir=/path/to/src          output all files to directory \n" )
 LITESQL_L( "\n" )
 LITESQL_L( "\n\n" )
;

struct options_t {
  LITESQL_String output_dir;
  bool printHelp;
};

options_t options = { LITESQL_L( "" ),false};

int parseArgs(int argc, LITESQL_Char **argv) 
{
  if(argc<2)
    return -1;

  for (int i = 1; i < argc; i++) {
    LITESQL_String arg = argv[i];
    if (arg ==  LITESQL_L( "-v" ) || arg ==  LITESQL_L( "--verbose" )) {
      Logger::verbose(true);
      continue;
    } else if (litesql::startsWith(arg,  LITESQL_L( "--output-dir" ))) {
      litesql::Split param(arg,  LITESQL_L( "=" ));
      options.output_dir=param[1];
      continue;
    } else if (arg ==  LITESQL_L( "--help" )) {
      options.printHelp = true;
      continue;
    } else if (i < argc - 1) {
      Logger::error( LITESQL_L( "Error: invalid argument " )+ arg);
      return -1;
    }
  }
  return 0;
}

int main(int argc, LITESQL_Char **argv) { 

  int rc = parseArgs(argc,argv);
  if (rc!=0)
  {
    Logger::error(help);
    return EXIT_FAILURE;
  }
  else
  {
    if (options.printHelp) {
      LITESQL_cout << help << std::endl;
    }

    LITESQL_ifStream is;

    is.open(argv[argc-1]);
    if (!is.is_open())
    {
      Logger::error( LITESQL_L( "cant open " ),argv[argc-1]);
      return EXIT_FAILURE;
    }
    else
    {
      ObjectModel model;
//    LITESQL_cout << argv[argc-1] << std::endl;

    }
  }
}