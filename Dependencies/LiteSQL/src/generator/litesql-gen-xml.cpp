#include "litesql-gen-xml.hpp"

#include <fstream>

using namespace xml;
using namespace litesql;

void XmlGenerator::setOutputFilename(const LITESQL_String& filename)
{
  m_outputFilename=filename;
}


bool generate(Field::counted_ptr const field, LITESQL_oStream& os,size_t indent)
{
  LITESQL_String indent_string(indent,' ');
       
  os  << indent_string << LITESQL_L("<field ") 
      << attribute(LITESQL_L("name"),field->name) 
      << attribute(LITESQL_L("type"),toString(field->type)) 
      << (field->default_.empty() ? LITESQL_L(""):attribute(LITESQL_L("default"),field->default_)) 
      << (field->isIndexed() ? attribute(LITESQL_L("indexed"),LITESQL_L("true")) : LITESQL_L("")) 
      << (field->isUnique() ? attribute(LITESQL_L("unique"),LITESQL_L("true")) : LITESQL_L("")) 
      ;
  if (field->values.empty())
  {
    os << LITESQL_L("/>") <<std::endl;
  }
  else 
  {
    os << LITESQL_L(">") <<std::endl;
    for (std::vector<xml::Value >::const_iterator it = field->values.begin();
      it != field->values.end();
      it++)
  {
    os << indent_string <<  ' ' << ' ' << LITESQL_L("<value ") 
      << attribute(LITESQL_L("name"), it->name)
      << attribute(LITESQL_L("value"), it->value)
      << LITESQL_L("/>") <<std::endl;
    }
    os << indent_string << endtag(LITESQL_L("field")) <<std::endl;

  }
  return true;
}

void generate(const xml::Method::counted_ptr& pMethod,LITESQL_oStream& os,size_t indent)
{
  LITESQL_String indent_string(indent,' ');
  
  os << indent_string << LITESQL_L("<method ") << attribute(LITESQL_L("name"),pMethod->name );
  
  if (pMethod->params.empty())
  {
    os << LITESQL_L("/>") <<std::endl;
  }
  else
  {
    os << LITESQL_L(">") <<std::endl;
    for (std::vector<xml::Param >::const_iterator it = pMethod->params.begin();
    it != pMethod->params.end();
    it++)
    {
      LITESQL_String tempString;
      LITESQL_ConvertString((*it).type, tempString);
      os << indent_string << LITESQL_L("  ") << LITESQL_L("<param ") << attribute(LITESQL_L("type"),tempString.c_str()) << attribute(LITESQL_L("name"),(*it).name) << LITESQL_L("/>") << std::endl;
    }
    os << endtag(LITESQL_L("method")) <<std::endl;
    
  }
}

bool XmlGenerator::generate(const xml::ObjectPtr& object,LITESQL_oStream& os,size_t indent)
{
  LITESQL_String indent_string(indent,' ');
  os << indent_string << LITESQL_L("<object ") 
    << attribute(LITESQL_L("name"),object->name)
    << (object->inheritsFromDefault() ? LITESQL_L(""): attribute(LITESQL_L("inherits"),object->inherits))
    << LITESQL_L(">") <<std::endl;

  for (Field::sequence::const_iterator field_it = object->fields.begin();
    field_it != object->fields.end();
    field_it++)
  {
    if (((*field_it)->name!=LITESQL_L("id")) && ((*field_it)->name!=LITESQL_L("type")))
    {
      ::generate(*field_it,os,indent+2);
    }
  }

  for (Method::sequence::const_iterator method_it = object->methods.begin();
    method_it  != object->methods.end();
    method_it++)
  {
    ::generate(*method_it,os,indent+2);
  }
  os << indent_string << endtag(LITESQL_L("object")) <<std::endl;
  return true;
}

bool XmlGenerator::generate(Relation* const relation,LITESQL_oStream& os,size_t indent)
{
  LITESQL_String indent_string(indent,' ');
  os << indent_string << LITESQL_L("<relation ") 
    << attribute(LITESQL_L("name"),relation->name)
    << attribute(LITESQL_L("id"),relation->id)
    << (relation->isUnidir() ? attribute(LITESQL_L("unidir"),LITESQL_L("true")):LITESQL_L(""));

  if (relation->related.empty())
  {
    os << LITESQL_L("/>") ;
  }
  else
  {
    os << LITESQL_L(">") << std::endl;
    for(Relate::sequence::const_iterator it = relation->related.begin();
      it != relation->related.end();
      it++)
    {
      os << indent_string << LITESQL_L("  ") 
        <<  LITESQL_L("<relate ") 
        <<  attribute(LITESQL_L("object"),(*it)->objectName)
        <<  ((*it)->hasLimit() ? attribute(LITESQL_L("limit"),toString((*it)->limit)):LITESQL_L(""))
        <<  attribute(LITESQL_L("handle"),(*it)->handle)
        <<  ((*it)->isUnique() ? attribute(LITESQL_L("unique"),LITESQL_L("true")):LITESQL_L(""))
        
        <<  LITESQL_L("/>") 
        <<std::endl;
    
    }
    os << indent_string << endtag(LITESQL_L("relation"));
  }

  os << std::endl;
  return true;
  /*<relation id=LITESQL_L("Mother") unidir=LITESQL_L("true")>
        <relate object=LITESQL_L("Person") limit=LITESQL_L("many") handle=LITESQL_L("mother")/>
        <relate object=LITESQL_L("Person") limit=LITESQL_L("one")/>
    </relation>
    <relation id=LITESQL_L("Father") unidir=LITESQL_L("true")>
        <relate object=LITESQL_L("Person") limit=LITESQL_L("many") handle=LITESQL_L("father")/>
        <relate object=LITESQL_L("Person") limit=LITESQL_L("one")/>
    </relation>
    <relation id=LITESQL_L("Siblings")>
        <relate object=LITESQL_L("Person") handle=LITESQL_L("siblings")/>
        <relate object=LITESQL_L("Person")/>
    </relation>
    <relation id=LITESQL_L("Children") unidir=LITESQL_L("true")>
        <relate object=LITESQL_L("Person") handle=LITESQL_L("children")/>
        <relate object=LITESQL_L("Person")/>
    </relation>
    <object name=LITESQL_L("Role")/>
    <object name=LITESQL_L("Student") inherits=LITESQL_L("Role")/>
    <object name=LITESQL_L("Employee") inherits=LITESQL_L("Role")/>
    <relation id=LITESQL_L("Roles") name=LITESQL_L("RoleRelation")>
        <relate object=LITESQL_L("Person") handle=LITESQL_L("roles") limit=LITESQL_L("one")/>
        <relate object=LITESQL_L("Role") handle=LITESQL_L("person")/>
    </relation>
    */

}

bool XmlGenerator::generateDatabase(LITESQL_oStream& os,const ObjectModel* model)
{
  os << LITESQL_L("<") << Database::TAG << LITESQL_L(" ") 
     << attribute(LITESQL_L("name"),model->db->name) 
     << attribute(LITESQL_L("namespace"), model->db->nspace) 
     << LITESQL_L(">") << std::endl;

  CodeGenerator::generate(model->objects,os,2);
  CodeGenerator::generate(model->relations,os,2);
  
  os << LITESQL_L("</")<<  Database::TAG <<LITESQL_L(">") << std::endl;
  return true;
}

bool XmlGenerator::generateCode(const ObjectModel* model)
{
  bool success;
  LITESQL_ofSstream ofs(getOutputFilename(m_outputFilename).c_str());
  ofs << LITESQL_L("<?xml version=\"1.0\"?>") << std::endl
      << LITESQL_L("<!DOCTYPE database SYSTEM \"litesql.dtd\">") << std::endl; 
  success = generateDatabase(ofs,model);

  ofs.close();
  return success;
}
