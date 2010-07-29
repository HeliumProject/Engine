#ifndef objectmodel_hpp
#define objectmodel_hpp
#include "litesql_char.hpp"
#include <string>
#include <vector>

#include "xmlobjects.hpp"

namespace litesql {
/** the litesql object model */ 
class ObjectModel {
public:
   virtual ~ObjectModel();
   /** load model from xml-file (uses a  specialized XmlParser) */
   bool loadFromFile(const LITESQL_String& filename);

   bool remove(xml::Field* field);
   bool remove(xml::Object* object);
//   bool remove(xml::relation* relation);

   xml::Database db;   

   std::vector<xml::Object* > objects;
   std::vector<xml::Relation* > relations;
};

const LITESQL_Char* toString(AT_field_type t);
AT_field_type field_type(const LITESQL_Char* value);

}
#endif //#ifndef objectmodel_hpp
