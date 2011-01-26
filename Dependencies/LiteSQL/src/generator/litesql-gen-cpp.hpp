#ifndef litesql_gen_cpp_hpp
#define litesql_gen_cpp_hpp

#include "generator.hpp"

namespace litesql {
  
  class CppGenerator : public CodeGenerator {
  public:
    static const LITESQL_Char* NAME;
    CppGenerator(): CodeGenerator(NAME) { };
    bool generateCode(const ObjectModel* model);
    
    void setOutputSourcesDirectory(const LITESQL_String& directory);
    void setOutputIncludesDirectory(const LITESQL_String& directory);
    
    const LITESQL_String& getOutputSourcesDirectory() const;
    const LITESQL_String& getOutputIncludesDirectory() const;
    
    LITESQL_String getOutputSourcesFilename(const LITESQL_String& name) const;
    LITESQL_String getOutputIncludesFilename(const LITESQL_String& name) const;
    
  private:
    LITESQL_String m_sources_directory;
    LITESQL_String m_includes_directory;
    
    void writeCPPClasses(const ObjectModel* model);
    FILE *hpp, *cpp;
  };
  
}
#endif