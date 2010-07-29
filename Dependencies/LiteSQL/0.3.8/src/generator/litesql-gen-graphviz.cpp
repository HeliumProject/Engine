#include "litesql_char.hpp"
#include "litesql-gen-graphviz.hpp"
#include <fstream>

using namespace xml;

bool GraphvizGenerator::generate(Object* const object    ,LITESQL_oStream& os, size_t indent)
{
  LITESQL_String indents(indent,' ');
  os << indents << '"' << object->name << '"';
  
  if (object->parentObject)
    os <<  LITESQL_L(" -> \"") << object->inherits <<  LITESQL_L("\"");
  
  os << ';' << std::endl;
  return true;
}

bool GraphvizGenerator::generate(Relation* const relation,LITESQL_oStream& os,size_t indent)
{
  LITESQL_String indents(indent,' ');
  Relation& r = *relation;
  for (size_t i2 = 0; i2 < r.related.size(); i2++) {
    Relate& rel = *r.related[i2];
    LITESQL_String extra;
    for (size_t i3 = 0; i3 < r.related.size(); i3++) {
      if (i3 == i2) 
        continue;

      Relate& destRel = *r.related[i3];
      if (rel.handle.size() > 0) {
        extra =  LITESQL_L(" [label=\"") + rel.handle +  LITESQL_L("\"]");
        os << indents <<  LITESQL_L("\"") << rel.objectName <<  LITESQL_L("\" -> \"")<< destRel.objectName << LITESQL_L("\"")<<  LITESQL_L(" [label=\"") << rel.handle <<  LITESQL_L("\"]") << LITESQL_L(";") <<std::endl; 
      }
    }
  }
  return true;
}

bool GraphvizGenerator::generateCode(const ObjectModel* model)
{

  LITESQL_String fname = getOutputFilename(toLower(model->db.name +  LITESQL_L(".dot")));

  LITESQL_ofSstream os(fname.c_str());
  os <<  LITESQL_L("digraph database {") << std::endl
     <<  LITESQL_L("  node[shape=box,color=black];") << std::endl
     <<  LITESQL_L("  subgraph inheritance {") << std::endl
     <<  LITESQL_L("    edge[style=dashed,dir=forward,arrowhead=normal];") << std::endl;
    
  CodeGenerator::generate(model->objects,os,4);

  os <<  LITESQL_L("  }") << std::endl
     <<  LITESQL_L("  subgraph relations {") << std::endl
     <<  LITESQL_L("    edge[dir=forward,arrowhead=vee];") << std::endl;
  
  CodeGenerator::generate(model->relations,os,4);
  
  os <<  LITESQL_L("  }") << std::endl
     <<  LITESQL_L("}") << std::endl;

  os.close();
  return true;
}

