#include "litesql_char.hpp"
#include "generator.hpp"
#include "objectmodel.hpp"
#include <fstream>

using namespace xml;
using namespace litesql;

void XmlGenerator::setOutputFilename(const LITESQL_String& filename)
{
  m_outputFilename=filename;
}

LITESQL_String attribute(const LITESQL_String& name, const LITESQL_String& value)
{
  LITESQL_String a;
  a.append(name).append( LITESQL_L( "=" )).append( LITESQL_L( "\"" )).append(value).append( LITESQL_L( "\" " ));
  return a;
}

LITESQL_String endtag(const LITESQL_String& name)
{
  LITESQL_String a;
  a.append( LITESQL_L( "</" )).append(name).append( LITESQL_L( ">" ));
  return a;
}

bool generate(LITESQL_oStream& os,xml::Field* field, size_t indent=4)
{
  LITESQL_String indent_string(indent,' ');
       
  os  << indent_string <<  LITESQL_L( "<field " ) 
      << attribute( LITESQL_L( "name" ),field->name) 
      << attribute( LITESQL_L( "type" ),toString(field->type)) 
      << (field->default_.empty() ?  LITESQL_L( "" ):attribute( LITESQL_L( "default" ),field->default_)) 
      << (field->isIndexed() ? attribute( LITESQL_L( "indexed" ), LITESQL_L( "true" )) :  LITESQL_L( "" )) 
      << (field->isUnique() ? attribute( LITESQL_L( "unique" ), LITESQL_L( "true" )) :  LITESQL_L( "" )) 
      ;
  if (field->values.empty())
  {
    os <<  LITESQL_L( "/>" ) <<std::endl;
  }
  else 
  {
    os <<  LITESQL_L( ">" ) <<std::endl;
    for (std::vector<xml::Value >::const_iterator it = field->values.begin();
      it != field->values.end();
      it++)
  {
    os << indent_string <<  ' ' << ' ' <<  LITESQL_L( "<value " ) 
      << attribute( LITESQL_L( "name" ), it->name)
      << attribute( LITESQL_L( "value" ), it->value)
      <<  LITESQL_L( "/>" ) <<std::endl;
    }
    os << indent_string << endtag( LITESQL_L( "field" )) <<std::endl;

  }
  return true;
}

void generate(LITESQL_oStream& os,xml::Method* pMethod,size_t indent=2)
{
  LITESQL_String indent_string(indent,' ');
  
  os << indent_string <<  LITESQL_L( "<method " ) << attribute( LITESQL_L( "name" ),pMethod->name );
  
  if (pMethod->params.empty())
  {
    os <<  LITESQL_L( "/>" ) <<std::endl;
  }
  else
  {
    os <<  LITESQL_L( ">" ) <<std::endl;
    for (std::vector<xml::Param >::const_iterator it = pMethod->params.begin();
    it != pMethod->params.end();
    it++)
    {
      os << indent_string <<  LITESQL_L( "  " ) <<  LITESQL_L( "<param " ) << attribute( LITESQL_L( "type" ),(*it).type) << attribute( LITESQL_L( "name" ),(*it).name) <<  LITESQL_L( "/>" ) << std::endl;
    }
    os << endtag( LITESQL_L( "method" )) <<std::endl;
    
  }
}

bool generate(LITESQL_oStream& os,const std::vector<xml::Object* >& objects,size_t indent=2)
{
  LITESQL_String indent_string(indent,' ');
  for (std::vector<xml::Object* >::const_iterator it = objects.begin();
    it != objects.end();
    it++)
  {
    os << indent_string <<  LITESQL_L( "<object " ) 
       << attribute( LITESQL_L( "name" ),(*it)->name)
       << ((*it)->inheritsFromDefault() ?  LITESQL_L( "" ): attribute( LITESQL_L( "inherits" ),(*it)->inherits))
       <<  LITESQL_L( ">" ) <<std::endl;
    
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
    os << indent_string << endtag( LITESQL_L( "object" )) <<std::endl;
  }
  return true;
}

void generate(LITESQL_oStream& os,Relation* relation,size_t indent=4)
{
  LITESQL_String indent_string(indent,' ');
  os << indent_string <<  LITESQL_L( "<relation " ) 
    << attribute( LITESQL_L( "name" ),relation->name)
    << attribute( LITESQL_L( "id" ),relation->id)
    << (relation->isUnidir() ? attribute( LITESQL_L( "unidir" ), LITESQL_L( "true" )): LITESQL_L( "" ));

  if (relation->related.empty())
  {
    os <<  LITESQL_L( "/>" ) ;
  }
  else
  {
    os <<  LITESQL_L( ">" ) << std::endl;
    for(std::vector<xml::Relate*>::const_iterator it = relation->related.begin();
      it != relation->related.end();
      it++)
    {
      os << indent_string <<  LITESQL_L( "  " ) 
        <<   LITESQL_L( "<relate " ) 
        <<  attribute( LITESQL_L( "object" ),(*it)->objectName)
        <<  ((*it)->hasLimit() ? attribute( LITESQL_L( "limit" ),toString((*it)->limit)): LITESQL_L( "" ))
        <<  attribute( LITESQL_L( "handle" ),(*it)->handle)
        <<  ((*it)->isUnique() ? attribute( LITESQL_L( "unique" ), LITESQL_L( "true" )): LITESQL_L( "" ))
        
        <<   LITESQL_L( "/>" ) 
        <<std::endl;
    
    }
    os << indent_string << endtag( LITESQL_L( "relation" ));
  }

  os << std::endl;
  /*<relation id= LITESQL_L( "Mother" ) unidir= LITESQL_L( "true" )>
        <relate object= LITESQL_L( "Person" ) limit= LITESQL_L( "many" ) handle= LITESQL_L( "mother" )/>
        <relate object= LITESQL_L( "Person" ) limit= LITESQL_L( "one" )/>
    </relation>
    <relation id= LITESQL_L( "Father" ) unidir= LITESQL_L( "true" )>
        <relate object= LITESQL_L( "Person" ) limit= LITESQL_L( "many" ) handle= LITESQL_L( "father" )/>
        <relate object= LITESQL_L( "Person" ) limit= LITESQL_L( "one" )/>
    </relation>
    <relation id= LITESQL_L( "Siblings" )>
        <relate object= LITESQL_L( "Person" ) handle= LITESQL_L( "siblings" )/>
        <relate object= LITESQL_L( "Person" )/>
    </relation>
    <relation id= LITESQL_L( "Children" ) unidir= LITESQL_L( "true" )>
        <relate object= LITESQL_L( "Person" ) handle= LITESQL_L( "children" )/>
        <relate object= LITESQL_L( "Person" )/>
    </relation>
    <object name= LITESQL_L( "Role" )/>
    <object name= LITESQL_L( "Student" ) inherits= LITESQL_L( "Role" )/>
    <object name= LITESQL_L( "Employee" ) inherits= LITESQL_L( "Role" )/>
    <relation id= LITESQL_L( "Roles" ) name= LITESQL_L( "RoleRelation" )>
        <relate object= LITESQL_L( "Person" ) handle= LITESQL_L( "roles" ) limit= LITESQL_L( "one" )/>
        <relate object= LITESQL_L( "Role" ) handle= LITESQL_L( "person" )/>
    </relation>
    */

}

bool generateDatabase(LITESQL_oStream& os,const ObjectModel* model)
{
  os <<  LITESQL_L( "<" ) << Database::TAG <<  LITESQL_L( " " ) 
     << attribute( LITESQL_L( "name" ),model->db.name) 
     << attribute( LITESQL_L( "namespace" ), model->db.nspace) 
     <<  LITESQL_L( ">" ) << std::endl;

  generate(os,model->objects);
  
  for ( std::vector<xml::Relation*>::const_iterator it = model->relations.begin();
    it != model->relations.end();
    it++)
  {
    generate(os,*it,2);
  }
  
  os <<  LITESQL_L( "</" )<<  Database::TAG << LITESQL_L( ">" ) << std::endl;
  return true;
}

bool XmlGenerator::generateCode(const ObjectModel* model)
{
  bool success;
  LITESQL_ofSstream ofs(m_outputFilename.c_str());
  ofs <<  LITESQL_L( "<?xml version=\"1.0\"?>" ) << std::endl
      <<  LITESQL_L( "<!DOCTYPE database SYSTEM \"litesql.dtd\">" ) << std::endl; 
  success = generateDatabase(ofs,model);

  ofs.close();
  return success;
}
