#ifndef objectmodel_hpp
#define objectmodel_hpp

#include "xmlobjects.hpp"

namespace litesql {
/** the litesql object model */ 
class ObjectModel {
public:
   ObjectModel();
   virtual ~ObjectModel();
   /** load model from xml-file (uses a  specialized XmlParser) */
   bool loadFromFile(const LITESQL_String& filename);

   bool remove(xml::Field::counted_ptr& field);
  bool remove(xml::Method::counted_ptr& method);
   bool remove(xml::ObjectPtr& object);

   xml::Relation::sequence relations;
   xml::ObjectSequence objects;
   
   xml::DatabasePtr db;   
};

const LITESQL_Char* toString(AT_field_type t);
AT_field_type field_type(const LITESQL_Char* value);

}
#endif //#ifndef objectmodel_hpp
