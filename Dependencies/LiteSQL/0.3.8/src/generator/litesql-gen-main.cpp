#include "litesql.hpp"
#include "litesql_char.hpp"
#include "generator.hpp"
#include "litesql-gen-cpp.hpp"
#include "litesql-gen-graphviz.hpp"
#include "litesql-gen-ruby-activerecord.hpp"
#include "logger.hpp"
#include "objectmodel.hpp"

using namespace litesql;

const LiteSQL_Char* help = 
 LiteSQL_L( "Usage: litesql-gen [options] <my-database.xml>\n\n" )
 LiteSQL_L( "Options:\n" )
 LiteSQL_L( " -t, --target=TARGET                generate code for TARGET (default: c++)\n" )
 LiteSQL_L( " -v, --verbose                      verbosely report code generation\n" )
 LiteSQL_L( " --help                             print help\n" )

 LiteSQL_L( " -t, --target=TARGET                generate code for TARGET (default: c++)\n" )
 LiteSQL_L( " --output-dir=/path/to/src          output all files to directory \n" )
 LiteSQL_L( " --output-sources=/path/to/src      output sources to directory \n" )
 LiteSQL_L( " --output-include=/path/to/include  output includes to directory\n" )
 LiteSQL_L( " --refresh                          refresh code of target\n" )
 LiteSQL_L( " --overwrite                        overwrite code on generation\n" )
 LiteSQL_L( "\n" )
 LiteSQL_L( "Supported targets:\n" )
 LiteSQL_L( "  'c++'        C++ target (.cpp,.hpp)\n" )
 LiteSQL_L( "  'ruby-activerecord'          ruby target (.rb)\n" )
// LiteSQL_L( "  'objc'          Objective C (.m,.h)\n" )
// LiteSQL_L( "  'c'          C target (.c,.h)\n" )
// LiteSQL_L( "  'haskell'    Haskell target (.hs)\n" )
// LiteSQL_L( "  'sql'        SQL schema of database (.sql)\n" )
// LiteSQL_L( "  'php'        PHP target (.php)\n" )
// LiteSQL_L( "  'python'     Python target (.py)\n" )
 LiteSQL_L( "  'graphviz'   Graphviz file (.dot)\n" )
 LiteSQL_L( "\n\n" )
;

struct options_t {
  LiteSQL_String output_dir;
  LiteSQL_String output_sources;
  LiteSQL_String output_includes;
  bool refresh;
  bool printHelp;
  std::vector<LiteSQL_String> targets;
};

options_t options = { LiteSQL_L( "" ), LiteSQL_L( "" ), LiteSQL_L( "" ),true,false};

int parseArgs(int argc, LiteSQL_Char **argv) 
{
  if(argc==1)
    return -1;

  for (int i = 1; i < argc; i++) {
    LiteSQL_String arg = argv[i];
    if (arg ==  LiteSQL_L( "-v" ) || arg ==  LiteSQL_L( "--verbose" )) {
      Logger::verbose(true);
      continue;
    } else if (arg ==  LiteSQL_L( "-t" ) || arg ==  LiteSQL_L( "--target" )) {
      if (i+1 >= argc) {
        Logger::error( LiteSQL_L( "Error: missing target" ));
        return -1;
      }    
      options.targets.push_back(argv[i+1]);
      i++;
      continue;
    } else if (litesql::startsWith(arg,  LiteSQL_L( "--target=" ))) {
      litesql::Split lang(arg,  LiteSQL_L( "=" ));
      options.targets.push_back(lang[1]);
      continue;
    }  else if (litesql::startsWith(arg,  LiteSQL_L( "--output-dir" ))) {
      litesql::Split lang(arg,  LiteSQL_L( "=" ));
      options.output_dir=lang[1];
      continue;
    } else if (litesql::startsWith(arg,  LiteSQL_L( "--output-sources" ))) {
      litesql::Split lang(arg,  LiteSQL_L( "=" ));
      options.output_sources=lang[1];
      continue;
    }  else if (litesql::startsWith(arg,  LiteSQL_L( "--output-include" ))) {
      litesql::Split lang(arg,  LiteSQL_L( "=" ));
      options.output_includes=lang[1];
      continue;
    }
    else if (arg ==  LiteSQL_L( "--help" )) {
      options.printHelp = true;
      continue;
    } else if (i < argc - 1) {
      Logger::error( LiteSQL_L( "Error: invalid argument " )+ arg);
      return -1;
    }
  }
  return 0;
}


int generateCode(ObjectModel& model)
{
    CompositeGenerator generator;
    
    generator.setOutputDirectory(options.output_dir);
    
    for (std::vector<LiteSQL_String>::const_iterator target= options.targets.begin(); target!=options.targets.end();target++)
    {

      if (*target ==  LiteSQL_L( "c++" )) 
      {
        CppGenerator* pCppGen = new CppGenerator();
        pCppGen->setOutputSourcesDirectory(options.output_sources);
        pCppGen->setOutputIncludesDirectory(options.output_includes);

        generator.add(pCppGen);
      }    
      else if (*target ==  LiteSQL_L( "graphviz" )) 
      {
        generator.add(new GraphvizGenerator());
      }
      else if (*target ==  LiteSQL_L( "ruby-activerecord" )) 
      {
        generator.add(new RubyActiveRecordGenerator());
      }
      else 
      {
        throw litesql::Except( LiteSQL_L( "unsupported target: " ) + *target);
      }
    }

    return generator.generateCode(&model)? 0 : 1 ;
}

int main(int argc, LiteSQL_Char **argv) { 

  int rc = parseArgs(argc,argv);
  if (rc!=0)
  {
    Logger::error(help);
    return -1;
  }

  if (options.printHelp) {
    LiteSQL_cout << help << std::endl;
  }

  ObjectModel model;
  try {
    if (!model.loadFromFile(argv[argc-1]))
    {
      LiteSQL_String msg =  LiteSQL_L( "could not load file '" ) + LiteSQL_String(argv[argc-1]) +  LiteSQL_L( "'" );
      Logger::error(msg);
      return -1;
    }
    else
    {
      return generateCode(model); 
    }
  } 
  catch (Except e) {
      Logger::error(e);
      return -1;
  }
}