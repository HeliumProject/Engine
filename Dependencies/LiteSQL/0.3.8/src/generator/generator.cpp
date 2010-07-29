#include "generator.hpp"
#include "objectmodel.hpp"
#include "litesql_char.hpp"
//#include <fstream>

using namespace litesql;

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

bool CodeGenerator::generate(const std::vector<xml::Object* >& objects)
{
  for (std::vector<xml::Object* >::const_iterator it = objects.begin();
    it != objects.end();
    it++)
  {
      generate(*it);
  }
  return true;
}

bool CodeGenerator::generate(const std::vector<xml::Relation* >& relations)
{
  for (std::vector<xml::Relation* >::const_iterator it = relations.begin();
    it != relations.end();
    it++)
  {
      generate(*it);
  }
  return true;
}

bool CodeGenerator::generate(const std::vector<xml::Object* >& objects,LITESQL_oStream& os,size_t indent)
{
  for (std::vector<xml::Object* >::const_iterator it = objects.begin();
    it != objects.end();
    it++)
  {
      generate(*it,os,indent);
  }
  return true;
}

bool CodeGenerator::generate(const std::vector<xml::Relation* >& relations,LITESQL_oStream& os,size_t indent)
{
  for (std::vector<xml::Relation* >::const_iterator it = relations.begin();
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