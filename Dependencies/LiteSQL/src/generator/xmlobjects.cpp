#include "litesql_char.hpp"
#include "xmlobjects.hpp"
#include "md5.hpp"
#include "litesql.hpp"
#include "logger.hpp"

#include <algorithm>

namespace std {
    template <>
    struct less<xml::Relate*> {
        bool operator()(xml::Relate const* r1, xml::Relate const* r2) {
            if (!r1)
                return true;
            if (!r2)
                return false;
            return *r1 < *r2;
        }
    };
}
namespace xml {

const LITESQL_Char* Database::TAG= LITESQL_L("database");
const LITESQL_Char* Object::TAG= LITESQL_L("object");
const LITESQL_Char* Field::TAG= LITESQL_L("field");
const LITESQL_Char* Relation::TAG= LITESQL_L("relation");
const LITESQL_Char* Relate::TAG= LITESQL_L("relate");
const LITESQL_Char* Value::TAG= LITESQL_L("value");
const LITESQL_Char* IndexField::TAG= LITESQL_L("indexfield");
const LITESQL_Char* Index::TAG= LITESQL_L("index");
const LITESQL_Char* Param::TAG= LITESQL_L("param");
const LITESQL_Char* Method::TAG= LITESQL_L("method");

const Object Object::DEFAULT_BASE(LITESQL_L("litesql::Persistent"), LITESQL_L(""));
    
LITESQL_String validID(const LITESQL_String& s, const LITESQL_String& type= LITESQL_L("field")) {
  LITESQL_String result= LITESQL_L("");  
  
  if (s.empty()) 
    result =  LITESQL_L("empty identifier");
  else if (toupper(s[0]) == s[0] && type ==  LITESQL_L("field"))
    result =   LITESQL_L("does not begin with lower case letter");
  else if (!isalpha(s[0])) 
    result =  LITESQL_L("first character is not alphabet");
  else if (s[s.size()-1] == '_')
        result =  LITESQL_L("ends with an underscore");
  else
  {
    for (size_t i = 1; i < s.size(); i++)
    {
      if (!isalnum(s[i]) && !(s[i] != '_')) 
      {
        result =  LITESQL_L("illegal character : ");
        result += s[i];
        break;
      }
    }
  }
  return result;
}

LITESQL_String makeDBName(const LITESQL_String& s) {
    if (s.size() > 31)
    {
        std::string tempSTDString;
        LITESQL_ConvertString(s, tempSTDString);

        LITESQL_String tempString;
        LITESQL_ConvertString(md5HexDigest(tempSTDString), tempString);
        return LITESQL_L("_") + tempString;
    }
    return s;
}
static void sanityCheck(Database& db,
                        const std::vector<Object*>& objects,
                        const std::vector<Relation*>& relations) {
    using namespace litesql;
    std::map<LITESQL_String, bool> usedID;
    std::map<LITESQL_String, bool> objectName;
    LITESQL_String err;
    if (!(err = validID(db.name, LITESQL_L("class"))).empty()) 
        throw Except(LITESQL_L("invalid id: database.name : ") + db.name);
    for (size_t i = 0; i < objects.size(); i++) {
        Object& o = *objects[i];
        if (!(err = validID(o.name,  LITESQL_L("class"))).empty())
            throw Except(LITESQL_L("invalid id: object.name : ") + o.name);
        if (usedID.find(o.name) != usedID.end())
            throw Except(LITESQL_L("duplicate id: object.name : ") + o.name);
        usedID[o.name] = true;
        objectName[o.name] = true;
        std::map<LITESQL_String, bool> usedField;
        usedField.clear();
        for (size_t i2 = 0; i2 < o.fields.size(); i2++) {
            Field& f = *o.fields[i2];
            if (!(err = validID(f.name)).empty())
              throw Except(LITESQL_L("invalid id: object.field.name : ") + o.name +  LITESQL_L(".") + f.name +  LITESQL_L(" cause:") + err);
            if (usedField.find(f.name) != usedField.end())
                throw Except(LITESQL_L("duplicate id: object.field.name : ") + o.name +  LITESQL_L(".") + f.name);
            usedField[f.name] = true;
        }
    }
    for (size_t i = 0; i < relations.size(); i++) {
        Relation& r = *relations[i];
        LITESQL_String name = r.getName();
        if (!(err = validID(name, LITESQL_L("class"))).empty())
            throw Except(LITESQL_L("invalid id: relation.name : ") + name +  LITESQL_L("cause:") + err);
        if (usedID.find(name) != usedID.end())
            throw Except(LITESQL_L("duplicate id: relation.name : ") + name);
        usedID[name] = true;
        std::map<LITESQL_String, bool> usedField;
        usedField.clear();
  
        bool defaults = false;
        
        for (size_t i2 = 0; i2 < r.fields.size(); i2++) {
            Field& f = *r.fields[i2];
            if (!(err = validID(f.name)).empty())
                throw Except(LITESQL_L("invalid id: relation.field.name : ") + name +  LITESQL_L(".") + f.name +  LITESQL_L("cause:") + err);
            if (usedField.find(f.name) != usedField.end())
                throw Except(LITESQL_L("duplicate id: relation.field.name : ") + name +  LITESQL_L(".") + f.name);
            usedField[f.name] = true;
            if (f.default_.size() > 0)
                defaults = true;
            else if (defaults)
                throw Except(LITESQL_L("no default-value after field with default value : ") + name +  LITESQL_L(".") + f.name);

        }
        usedField.clear();
        bool limits = false;
        bool uniques = false;
        for (size_t i2 = 0; i2 < r.related.size(); i2++) {
            Relate& rel = *r.related[i2];
            if (!(err = validID(rel.handle)).empty() && !rel.handle.empty())
                throw Except(LITESQL_L("invalid id: relation.relate.handle : ") + name +  LITESQL_L(".") + rel.handle +  LITESQL_L("cause:") + err);
            if (usedField.find(rel.handle) != usedField.end())
                throw Except(LITESQL_L("duplicate id: relation.relate.handle : ") + name +  LITESQL_L(".") + rel.handle);
            if (objectName.find(rel.objectName) == objectName.end())
                throw Except(LITESQL_L("unknown object: relation.relate.name : ") + name +  LITESQL_L(".") + rel.objectName);
            if (!rel.handle.empty())
                usedField[rel.handle] = true;
            if (rel.isUnique())
                uniques = true;
            if (rel.hasLimit())
                limits = true;
            if (uniques && limits)
                throw Except(LITESQL_L("both 'unique' and 'limit' attributes used in relation ") + name);
        }
        if (r.related.size() != 2 && limits)
            throw Except(LITESQL_L("'limit' attribute used in relation of ") + toString(r.related.size()) 
                         +  LITESQL_L(" object(s) ") + name);
            
    }   
}

static void initSchema(Database& db,
                std::vector<Object*>& objects,
                std::vector<Relation*>& relations) {
    for (size_t i = 0; i < objects.size(); i++) {
        Object& o = *objects[i];
        std::map<LITESQL_String, Database::DBField*> fldMap;
        Database::Table* tbl = new Database::Table;
        tbl->name = o.getTable();
        db.tables.push_back(tbl);
        
        if (!o.parentObject) {
            Database::Sequence* seq = new Database::Sequence;
            seq->name = o.getSequence();
            seq->table = o.getTable();
            db.sequences.push_back(seq);
        }  else {
            Database::DBField *id = new Database::DBField; 
            id->name =  LITESQL_L("id_");
            id->type =  LITESQL_L("INTEGER");
            id->primaryKey = true;
            tbl->fields.push_back(id);
        }

        for (size_t i2 = 0; i2 < o.fields.size(); i2++) {
            Field& f = *o.fields[i2];
            Database::DBField* fld = new Database::DBField;
            fld->name = f.name +  LITESQL_L("_");
            fldMap[f.name] = fld;
            fld->type = f.getSQLType();
            fld->primaryKey = (f.name ==  LITESQL_L("id"));
            if (f.isUnique())
                fld->extra =  LITESQL_L(" UNIQUE");
            fld->field = o.fields[i2];
            tbl->fields.push_back(fld);
            
            if (f.isIndexed()) {
                Database::DBIndex* idx = new Database::DBIndex;
                idx->name = makeDBName(tbl->name + fld->name +  LITESQL_L("idx"));
                idx->table = tbl->name;
                idx->fields.push_back(fld);
                db.indices.push_back(idx);
            }
                
        }
        for (size_t i2 = 0; i2 < o.indices.size(); i2++) {
            const Index& idx = *o.indices[i2];

            litesql::Split fldNames;
            Database::DBIndex* index = new Database::DBIndex;
            for (size_t i3 = 0; i3 < idx.fields.size(); i3++) {
                if (fldMap.find(idx.fields[i3].name) == fldMap.end())
                    throw litesql::Except(LITESQL_L("Indexfield ") + o.name +  LITESQL_L(".") + idx.fields[i3].name +  LITESQL_L(" is invalid."));
                index->fields.push_back(fldMap[idx.fields[i3].name]);
                fldNames.push_back(idx.fields[i3].name);
            }

            index->name = makeDBName(tbl->name +  LITESQL_L("_") + fldNames.join(LITESQL_L("_")) +  LITESQL_L("_idx"));
            index->table = tbl->name;
            
            LITESQL_String unique =  LITESQL_L("");
            if (idx.isUnique())
                index->unique = true;
            db.indices.push_back(index);
        }
    }
    for (size_t i = 0; i < relations.size(); i++) {
        Relation& r = *relations[i];
        Database::Table* tbl = new Database::Table;
        db.tables.push_back(tbl);
        tbl->name = r.getTable();
        std::vector<Database::DBField*> objFields;
        std::map<LITESQL_String, Database::DBField*> fldMap;
        for (size_t i2 = 0; i2 < r.related.size(); i2++) {
            const xml::Relate& relate = *r.related[i2];
            LITESQL_String extra;
            if (relate.isUnique())
                    extra =  LITESQL_L(" UNIQUE");
            else if (r.related.size() == 2) { 
                if (i2 == 0 && r.related[1]->hasLimit())
                    extra =  LITESQL_L(" UNIQUE");
                if (i2 == 1 && r.related[0]->hasLimit())
                    extra =  LITESQL_L(" UNIQUE");
            }
            Database::DBField* fld = new Database::DBField;
            fld->name = relate.fieldName;
            fld->type =  LITESQL_L("INTEGER");
            fld->extra = extra;
            tbl->fields.push_back(fld);
            objFields.push_back(fld);
            
            Database::DBIndex* idx = new Database::DBIndex;
            idx->name = makeDBName(tbl->name + fld->name +  LITESQL_L("idx"));
            idx->table = tbl->name;
            idx->fields.push_back(fld);
            db.indices.push_back(idx);
            
        }
        for (size_t i2 = 0; i2 < r.fields.size(); i2++) {
            Field& f = *r.fields[i2];
            Database::DBField* fld = new Database::DBField;
            fld->name = f.name +  LITESQL_L("_");
            fldMap[f.name] = fld;
            fld->type = f.getSQLType();
            fld->primaryKey = false;
            if (f.isUnique())
                fld->extra =  LITESQL_L(" UNIQUE");
            fld->field = r.fields[i2];
            tbl->fields.push_back(fld);
            
            if (f.isIndexed()) {
                Database::DBIndex* idx = new Database::DBIndex;
                idx->name = makeDBName(tbl->name + fld->name +  LITESQL_L("idx"));
                idx->table = tbl->name;
                idx->fields.push_back(fld);
                db.indices.push_back(idx);
            }
            
        }
    
        if (r.related.size() > 1) {

            Database::DBIndex* idx = new Database::DBIndex;
            idx->name = makeDBName(tbl->name +  LITESQL_L("_all_idx"));
            idx->table = tbl->name;
            for (size_t i2 = 0; i2 < objFields.size(); i2++)
                idx->fields.push_back(objFields[i2]);
            db.indices.push_back(idx);
        }
        for (size_t i2 = 0; i2 < r.indices.size(); i2++) {
            const Index& idx = *r.indices[i2];

            litesql::Split fldNames;
            Database::DBIndex* index = new Database::DBIndex;
            for (size_t i3 = 0; i3 < idx.fields.size(); i3++) {
//                Database::DBField* fld = new Database::DBField;
                if (fldMap.find(idx.fields[i3].name) == fldMap.end())
                    throw litesql::Except(LITESQL_L("Indexfield ") + r.name +  LITESQL_L(".") + idx.fields[i3].name +  LITESQL_L(" is invalid."));
                index->fields.push_back(fldMap[idx.fields[i3].name]);
                fldNames.push_back(idx.fields[i3].name);
            }

            index->name = makeDBName(tbl->name +  LITESQL_L("_") + fldNames.join(LITESQL_L("_")) +  LITESQL_L("_idx"));
            LITESQL_String unique =  LITESQL_L("");
            if (idx.isUnique())
                index->unique = true;
            db.indices.push_back(index);
        }
    }

}
void init(Database& db,
          std::vector<Object*>& objects,
          std::vector<Relation*>& relations) {
    std::map<LITESQL_String, Object*> objMap;
    Logger::report(LITESQL_L("validating XML file\n"));
    sanityCheck(db, objects, relations);
    Logger::report(LITESQL_L("linking XML - objects\n")); 
    // make LITESQL_String -> Object mapping

    for (size_t i = 0; i < objects.size(); i++)
        objMap[objects[i]->name] = objects[i];

    // make Object's class hierarchy mapping (parent and children)

    for (size_t i = 0; i < objects.size(); i++) 
      if (objMap.find(objects[i]->inherits) != objMap.end()) {
            objects[i]->parentObject = objMap[objects[i]->inherits];
            objects[i]->parentObject->children.push_back(objects[i]);
      }
    //for (size_t i = 0; i < objects.size(); i++) 
    //    if (objects[i]->parentObject)
    //        objects[i]->parentObject->children.push_back(objects[i]);

    // sort objects of relations alphabetically (ascii)

    for (size_t i = 0; i < relations.size(); i++) {
        std::sort(relations[i]->related.begin(), relations[i]->related.end(),
            std::less<Relate*>());
    }
          
    for (size_t i = 0; i < relations.size(); i++) {
        Relation& rel = *relations[i];
        bool same = rel.sameTypes() > 1;
        
        for (size_t i2 = 0; i2 < rel.related.size(); i2++) {
            Relate& relate = *rel.related[i2];
            Object* obj = objMap[relate.objectName];
            LITESQL_String num;
            if (same)
                num = toString(i2 + 1);
            relate.fieldTypeName = relate.objectName + num;
            relate.fieldName = relate.objectName + toString(i2 + 1);
            if (obj->relations.find(&rel) == obj->relations.end())
                obj->relations[&rel] = std::vector<Relate*>();

            // make Object -> Relation mapping

            obj->relations[&rel].push_back(&relate);
            if (!relate.handle.empty()) {
                
                // make Object's relation handles

                RelationHandle* handle = new RelationHandle(relate.handle, &rel,
                                                                      &relate, obj);
                for (size_t i3 = 0; i3 < rel.related.size(); i3++) {
                    if (i2 != i3) {
                        Object* o = objMap[rel.related[i3]->objectName];
                        // make RelationHandle -> (Object,Relation) mapping
                        handle->destObjects.push_back(std::make_pair(o,rel.related[i3]));
                    }
                }
                obj->handles.push_back(handle);
            }
        }
    }
    

    initSchema(db, objects, relations);

}
LITESQL_String safe(const LITESQL_Char * s) {
    if (s)
        return s;
    return  LITESQL_L("");
}

LITESQL_String attribute(const LITESQL_String& name, const LITESQL_String& value)
{
  LITESQL_String a;
  a.append(name).append(LITESQL_L("=")).append(LITESQL_L("\"")).append(value).append(LITESQL_L("\" "));
  return a;
}

LITESQL_String endtag(const LITESQL_String& name)
{
  LITESQL_String a;
  a.append(LITESQL_L("</")).append(name).append(LITESQL_L(">"));
  return a;
}


}