#ifndef _litesql_gen_graphviz_hpp
#define _litesql_gen_graphviz_hpp

#include "generator.hpp"

namespace litesql {
  
  class GraphvizGenerator : public CodeGenerator {
  public:
    static const LITESQL_Char* NAME;

    GraphvizGenerator(): CodeGenerator(NAME) { };
    bool generateCode(const ObjectModel* model);
  protected:   
    bool generate(const xml::ObjectPtr& object     , LITESQL_oStream& os , size_t indent);
    bool generate(const xml::Relation::counted_ptr& relation , LITESQL_oStream& os , size_t indent);
  };
}

#endif