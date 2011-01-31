#include "litesql.hpp"
#include "generator.hpp"
#include "litesql-gen-cpp.hpp"
#include "litesql-gen-graphviz.hpp"
#include "litesql-gen-ruby-activerecord.hpp"
#include "logger.hpp"
#include "objectmodel.hpp"

using namespace litesql;

const LITESQL_Char* help = 
LITESQL_L("Usage: litesql-gen [options] <my-database.xml>\n\n")
LITESQL_L("Options:\n")
LITESQL_L(" -t, --target=TARGET                generate code for TARGET (default: c++)\n")
LITESQL_L(" -v, --verbose                      verbosely report code generation\n")
LITESQL_L(" --help                             print help\n")

LITESQL_L(" -t, --target=TARGET                generate code for TARGET (default: c++)\n")
LITESQL_L(" --output-dir=/path/to/src          output all files to directory \n")
LITESQL_L(" --output-sources=/path/to/src      output sources to directory \n")
LITESQL_L(" --output-include=/path/to/include  output includes to directory\n")
LITESQL_L(" --refresh                          refresh code of target\n")
LITESQL_L(" --overwrite                        overwrite code on generation\n")
LITESQL_L("\n")
LITESQL_L("Supported targets:\n")
LITESQL_L("  'c++'        C++ target (.cpp,.hpp)\n")
LITESQL_L("  'ruby-activerecord'          ruby target (.rb)\n")
//LITESQL_L("  'objc'          Objective C (.m,.h)\n")
//LITESQL_L("  'c'          C target (.c,.h)\n")
//LITESQL_L("  'haskell'    Haskell target (.hs)\n")
//LITESQL_L("  'sql'        SQL schema of database (.sql)\n")
//LITESQL_L("  'php'        PHP target (.php)\n")
//LITESQL_L("  'python'     Python target (.py)\n")
LITESQL_L("  'graphviz'   Graphviz file (.dot)\n")
LITESQL_L("\n\n")
;

struct options_t {
  LITESQL_String output_dir;
  LITESQL_String output_sources;
  LITESQL_String output_includes;
  bool refresh;
  bool printHelp;
  std::vector<LITESQL_String> targets;
};

options_t options = {LITESQL_L(""),LITESQL_L(""),LITESQL_L(""),true,false};

int parseArgs(int argc, LITESQL_Char **argv) 
{
  if(argc==1)
    return -1;

  for (int i = 1; i < argc; i++) {
    LITESQL_String arg = argv[i];
    if (arg == LITESQL_L("-v") || arg == LITESQL_L("--verbose")) {
      Logger::verbose(true);
      continue;
    } else if (arg == LITESQL_L("-t") || arg == LITESQL_L("--target")) {
      if (i+1 >= argc) {
        Logger::error(LITESQL_L("Error: missing target"));
        return -1;
      }    
      options.targets.push_back(argv[i+1]);
      i++;
      continue;
    } else if (litesql::startsWith(arg, LITESQL_L("--target="))) {
      litesql::Split lang(arg, LITESQL_L("="));
      options.targets.push_back(lang[1]);
      continue;
    }  else if (litesql::startsWith(arg, LITESQL_L("--output-dir"))) {
      litesql::Split lang(arg, LITESQL_L("="));
      options.output_dir=lang[1];
      continue;
    } else if (litesql::startsWith(arg, LITESQL_L("--output-sources"))) {
      litesql::Split lang(arg, LITESQL_L("="));
      options.output_sources=lang[1];
      continue;
    }  else if (litesql::startsWith(arg, LITESQL_L("--output-include"))) {
      litesql::Split lang(arg, LITESQL_L("="));
      options.output_includes=lang[1];
      continue;
    }
    else if (arg == LITESQL_L("--help")) {
      options.printHelp = true;
      continue;
    } else if (i < argc - 1) {
      Logger::error(LITESQL_L("Error: invalid argument ")+ arg);
      return -1;
    }
  }
  return 0;
}


int generateCode(ObjectModel& model)
{
    CompositeGenerator generator;
    
    generator.setOutputDirectory(options.output_dir);
    
    for (std::vector<LITESQL_String>::const_iterator target= options.targets.begin(); target!=options.targets.end();target++)
    {

      if (*target == LITESQL_L("c++")) 
      {
        CppGenerator* pCppGen = new CppGenerator();
        pCppGen->setOutputSourcesDirectory(options.output_sources);
        pCppGen->setOutputIncludesDirectory(options.output_includes);

        generator.add(pCppGen);
      }    
      else if (*target == LITESQL_L("graphviz")) 
      {
        generator.add(new GraphvizGenerator());
      }
      else if (*target == LITESQL_L("ruby-activerecord")) 
      {
        generator.add(new RubyActiveRecordGenerator());
      }
      else 
      {
        throw litesql::Except(LITESQL_L("unsupported target: ") + *target);
      }
    }

    return generator.generateCode(&model)? 0 : 1 ;
}

int main(int argc, LITESQL_Char **argv) { 

  int rc = parseArgs(argc,argv);
  if (rc!=0)
  {
    Logger::error(help);
    return -1;
  }

  if (options.printHelp) {
    LITESQL_cout << help << std::endl;
  }

  ObjectModel model;
  try {
    if (!model.loadFromFile(argv[argc-1]))
    {
      LITESQL_String msg = LITESQL_L("could not load file '") + LITESQL_String(argv[argc-1]) + LITESQL_L("'");
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