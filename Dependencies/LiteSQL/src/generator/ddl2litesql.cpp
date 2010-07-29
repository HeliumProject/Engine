#include "litesql.hpp"
#include "logger.hpp"
#include "objectmodel.hpp"
#include <fstream>

using namespace litesql;

const LiteSQL_Char* help = 
 LiteSQL_L( "Usage: ddl2litesql [options] <my-database.xml>\n\n" )
 LiteSQL_L( "Options:\n" )
 LiteSQL_L( " -v, --verbose                      verbosely report code generation\n" )
 LiteSQL_L( " --help                             print help\n" )

 LiteSQL_L( " --output-dir=/path/to/src          output all files to directory \n" )
 LiteSQL_L( "\n" )
 LiteSQL_L( "\n\n" )
;

struct options_t {
  LiteSQL_String output_dir;
  bool printHelp;
};

options_t options = { LiteSQL_L( "" ),false};

int parseArgs(int argc, LiteSQL_Char **argv) 
{
  if(argc<2)
    return -1;

  for (int i = 1; i < argc; i++) {
    LiteSQL_String arg = argv[i];
    if (arg ==  LiteSQL_L( "-v" ) || arg ==  LiteSQL_L( "--verbose" )) {
      Logger::verbose(true);
      continue;
    } else if (litesql::startsWith(arg,  LiteSQL_L( "--output-dir" ))) {
      litesql::Split param(arg,  LiteSQL_L( "=" ));
      options.output_dir=param[1];
      continue;
    } else if (arg ==  LiteSQL_L( "--help" )) {
      options.printHelp = true;
      continue;
    } else if (i < argc - 1) {
      Logger::error( LiteSQL_L( "Error: invalid argument " )+ arg);
      return -1;
    }
  }
  return 0;
}

int main(int argc, LiteSQL_Char **argv) { 

  int rc = parseArgs(argc,argv);
  if (rc!=0)
  {
    Logger::error(help);
    return EXIT_FAILURE;
  }
  else
  {
    if (options.printHelp) {
      LiteSQL_cout << help << std::endl;
    }

    LiteSQL_ifStream is;

    is.open(argv[argc-1]);
    if (!is.is_open())
    {
      Logger::error( LiteSQL_L( "cant open " ),argv[argc-1]);
      return EXIT_FAILURE;
    }
    else
    {
      ObjectModel model;
//    LiteSQL_cout << argv[argc-1] << std::endl;

    }
  }
}