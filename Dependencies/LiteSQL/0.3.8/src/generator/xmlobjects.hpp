#ifndef litesql_xmlobjects_hpp
#define litesql_xmlobjects_hpp
#include "litesql_char.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include "litesql-gen.hpp"
#include "litesql/split.hpp"
#include "litesql/string.hpp"

namespace xml {
using namespace litesql;
LITESQL_String safe(const LITESQL_Char *s);
LITESQL_String attribute(const LITESQL_String& name, const LITESQL_String& value);
LITESQL_String endtag(const LITESQL_String& name);
LITESQL_String makeDBName(const LITESQL_String& s);

class Value {
public:
    static const LITESQL_Char* TAG;

    LITESQL_String name, value;
    Value(const LITESQL_String& n, const LITESQL_String& v) : name(n), value(v) {}
};

class IndexField {
public:
    static const LITESQL_Char* TAG;

    LITESQL_String name;
    IndexField(const LITESQL_String& n) : name(n) {}
};

class Index {
public:
    static const LITESQL_Char* TAG;

    std::vector<IndexField> fields;
    AT_index_unique unique;
    Index(AT_index_unique u) : unique(u) {}

    bool isUnique() const {
        return unique == A_index_unique_true;
    }
};

class Field {
public:
    static const LITESQL_Char* TAG;

    LITESQL_String name;
    LITESQL_String fieldTypeName;
    AT_field_type type;
    LITESQL_String default_;
    AT_field_indexed indexed;
    AT_field_unique unique;
    std::vector<Value> values;
    Field(const LITESQL_String& n, AT_field_type t, const LITESQL_String& d, AT_field_indexed i, AT_field_unique u) 
        : name(n), fieldTypeName(capitalize(n)), type(t), default_(d), indexed(i), unique(u) {
    }
    void value(const Value& v) {
        values.push_back(v);
    }
    
    bool isEditable() {
        return (name!= LITESQL_L("id")) && (name!= LITESQL_L("type"));  
    }

    bool isIndexed() const {
        return indexed == A_field_indexed_true;
    }
    bool isUnique() const {
        return unique == A_field_unique_true;
    }
    bool hasQuotedValues() const {
       switch(type) {
       case A_field_type_integer:
       case A_field_type_float: 
       case A_field_type_double: 
       case A_field_type_boolean: 
       case A_field_type_date:
       case A_field_type_time:
       case A_field_type_datetime:
       case A_field_type_blob:
          return false;
       
       case A_field_type_string:
       default:
         return true;
       }
    }
    LITESQL_String getQuotedDefaultValue() const {
        if (hasQuotedValues())
            return  LITESQL_L("\"") + default_ +  LITESQL_L("\"");
        if (default_.size() == 0)
        {
          switch(type) {
            case A_field_type_float: 
            case A_field_type_double: 
              return  LITESQL_L("0.0");
            case A_field_type_blob: 
              return  LITESQL_L("Blob::nil");
            default:
              return  LITESQL_L("0");
          }
        }
        return default_;
    }
    LITESQL_String getSQLType() const {
       switch(type) {
           case A_field_type_integer: return  LITESQL_L("INTEGER");
           case A_field_type_string: return  LITESQL_L("TEXT");
           case A_field_type_float: return  LITESQL_L("FLOAT");
           case A_field_type_double: return  LITESQL_L("DOUBLE");
           case A_field_type_boolean: return  LITESQL_L("INTEGER");
           case A_field_type_date: return  LITESQL_L("INTEGER");
           case A_field_type_time: return  LITESQL_L("INTEGER");
           case A_field_type_datetime: return  LITESQL_L("INTEGER");
           case A_field_type_blob: return  LITESQL_L("BLOB");
           default: return  LITESQL_L("");
       }
    }
    LITESQL_String getCPPType() const {
       switch(type) {
           case A_field_type_integer: return  LITESQL_L("int");
           case A_field_type_string: return  LITESQL_L("LITESQL_String");
           case A_field_type_float: return  LITESQL_L("float");
           case A_field_type_double: return  LITESQL_L("double");
           case A_field_type_boolean: return  LITESQL_L("bool");
           case A_field_type_date: return  LITESQL_L("litesql::Date");
           case A_field_type_time: return  LITESQL_L("litesql::Time");
           case A_field_type_datetime: return  LITESQL_L("litesql::DateTime");
           case A_field_type_blob: return  LITESQL_L("litesql::Blob");
           default: return  LITESQL_L("");
       }
    }
};

class Param {
public:
    static const LITESQL_Char* TAG;

  LITESQL_String name;
    AT_param_type type;
    Param(const LITESQL_String& n, AT_param_type t) : name(n), type(t) {}
    
};
class Method {
public:
    static const LITESQL_Char* TAG;

    LITESQL_String name, returnType;
    std::vector<Param> params;
    Method(const LITESQL_String& n, const LITESQL_String& rt) 
        : name(n), returnType(rt) {}
    void param(const Param& p) {
        params.push_back(p);
    }
};
class Relation;
class Relate;
class Object;
class RelationHandle {
public:
    LITESQL_String name;
    Relation * relation;
    Relate * relate;
    Object * object;
    std::vector< std::pair<Object*,Relate*> > destObjects;

    RelationHandle(const LITESQL_String& n, Relation * r, Relate * rel, Object * o) 
        : name(n), relation(r), relate(rel), object(o) {}
};
class Relate {
public:  
  static const LITESQL_Char* TAG;
  
    LITESQL_String objectName;
    LITESQL_String fieldTypeName, fieldName;
    LITESQL_String getMethodName;
    size_t paramPos;
    AT_relate_limit limit;
    AT_relate_unique unique;
    LITESQL_String handle;
    Relate(const LITESQL_String& on, AT_relate_limit l, AT_relate_unique u, const LITESQL_String& h) 
        : objectName(on), limit(l), unique(u), handle(h) {
            if (hasLimit() && isUnique()) {
                throw std::logic_error("both limit and unique specified in relate: line " /*+ toString(yylineno)*/);
            }
    }
    bool hasLimit() const {
        return limit == A_relate_limit_one;
    }
    
    bool hasLimitOne() const {
        return limit == A_relate_limit_one;
    }
    bool isUnique() const {
        return unique == A_relate_unique_true;
    }
    bool operator < (const Relate& r) const {
        return objectName < r.objectName;
    }

};
class Relation {
public:
    static const LITESQL_Char* TAG;

    LITESQL_String id, name;
    LITESQL_String table;
    AT_relation_unidir unidir;
    std::vector<Relate*> related;
    std::vector<Field*> fields;
    std::vector<Index*> indices;
    Relation(const LITESQL_String& i, const LITESQL_String& n, AT_relation_unidir ud) 
        : id(i), name(n), unidir(ud) {}
    LITESQL_String getName() const {
        if (name.size() == 0) {
            LITESQL_String result;
            for (size_t i = 0; i < related.size(); i++) 
                result += related[i]->objectName;
            return result +  LITESQL_L("Relation") + id;
        }
        return name;
    }
    bool isUnidir() const {
        return unidir == A_relation_unidir_true;
    }
    int sameTypes() const {
        std::map<LITESQL_String, int> names;
        int max = 0;
        for (size_t i = 0; i < related.size(); i++) {
            if (names.find(related[i]->objectName) == names.end()) 
                names[related[i]->objectName] = 0;
            int value =	++names[related[i]->objectName];
            if (value > max)
                max = value;
        }
        return max;
    }
    int countTypes(const LITESQL_String& name) const {
        int res = 0;
        for (size_t i = 0; i < related.size(); i++)
            if (related[i]->objectName == name)
                res++;
        return res;
    }
    LITESQL_String getTable() const {
        Split res(related.size());
        for (size_t i = 0; i < related.size(); i++)
            res.push_back(related[i]->objectName);
        res.push_back(id);

        return makeDBName(res.join(LITESQL_L("_")));
    }
};
class Object {
public:
    static const Object DEFAULT_BASE;
    static const LITESQL_Char* TAG;

    LITESQL_String name, inherits;
    std::vector<Field*> fields;
    std::vector<Method*> methods;
    std::vector<Index*> indices;
    std::vector<RelationHandle*> handles;
    std::map<Relation*, std::vector<Relate*> > relations;
    Object* parentObject;
    std::vector<Object*> children;

    Object(const LITESQL_String& n, const LITESQL_String& i) : name(n), inherits(i),
        parentObject(NULL) {
        if (i.size() == 0) {
            inherits =  LITESQL_L("litesql::Persistent");
            fields.push_back(new Field(LITESQL_L("id"), A_field_type_integer,  LITESQL_L(""), 
                         A_field_indexed_false, A_field_unique_false));
            fields.push_back(new Field(LITESQL_L("type"), A_field_type_string,  LITESQL_L(""), 
                        A_field_indexed_false, A_field_unique_false));
        }
    }

    bool inheritsFromDefault() const
    {
      return inherits ==  LITESQL_L("litesql::Persistent");
    }

    size_t getLastFieldOffset() const {
        if (!parentObject)
            return fields.size();
        else return parentObject->getLastFieldOffset() + fields.size();
    }
    void getAllFields(std::vector<Field*>& flds) const {
        if (parentObject)
            parentObject->getAllFields(flds);
        for (size_t i = 0; i < fields.size(); i++)
            flds.push_back(fields[i]);
    }
    void getChildrenNames(Split & names) const {
        for (size_t i = 0; i < children.size(); i++) {
            names.push_back(children[i]->name);
            children[i]->getChildrenNames(names);
        }
    }
    const Object* getBaseObject() const{
        if (!parentObject)
            return this;
        else
            return parentObject->getBaseObject();
    }
    LITESQL_String getTable() const {
        return makeDBName(name +  LITESQL_L("_"));
    }
    LITESQL_String getSequence() const {
        return makeDBName(name +  LITESQL_L("_seq"));
    }
};
class Database {
public:
  static const LITESQL_Char* TAG;
    class Sequence {
    public:
        LITESQL_String name, table;
        LITESQL_String getSQL() {
            return  LITESQL_L("CREATE SEQUENCE ") + name +  LITESQL_L(" START 1 INCREMENT 1");
        }
    };
    class DBField {
    public:
        LITESQL_String name, type, extra;
        bool primaryKey;
        Field* field;
        std::vector<DBField*> references;
        DBField() : primaryKey(false) {}
        LITESQL_String getSQL(const LITESQL_String& rowIDType) {
            if (primaryKey)
                type = rowIDType;
            return name +  LITESQL_L(" ") + type + extra;
        }
    };
    class DBIndex {
    public:
        LITESQL_String name, table;
        bool unique;
        std::vector<DBField*> fields;
        DBIndex() : unique(false) {}
        LITESQL_String getSQL() {
            litesql::Split flds;
            for (size_t i = 0; i < fields.size(); i++)
                flds.push_back(fields[i]->name);
            LITESQL_String uniqueS;
            if (unique)
                uniqueS =  LITESQL_L(" UNIQUE");
            return  LITESQL_L("CREATE") + uniqueS +  LITESQL_L(" INDEX ") + name +  LITESQL_L(" ON ") + table +  LITESQL_L(" (") + flds.join(LITESQL_L(",")) +  LITESQL_L(")");
        }
    };
    class Table {
    public:
        LITESQL_String name;
        std::vector<DBField*> fields;
        LITESQL_String getSQL(const LITESQL_String& rowIDType) {
            litesql::Split flds;
            for (size_t i = 0; i < fields.size(); i++)
                flds.push_back(fields[i]->getSQL(rowIDType));
            return  LITESQL_L("CREATE TABLE ") + name +  LITESQL_L(" (") + flds.join(LITESQL_L(",")) +  LITESQL_L(")");
        }

    };
    std::vector<Sequence*> sequences;
    std::vector<DBIndex*> indices;
    std::vector<Table*> tables;
    LITESQL_String name, include, nspace;

    bool hasNamespace() const { return !nspace.empty(); }
};

void init(Database& db, 
          std::vector<Object*>& objects,
          std::vector<Relation*>& relations);


}

#endif