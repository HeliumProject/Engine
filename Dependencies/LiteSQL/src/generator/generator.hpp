#ifndef generator_hpp
#define generator_hpp

#include <vector>
#include <map>
#include <ostream>
#include <memory>

#include "objectmodel.hpp"


namespace litesql {

  class CodeGenerator {

  public:
    typedef enum generation_mode_t { REFRESH=0,OVERWRITE };

    CodeGenerator();
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

    bool generate(const xml::ObjectSequence& objects);
    bool generate(const xml::Relation::sequence& relations);
    virtual bool generate(const xml::ObjectPtr& object) {return true;};
    virtual bool generate(const xml::Relation::counted_ptr& relation){return true;};

    bool generate(const xml::ObjectSequence& objects,LITESQL_oStream& os,size_t indent=2);
    bool generate(const xml::Relation::sequence& relations,LITESQL_oStream& os,size_t indent=2);
    
    virtual bool generate(const xml::ObjectPtr& object, LITESQL_oStream& os, size_t indent=2) {return true;};
    
    virtual bool generate(const xml::Relation::counted_ptr& relation, LITESQL_oStream& os, size_t indent=4){return true;};

    static CodeGenerator* create(const LITESQL_Char* target);

  protected:
    CodeGenerator(const LITESQL_Char* target)
      : m_target(target),m_generationMode(REFRESH) {};

  protected:
    class AbstractFactory {
    public: 
      AbstractFactory(const LITESQL_Char* pszName) : name(pszName) {};
      const LITESQL_Char* getName() { return name;};
      virtual CodeGenerator* create() =0;

    private:
      const LITESQL_Char* name;
    };

  public:

    class FactoryMap : public std::map< LITESQL_String,AbstractFactory* > 
    {
    public:
      FactoryMap();
      ~FactoryMap();
    };

    static FactoryMap& getFactoryMap();
    static bool registerFactory(AbstractFactory* f);

    template <class T > class Factory : public AbstractFactory {
    public:
      Factory(const LITESQL_Char* pszName)
        : AbstractFactory(pszName)  
      {};

      CodeGenerator* create(void)  {return new T(); };
    };

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
