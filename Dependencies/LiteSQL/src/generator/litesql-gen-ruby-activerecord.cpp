#include "litesql-gen-ruby-activerecord.hpp"
#include <fstream>

using namespace xml;

const LITESQL_Char* RubyActiveRecordGenerator ::NAME = LITESQL_L("ruby-activerecord");
const LITESQL_Char* ActiveRecordClassGenerator::NAME = LITESQL_L("ruby-activerecord-class");
const LITESQL_Char* RubyMigrationsGenerator :: NAME =LITESQL_L("ruby-activerecord-migrations");

LITESQL_String toActiveRecordType(AT_field_type field_type) {
  switch(field_type) {
       case A_field_type_integer:  return LITESQL_L("int");
       case A_field_type_float:    return LITESQL_L("float");
       case A_field_type_double:    return LITESQL_L("double");
       case A_field_type_boolean:  return LITESQL_L("bool");
       case A_field_type_date:     return LITESQL_L("date");
       case A_field_type_time:     return LITESQL_L("timestamp");
       case A_field_type_datetime: return LITESQL_L("datetime");
       case A_field_type_blob:     return LITESQL_L("blob");
       case A_field_type_string:   return LITESQL_L("LITESQL_String");
       default:
         return LITESQL_L("unknown");
  }
}

RubyActiveRecordGenerator::RubyActiveRecordGenerator(): CompositeGenerator(RubyActiveRecordGenerator::NAME) 
{ 
  add(new ActiveRecordClassGenerator());
  add(new RubyMigrationsGenerator());
}

bool ActiveRecordClassGenerator::generate(const xml::ObjectPtr& object)
{
  LITESQL_String fname = getOutputFilename(toLower(object->name + LITESQL_L(".rb")));

  LITESQL_ofSstream os(fname.c_str());
  
  
  LITESQL_String baseClass = object->parentObject.get() ? object->inherits : LITESQL_L("ActiveRecord::Base"); 
  os << LITESQL_L("class ") << object->name << LITESQL_L(" < ") << baseClass << std::endl;
  
  for (RelationHandle::sequence::const_iterator it = object->handles.begin(); it!= object->handles.end(); it++) {
    os  << ((*it)->relate->hasLimit() ? LITESQL_L("has_one") : LITESQL_L("has_many")) 
        << LITESQL_L(" :") << (*it)->name;
    
    if (!(*it)->name.empty())
      os << LITESQL_L(", :through => :") << (*it)->name;
      
      
    os  << std::endl;
    
  }
  
  os << LITESQL_L("end") << std::endl;
  os.close();
  return true;
}

bool ActiveRecordClassGenerator::generateCode(const ObjectModel* model)
{
  CodeGenerator::generate(model->objects);
  return true;
}

void generateSelfUp(const ObjectModel* model,LITESQL_oStream& os)
{
  LITESQL_String indent(LITESQL_L("  "));
  os << LITESQL_L("def self.up") << std::endl;
  for (ObjectSequence::const_iterator it = model->objects.begin(); it !=model->objects.end();it++)
  {
    os << indent << LITESQL_L("create_table :") << (*it)->getTable() <<  LITESQL_L(" do |t|") << std::endl;
  
    for (Field::sequence::const_iterator fit = (*it)->fields.begin(); fit !=(*it)->fields.end();fit++)
    {
      os  << indent << indent 
          << LITESQL_L("t.") << toActiveRecordType((*fit)->type) << LITESQL_L(" :") << (*fit)->name     
          << std::endl;
    }
    os  << std::endl;
    
    os  << indent << indent << LITESQL_L("t.timestamps") << std::endl;
    os  << indent << LITESQL_L("end") << std::endl
        << std::endl;


  }
  os << LITESQL_L("end") << std::endl;
}

void generateSelfDown(const ObjectModel* model,LITESQL_oStream& os)
{
  LITESQL_String indent(LITESQL_L("  "));
  os << LITESQL_L("def self.down") << std::endl;
  for (ObjectSequence::const_iterator it = model->objects.begin(); it !=model->objects.end();it++)
  {
    os << indent << LITESQL_L("drop_table :") << (*it)->getTable() << std::endl;
  
  }
  os << LITESQL_L("end") << std::endl;
}

bool RubyMigrationsGenerator::generateCode(const ObjectModel* model)
{
  LITESQL_ofSstream os(getOutputFilename(toLower(model->db->name + LITESQL_L("_migration.rb"))).c_str());
  
  os << LITESQL_L("class ") << LITESQL_L("Create") << model->db->name << LITESQL_L("  < ActiveRecord::Migration") << std::endl;
  os << std::endl;
  generateSelfUp(model,os);
  os << std::endl;
  generateSelfDown(model,os);
  os << std::endl;
  os << LITESQL_L("end") << std::endl;

  os.close();
  return true;
}
