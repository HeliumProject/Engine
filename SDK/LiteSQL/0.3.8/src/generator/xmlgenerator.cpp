#include "litesql_char.hpp"
#include "generator.hpp"
#include "objectmodel.hpp"
#include <fstream>

using namespace xml;
using namespace litesql;

void XmlGenerator::setOutputFilename(const LiteSQL_String& filename)
{
  m_outputFilename=filename;
}

LiteSQL_String attribute(const LiteSQL_String& name, const LiteSQL_String& value)
{
  LiteSQL_String a;
  a.append(name).append( LiteSQL_L( "=" )).append( LiteSQL_L( "\"" )).append(value).append( LiteSQL_L( "\" " ));
  return a;
}

LiteSQL_String endtag(const LiteSQL_String& name)
{
  LiteSQL_String a;
  a.append( LiteSQL_L( "</" )).append(name).append( LiteSQL_L( ">" ));
  return a;
}

bool generate(LiteSQL_oStream& os,xml::Field* field, size_t indent=4)
{
  LiteSQL_String indent_string(indent,' ');
       
  os  << indent_string <<  LiteSQL_L( "<field " ) 
      << attribute( LiteSQL_L( "name" ),field->name) 
      << attribute( LiteSQL_L( "type" ),toString(field->type)) 
      << (field->default_.empty() ?  LiteSQL_L( "" ):attribute( LiteSQL_L( "default" ),field->default_)) 
      << (field->isIndexed() ? attribute( LiteSQL_L( "indexed" ), LiteSQL_L( "true" )) :  LiteSQL_L( "" )) 
      << (field->isUnique() ? attribute( LiteSQL_L( "unique" ), LiteSQL_L( "true" )) :  LiteSQL_L( "" )) 
      ;
  if (field->values.empty())
  {
    os <<  LiteSQL_L( "/>" ) <<std::endl;
  }
  else 
  {
    os <<  LiteSQL_L( ">" ) <<std::endl;
    for (std::vector<xml::Value >::const_iterator it = field->values.begin();
      it != field->values.end();
      it++)
  {
    os << indent_string <<  ' ' << ' ' <<  LiteSQL_L( "<value " ) 
      << attribute( LiteSQL_L( "name" ), it->name)
      << attribute( LiteSQL_L( "value" ), it->value)
      <<  LiteSQL_L( "/>" ) <<std::endl;
    }
    os << indent_string << endtag( LiteSQL_L( "field" )) <<std::endl;

  }
  return true;
}

void generate(LiteSQL_oStream& os,xml::Method* pMethod,size_t indent=2)
{
  LiteSQL_String indent_string(indent,' ');
  
  os << indent_string <<  LiteSQL_L( "<method " ) << attribute( LiteSQL_L( "name" ),pMethod->name );
  
  if (pMethod->params.empty())
  {
    os <<  LiteSQL_L( "/>" ) <<std::endl;
  }
  else
  {
    os <<  LiteSQL_L( ">" ) <<std::endl;
    for (std::vector<xml::Param >::const_iterator it = pMethod->params.begin();
    it != pMethod->params.end();
    it++)
    {
      os << indent_string <<  LiteSQL_L( "  " ) <<  LiteSQL_L( "<param " ) << attribute( LiteSQL_L( "type" ),(*it).type) << attribute( LiteSQL_L( "name" ),(*it).name) <<  LiteSQL_L( "/>" ) << std::endl;
    }
    os << endtag( LiteSQL_L( "method" )) <<std::endl;
    
  }
}

bool generate(LiteSQL_oStream& os,const std::vector<xml::Object* >& objects,size_t indent=2)
{
  LiteSQL_String indent_string(indent,' ');
  for (std::vector<xml::Object* >::const_iterator it = objects.begin();
    it != objects.end();
    it++)
  {
    os << indent_string <<  LiteSQL_L( "<object " ) 
       << attribute( LiteSQL_L( "name" ),(*it)->name)
       << ((*it)->inheritsFromDefault() ?  LiteSQL_L( "" ): attribute( LiteSQL_L( "inherits" ),(*it)->inherits))
       <<  LiteSQL_L( ">" ) <<std::endl;
    
    for (std::vector<xml::Field*>::const_iterator field_it = (*it)->fields.begin();
          field_it != (*it)->fields.end();
          field_it++)
    {
      generate(os,*field_it,indent+2);
    }

    for (std::vector<xml::Method*>::const_iterator method_it = (*it)->methods.begin();
          method_it  != (*it)->methods.end();
          method_it++)
    {
      generate(os,*method_it,indent+2);
    }
    os << indent_string << endtag( LiteSQL_L( "object" )) <<std::endl;
  }
  return true;
}

void generate(LiteSQL_oStream& os,Relation* relation,size_t indent=4)
{
  LiteSQL_String indent_string(indent,' ');
  os << indent_string <<  LiteSQL_L( "<relation " ) 
    << attribute( LiteSQL_L( "name" ),relation->name)
    << attribute( LiteSQL_L( "id" ),relation->id)
    << (relation->isUnidir() ? attribute( LiteSQL_L( "unidir" ), LiteSQL_L( "true" )): LiteSQL_L( "" ));

  if (relation->related.empty())
  {
    os <<  LiteSQL_L( "/>" ) ;
  }
  else
  {
    os <<  LiteSQL_L( ">" ) << std::endl;
    for(std::vector<xml::Relate*>::const_iterator it = relation->related.begin();
      it != relation->related.end();
      it++)
    {
      os << indent_string <<  LiteSQL_L( "  " ) 
        <<   LiteSQL_L( "<relate " ) 
        <<  attribute( LiteSQL_L( "object" ),(*it)->objectName)
        <<  ((*it)->hasLimit() ? attribute( LiteSQL_L( "limit" ),toString((*it)->limit)): LiteSQL_L( "" ))
        <<  attribute( LiteSQL_L( "handle" ),(*it)->handle)
        <<  ((*it)->isUnique() ? attribute( LiteSQL_L( "unique" ), LiteSQL_L( "true" )): LiteSQL_L( "" ))
        
        <<   LiteSQL_L( "/>" ) 
        <<std::endl;
    
    }
    os << indent_string << endtag( LiteSQL_L( "relation" ));
  }

  os << std::endl;
  /*<relation id= LiteSQL_L( "Mother" ) unidir= LiteSQL_L( "true" )>
        <relate object= LiteSQL_L( "Person" ) limit= LiteSQL_L( "many" ) handle= LiteSQL_L( "mother" )/>
        <relate object= LiteSQL_L( "Person" ) limit= LiteSQL_L( "one" )/>
    </relation>
    <relation id= LiteSQL_L( "Father" ) unidir= LiteSQL_L( "true" )>
        <relate object= LiteSQL_L( "Person" ) limit= LiteSQL_L( "many" ) handle= LiteSQL_L( "father" )/>
        <relate object= LiteSQL_L( "Person" ) limit= LiteSQL_L( "one" )/>
    </relation>
    <relation id= LiteSQL_L( "Siblings" )>
        <relate object= LiteSQL_L( "Person" ) handle= LiteSQL_L( "siblings" )/>
        <relate object= LiteSQL_L( "Person" )/>
    </relation>
    <relation id= LiteSQL_L( "Children" ) unidir= LiteSQL_L( "true" )>
        <relate object= LiteSQL_L( "Person" ) handle= LiteSQL_L( "children" )/>
        <relate object= LiteSQL_L( "Person" )/>
    </relation>
    <object name= LiteSQL_L( "Role" )/>
    <object name= LiteSQL_L( "Student" ) inherits= LiteSQL_L( "Role" )/>
    <object name= LiteSQL_L( "Employee" ) inherits= LiteSQL_L( "Role" )/>
    <relation id= LiteSQL_L( "Roles" ) name= LiteSQL_L( "RoleRelation" )>
        <relate object= LiteSQL_L( "Person" ) handle= LiteSQL_L( "roles" ) limit= LiteSQL_L( "one" )/>
        <relate object= LiteSQL_L( "Role" ) handle= LiteSQL_L( "person" )/>
    </relation>
    */

}

bool generateDatabase(LiteSQL_oStream& os,const ObjectModel* model)
{
  os <<  LiteSQL_L( "<" ) << Database::TAG <<  LiteSQL_L( " " ) 
     << attribute( LiteSQL_L( "name" ),model->db.name) 
     << attribute( LiteSQL_L( "namespace" ), model->db.nspace) 
     <<  LiteSQL_L( ">" ) << std::endl;

  generate(os,model->objects);
  
  for ( std::vector<xml::Relation*>::const_iterator it = model->relations.begin();
    it != model->relations.end();
    it++)
  {
    generate(os,*it,2);
  }
  
  os <<  LiteSQL_L( "</" )<<  Database::TAG << LiteSQL_L( ">" ) << std::endl;
  return true;
}

bool XmlGenerator::generateCode(const ObjectModel* model)
{
  bool success;
  LiteSQL_ofSstream ofs(m_outputFilename.c_str());
  ofs <<  LiteSQL_L( "<?xml version=\"1.0\"?>" ) << std::endl
      <<  LiteSQL_L( "<!DOCTYPE database SYSTEM \"litesql.dtd\">" ) << std::endl; 
  success = generateDatabase(ofs,model);

  ofs.close();
  return success;
}
