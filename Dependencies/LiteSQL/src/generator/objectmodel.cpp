// include LiteSQL's header file and generated header file
#include "litesql_char.hpp"
#include "xmlparser.hpp"
#include "objectmodel.hpp"
#include <string.h>
#include "logger.hpp"

using namespace xml;

#define xmlStrcasecmp(s1,s2)  ((s1==NULL) ? (s2!=NULL) : _tcscmp(s1,s2))
#define xmlStrEqual(s1,s2)   (!_tcscmp(s1,s2))

const LITESQL_Char* litesql::toString(AT_field_type t)
{
  switch (t) 
  {
  case A_field_type_boolean:
    return  LITESQL_L("boolean");
  case A_field_type_integer:
    return  LITESQL_L("integer");
  case A_field_type_string:
    return  LITESQL_L("LITESQL_String");
  case A_field_type_float: 
    return  LITESQL_L("float");
  case A_field_type_double: 
    return  LITESQL_L("double");
  case A_field_type_time:
    return  LITESQL_L("time");
  case A_field_type_date:
    return  LITESQL_L("date");
  case A_field_type_datetime:
    return  LITESQL_L("datetime");
  case A_field_type_blob: 
    return  LITESQL_L("blob");

  default: 
    return  LITESQL_L("unknown");   
  }
}

AT_field_type litesql::field_type(const LITESQL_Char* value)
{
  AT_field_type t;

  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("boolean")))
  {
    t = A_field_type_boolean;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("integer")))
  {
    t = A_field_type_integer;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("LITESQL_String")))
  {
    t = A_field_type_string;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("float")))
  {
    t = A_field_type_float;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("double")))
  {
    t = A_field_type_double;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("time")))
  {
    t = A_field_type_time;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("date")))
  {
    t = A_field_type_date;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("datetime")))
  {
    t = A_field_type_datetime;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("blob")))
  {
    t = A_field_type_blob;
  }
  else
  {
    t = AU_field_type;   
  }
  return t;
}

static const LITESQL_Char* toString(AT_relate_limit t)
{
  switch (t) 
  {
  case A_relate_limit_one:
    return  LITESQL_L("one");
  case A_relate_limit_many:
    return  LITESQL_L("many");
  case AU_relate_limit:
  default:
    return  LITESQL_L("unknown");   
  }
}


static AT_field_unique field_unique(const XML_Char* value)
{
  AT_field_unique t;
  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("true")))
  {
    t = A_field_unique_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("false")))
  {
    t = A_field_unique_false;
  }
  else
  {
    t = AU_field_unique;
  }
  return t;
}

static AT_index_unique index_unique(const XML_Char* value)
{
  AT_index_unique t;
  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("true")))
  {
    t = A_index_unique_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("false")))
  {
    t = A_index_unique_false;
  }
  else
  {
    t = AU_index_unique;
  }
  return t;
}


static AT_field_indexed field_indexed(const XML_Char* value)
{
  AT_field_indexed t;
  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("true")))
  {
    t = A_field_indexed_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("false")))
  {
    t = A_field_indexed_false;
  }
  else
  {
    t = AU_field_indexed;
  }
  return t;
}

static AT_relation_unidir relation_unidir(const XML_Char* value)
{
  AT_relation_unidir t;
  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("true")))
  {
    t = A_relation_unidir_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("false")))
  {
    t = A_relation_unidir_false;
  }
  else
  {
    t = AU_relation_unidir;
  }
  return t;
}

static AT_relate_unique relate_unique(const XML_Char* value)
{
  AT_relate_unique t;
  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("true")))
  {
    t = A_relate_unique_true;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("false")))
  {
    t = A_relate_unique_false;
  }
  else
  {
    t = AU_relate_unique;
  }
  return t;
}

static AT_relate_limit relate_limit(const XML_Char* value)
{
  AT_relate_limit t;
  if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("one")))
  {
    t = A_relate_limit_one;
  }
  else if (!xmlStrcasecmp(value,(XML_Char*) LITESQL_L("many")))
  {
    t = A_relate_limit_many;
  }
  else
  {
    t = A_relate_limit_many;
  }
  return t;

}

namespace xml {
class LitesqlParser : public XmlParser {
public:
  LitesqlParser(ObjectModel* model)
    : m_pObjectModel(model),
      m_parseState(ROOT) {};

protected:
  void onStartElement(const XML_Char *fullname,
    const XML_Char **atts);

  void onEndElement(const XML_Char *fullname);
  /** ROOT->DATABASE; 
  *
  *    DATABASE->OBJECT;
  *      OBJECT->FIELD;
  *      OBJECT->METHOD;
  *      FIELD->OBJECT;
  *      METHOD->OBJECT;
  *
  *    DATABASE->RELATION;
  *    RELATION->DATABASE;
  *
  *  DATABASE->ROOT;
  * ERROR;
  */
  enum ParseState { ROOT,
    DATABASE,
    OBJECT,
    FIELD,
    METHOD,
    RELATION,
    INDEX,
    INDEXFIELD,
    INCLUDE,
    UNKNOWN,
    ERROR
  };

private:
  ObjectModel* m_pObjectModel;

  Object * obj;
  Relation * rel;
  Field * fld;
  Field * rel_fld;
  Method * mtd;
  Index * idx;
  IndexField* idxField;

  ParseState m_parseState;
  std::vector<ParseState> history;
};

}

void LitesqlParser::onStartElement(const XML_Char *fullname,
                                   const XML_Char **atts)
{
  //   Logger::report(LITESQL_L("starting ") <<fullname);
  history.push_back(m_parseState);

  if (xmlStrEqual(fullname,(XML_Char*) Database::TAG))
  {
    if (m_parseState!=ROOT)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_parseState = DATABASE;
      m_pObjectModel->db.name = safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")));
      m_pObjectModel->db.include = safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("include")));
      m_pObjectModel->db.nspace = safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("namespace")));
      Logger::report(LITESQL_L("database = ") + m_pObjectModel->db.name);
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Object::TAG))
  {
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_pObjectModel->objects.push_back(obj = new Object(   (LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")), 
        safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("inherits")))));
      Logger::report(LITESQL_L("object = ") + obj->name);
      m_parseState = OBJECT; 

    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Field::TAG))
  {
    Field* pNewField = new Field(  (LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")), 
                                    field_type(xmlGetAttrValue(atts, LITESQL_L("type"))),
                                    safe( (LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("default"))),
                                    field_indexed(xmlGetAttrValue(atts, LITESQL_L("indexed"))),
                                    field_unique(xmlGetAttrValue(atts, LITESQL_L("unique")))
     );

    switch(m_parseState)
    {
    case OBJECT:
      if (!obj) {
        Logger::error(LITESQL_L("parsing field inside object, but currentObject == NULL "));
      }
      else {
        Logger::report(LITESQL_L("field = "),(LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")));
        obj->fields.push_back(fld = pNewField);
      };
      m_parseState = FIELD;
      break;

    case RELATION:
      if (!rel) {
        Logger::error(LITESQL_L("parsing field inside realtion, but currentRelation == NULL "));
      }
      else
      {
        Logger::report(LITESQL_L("field = "),(LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")));
        rel->fields.push_back(rel_fld = pNewField);
      }
      m_parseState = FIELD;
      break;

    default:
      delete pNewField;
      m_parseState = ERROR;
    }
  }
  else if (xmlStrEqual(fullname,(XML_Char*)Index::TAG))
  {
    Index* pIdx = new Index(index_unique(xmlGetAttrValue(atts, LITESQL_L("unique"))));
    
    switch (m_parseState)
    {
    case OBJECT:
      idx = pIdx;
      obj->indices.push_back(idx);
      m_parseState = INDEX;
      break;

    case RELATION:
      idx = pIdx;
      rel->indices.push_back(idx);
      m_parseState = INDEX;
      break;

    default:
      delete pIdx;
      m_parseState = ERROR;
    }
  }
  else if (xmlStrEqual(fullname,(XML_Char*)IndexField::TAG))
  {
    if (m_parseState!=INDEX)
    {
      m_parseState = ERROR;
    }
    else
    {
      IndexField idxField((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")));
      idx->fields.push_back(idxField);
    }    
    
    m_parseState = INDEXFIELD;
  }
  else if (xmlStrEqual(fullname,(XML_Char*)Value::TAG))
  {
    if (m_parseState!=FIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      if (fld) 
      {
        fld->value(Value((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")), (LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("value"))));
      }
      Logger::report(LITESQL_L("value = "));
    }

  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Method::TAG))
  {
    if (m_parseState!=OBJECT)
    {
      m_parseState = ERROR;
    }
    else
    {
      obj->methods.push_back(
        mtd = new Method(safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name"))), 
        safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("returntype"))) 
       )
       );
      m_parseState= METHOD;
      Logger::report(LITESQL_L("method = "));
    }

  }
  else if (xmlStrEqual(fullname,(XML_Char*)Param::TAG))
  {
    if (m_parseState!=METHOD)
    {
      m_parseState = ERROR;
    }
    else
    {
        std::string tempString;
        LITESQL_ConvertString((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("type")), tempString);
        mtd->param(Param((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")),tempString.c_str()));
    }
  }
  else if (xmlStrEqual(fullname,(XML_Char*)Relation::TAG))
  {
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_pObjectModel->relations.push_back(rel = new Relation( safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("id"))), 
        safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name"))),
        relation_unidir(xmlGetAttrValue(atts, LITESQL_L("unidir")))));

      Logger::report( LITESQL_L("relation = ") + rel->getName());

      m_parseState = RELATION;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Relate::TAG))
  {
    if (m_parseState!=RELATION)
    {
      m_parseState = ERROR;
    }
    else
    {
      rel->related.push_back(
        new Relate(safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("object"))), 
        relate_limit(xmlGetAttrValue(atts, LITESQL_L("limit"))), 
        relate_unique(xmlGetAttrValue(atts, LITESQL_L("unique"))), 
        safe((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("handle")))));
      Logger::report(LITESQL_L("relate = "));
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*) LITESQL_L("include")))
  {
    LITESQL_String filename((LITESQL_Char*)xmlGetAttrValue(atts, LITESQL_L("name")));
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("include \"") + filename + LITESQL_L('"'));
      ObjectModel includedModel;
      if (!includedModel.loadFromFile(filename)) 
      {
        Logger::report(LITESQL_L("error on parsing included file \"") + filename + LITESQL_L('"'));
      }
      m_pObjectModel->objects.insert(m_pObjectModel->objects.end(),includedModel.objects.begin(),includedModel.objects.end());
      m_pObjectModel->relations.insert(m_pObjectModel->relations.end(),includedModel.relations.begin(),includedModel.relations.end());
      m_parseState = INCLUDE;
    }
  } 
  else
  {
    m_parseState = UNKNOWN;
    Logger::error(LITESQL_L("unknown = "),fullname);
  } 
}

void LitesqlParser::onEndElement(const XML_Char *fullname)
{
  //  Logger::report(LITESQL_L("ending "),fullname); 
  if (xmlStrEqual(fullname,(XML_Char*)Database::TAG))
  {
    if (m_parseState!=DATABASE)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end database "));
      m_parseState = ROOT;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Object::TAG))
  {
    if (m_parseState!=OBJECT)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end object "));
      obj = NULL;
      fld= NULL;
      rel = NULL;
      rel_fld = NULL;
      m_parseState = DATABASE;
    }
  } 
  else if (xmlStrEqual(fullname,xml::Field::TAG))
  {
    if (m_parseState!=FIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end field"));
      fld = NULL;
      m_parseState = history.back();
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Index::TAG))
  {
    if (m_parseState!=INDEX)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end index"));
      idx = NULL;
      m_parseState = history.back();
    }
  }
  else if (xmlStrEqual(fullname,(XML_Char*)IndexField::TAG))
  {
    if (m_parseState!=INDEXFIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end indexfield"));
      m_parseState = history.back();
    }
  }
  else if (xmlStrEqual(fullname,(XML_Char*)Value::TAG))
  {
    if (m_parseState!=FIELD)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end value"));
    }

  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Method::TAG))
  {
    if (m_parseState!=METHOD)
    {
      m_parseState = ERROR;
    }
    else
    {
      m_parseState = OBJECT;
      Logger::report(LITESQL_L("end method"));
    }

  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Param::TAG))
  {
    if (m_parseState!=METHOD)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end param"));
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Relation::TAG))
  {
    if (m_parseState!=RELATION)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end relation "));
      rel = NULL;
      rel_fld = NULL;
      m_parseState = DATABASE;
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*)Relate::TAG))
  {
    if (m_parseState!=RELATION)
    {
      m_parseState = ERROR;
    }
    else
    {
      rel_fld = NULL;
      Logger::report(LITESQL_L("end relate"));
    }
  } 
  else if (xmlStrEqual(fullname,(XML_Char*) LITESQL_L("include")))
  {
    if (m_parseState!=INCLUDE)
    {
      m_parseState = ERROR;
    }
    else
    {
      Logger::report(LITESQL_L("end include "));
      m_parseState = DATABASE;
    }
  } 
  else 
  {
    m_parseState = history.back();
    Logger::error( LITESQL_L("end unknown "));
  }

  history.pop_back();
}

ObjectModel::~ObjectModel()
{
/*
for (std::vector<xml::Object* >::iterator it=objects.begin();
        it !=objects.end();it++)
  {
    delete *it;
  }

  for (std::vector<xml::Relation* >::iterator it=relations.begin();
        it !=relations.end();it++)
  {
    delete *it;
  }
*/
}

bool ObjectModel::loadFromFile(const LITESQL_String& filename)
{
  LitesqlParser parser(this);

  bool successfulParsed = parser.parseFile(filename);
  if (successfulParsed)
  {
    xml::init(db,objects,relations);
  }
  return successfulParsed;
}

bool ObjectModel::remove(xml::Field* field)
{
  if (field!=NULL)
  {  
    std::vector<Field*>::iterator found;
    for (std::vector<xml::Object* >::iterator it=objects.begin();
          it !=objects.end();
          it++)
    {
      found = find((*it)->fields.begin(),(*it)->fields.end(),field);
      if (found!=(*it)->fields.end()) 
      {
        (*it)->fields.erase(found);
        return true;
      }
    }
  }
  return false;
}

bool ObjectModel::remove(xml::Object* object)
{
  if (object!=NULL)
  {  
    std::vector<Object*>::iterator found = find(objects.begin(),objects.end(),object);
    if (found!=objects.end()) 
    {
      objects.erase(found);
      return true;
    }
  }
  return false;
}
