#ifndef _litesql_gen_graphviz_hpp
#define _litesql_gen_graphviz_hpp
#include "litesql_char.hpp"
#include "generator.hpp"

namespace litesql {
  
  class GraphvizGenerator : public CodeGenerator {
  public:
    GraphvizGenerator(): CodeGenerator(LITESQL_L("graphviz")) { };
    bool generateCode(const ObjectModel* model);
  protected:   
    bool generate(xml::Object* const object     , LITESQL_oStream& os , size_t indent);
    bool generate(xml::Relation* const relation , LITESQL_oStream& os , size_t indent);
  };
}

#endif