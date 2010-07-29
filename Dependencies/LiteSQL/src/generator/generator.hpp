#ifndef generator_hpp
#define generator_hpp
#include "litesql_char.hpp"
#include <vector>
#include <ostream>

#include "objectmodel.hpp"


namespace litesql {
  class CodeGenerator {
  public:
    typedef enum generation_mode_t { REFRESH=0,OVERWRITE };
    
    virtual ~CodeGenerator();
    
    virtual void setOutputDirectory(const LITESQL_String& directory);
    virtual const LITESQL_String& getOutputDirectory() const;
    
    void setGenerationMode(generation_mode_t mode) { m_generationMode = mode; };
    generation_mode_t getGenerationMode() const { return m_generationMode; };
    
    LITESQL_String getOutputFilename(const LITESQL_String& name) const;
    
    virtual const LITESQL_Char* getTarget() const;
    virtual bool generateCode(const ObjectModel* model)=0;
    
    CodeGenerator* const getParentGenerator() const { return m_pParentGenerator; };
    void setParentGenerator(CodeGenerator* parent)  { m_pParentGenerator=parent; };
    
    bool generate(const std::vector<xml::Object* >& objects);
    bool generate(const std::vector<xml::Relation* >& relations);
    virtual bool generate(xml::Object* const object) {return true;};
    virtual bool generate(xml::Relation* const relation){return true;};
    
    bool generate(const std::vector<xml::Object* >& objects,LITESQL_oStream& os,size_t indent=2);
    bool generate(const std::vector<xml::Relation* >& relations,LITESQL_oStream& os,size_t indent=2);
    //virtual void generate(LITESQL_oStream& os,const ObjectModel* model,size_t indent=0);
    
    virtual bool generate(xml::Object* const object    ,LITESQL_oStream& os, size_t indent=2) {return true;};
    //virtual void generate(LITESQL_oStream& os,xml::Field* field     , size_t indent=4){};
    //virtual void generate(LITESQL_oStream& os,xml::Method* pMethod  , size_t indent=4){};
    
    virtual bool generate(xml::Relation* const relation,LITESQL_oStream& os,size_t indent=4){return true;};
    
    //  static CodeGenerator* create(const LITESQL_Char* target);
    
  protected:
    CodeGenerator(const LITESQL_Char* target)
    : m_target(target),m_generationMode(REFRESH) {};
    
  private:
    const LITESQL_Char* m_target;
    generation_mode_t m_generationMode;
    LITESQL_String m_directory;
    CodeGenerator* m_pParentGenerator;
  };
  
  class CompositeGenerator : public CodeGenerator {
  public:
    CompositeGenerator(): CodeGenerator(LITESQL_L("composite")) { };
    CompositeGenerator(const LITESQL_Char* target) : CodeGenerator(target) {};
        
    void setOutputDirectory(const LITESQL_String& directory);
    const LITESQL_String& getOutputDirectory() const;
    void add(CodeGenerator* g);
    bool generateCode(const ObjectModel* model);
  private:
    std::vector<CodeGenerator*> generators;
  };
  
}

#endif //#ifndef generator_hpp
