#include "generator.hpp"
#include "objectmodel.hpp"
#include "litesql-gen-cpp.hpp"
#include "litesql-gen-ruby-activerecord.hpp"
#include "litesql-gen-graphviz.hpp"


using namespace litesql;
using namespace xml;

CodeGenerator::FactoryMap::FactoryMap()
{
  registerFactory(new CodeGenerator::Factory<CppGenerator>(CppGenerator::NAME));
  
  registerFactory(new CodeGenerator::Factory<RubyActiveRecordGenerator>(RubyActiveRecordGenerator::NAME));
  registerFactory(new CodeGenerator::Factory<ActiveRecordClassGenerator>(ActiveRecordClassGenerator::NAME));
  registerFactory(new CodeGenerator::Factory<RubyMigrationsGenerator>(RubyMigrationsGenerator::NAME));

  registerFactory(new CodeGenerator::Factory<GraphvizGenerator>(GraphvizGenerator::NAME));

}

CodeGenerator::FactoryMap::~FactoryMap()
{
  for (iterator it  = begin();
                it != end();
                it++ )
  {
    delete it->second;
  }
}

CodeGenerator::FactoryMap& CodeGenerator::getFactoryMap()
{
  static FactoryMap instance;
  return instance;
}

bool CodeGenerator::registerFactory(AbstractFactory* pFactory)
{
  if (!pFactory)
  {
    return false;
  }
  else
  {
    getFactoryMap()[pFactory->getName()] = pFactory;  
    return true;
  }
}

CodeGenerator* CodeGenerator::create(const LITESQL_Char* target)
{
  FactoryMap::iterator it = getFactoryMap().find(target);
  if (it != getFactoryMap().end() &&  it->second!=NULL)
  {
    return it->second->create();
  }
  else
  {
    return NULL;
  }
}


CodeGenerator::CodeGenerator()
{}
  
CodeGenerator::~CodeGenerator()
{}

void CodeGenerator::setOutputDirectory(const LITESQL_String& directory) 
{
  m_directory = directory;
}
const LITESQL_String& CodeGenerator::getOutputDirectory() const 
{
  return m_directory;
}

LITESQL_String CodeGenerator::getOutputFilename(const LITESQL_String& name) const
{
  LITESQL_String fname = getOutputDirectory();

  if (!fname.empty())
  {
#ifdef WIN32
    fname.append(LITESQL_L("\\"));
#else
    fname.append(LITESQL_L("/"));
#endif // #ifdef _WINDOWS_
  }
  fname.append(name); 
  return fname;
}
    
const LITESQL_Char* CodeGenerator::getTarget() const
{return m_target;}

bool CodeGenerator::generate(const xml::ObjectSequence& objects)
{
  for (xml::ObjectSequence::const_iterator it = objects.begin();
    it != objects.end();
    it++)
  {
      generate(*it);
  }
  return true;
}

bool CodeGenerator::generate(const Relation::sequence& relations)
{
  for (Relation::sequence::const_iterator it = relations.begin();
    it != relations.end();
    it++)
  {
      generate(*it);
  }
  return true;
}

bool CodeGenerator::generate(const ObjectSequence& objects,LITESQL_oStream& os,size_t indent)
{
  for (ObjectSequence::const_iterator it = objects.begin();
    it != objects.end();
    it++)
  {
      generate(*it,os,indent);
  }
  return true;
}

bool CodeGenerator::generate(const Relation::sequence& relations,LITESQL_oStream& os,size_t indent)
{
  for (Relation::sequence::const_iterator it = relations.begin();
    it != relations.end();
    it++)
  {
      generate(*it,os,indent);
  }
  return true;
}


void CompositeGenerator::add(CodeGenerator* g)
{
  g->setParentGenerator(this);
  g->setOutputDirectory(getOutputDirectory());
  generators.push_back(g);

}

void CompositeGenerator::setOutputDirectory(const LITESQL_String& directory) 
{
  CodeGenerator::setOutputDirectory(directory);
  for(std::vector<CodeGenerator*>::iterator it = generators.begin(); it != generators.end();it++)
  {
    (*it)->setOutputDirectory(directory);
  }
}

const LITESQL_String& CompositeGenerator::getOutputDirectory() const
{
  return CodeGenerator::getOutputDirectory();
}

bool CompositeGenerator::generateCode(const ObjectModel* model)
{
  bool success=true;

  for(std::vector<CodeGenerator*>::iterator it = generators.begin(); it != generators.end();it++)
  {
    success &= (*it)->generateCode(model);
  }
  return success;
}