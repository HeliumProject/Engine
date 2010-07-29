#ifndef xmlgenerator_hpp
#define xmlgenerator_hpp
#include "litesql_char.hpp"
#include <ostream>
#include "generator.hpp"

namespace litesql {
  class XmlGenerator : public CodeGenerator {
  public:
    XmlGenerator(): CodeGenerator(LITESQL_L("xml")) { };
    virtual void setOutputFilename(const LITESQL_String& filename);
    bool generateCode(const ObjectModel* model);
    
  protected:
    bool generate(xml::Object *const object     , LITESQL_oStream& os , size_t indent=2);
    bool generate(xml::Relation* const relation , LITESQL_oStream& os , size_t indent=4);

  private:
    bool generateDatabase(LITESQL_oStream& os,const ObjectModel* model);
    LITESQL_String m_outputFilename;
  };
}

#endif //#ifndef xmlgenerator_hpp
