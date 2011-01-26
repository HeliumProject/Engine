#ifndef litesql_xmlobjects_hpp
#define litesql_xmlobjects_hpp

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <map>
#include "litesql_char.hpp"
#include "litesql-gen.hpp"
#include "litesql/split.hpp"
#include "litesql/string.hpp"

#define NO_MEMBER_TEMPLATES
#include "litesql/counted_ptr.hpp"
#undef NO_MEMBER_TEMPLATES

namespace xml {
using namespace litesql;
LITESQL_String safe(const LITESQL_Char *s);
LITESQL_String attribute(const LITESQL_String& name, const LITESQL_String& value);
LITESQL_String endtag(const LITESQL_String& name);
LITESQL_String makeDBName(const LITESQL_String& s);

class Value {
public:
    static const LITESQL_Char* TAG;

    LITESQL_String name;
    LITESQL_String value;
    
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
  typedef counted_ptr<Index> counted_ptr;
  typedef std::vector<counted_ptr> sequence;

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
  typedef counted_ptr<Field> counted_ptr;
  typedef std::vector<counted_ptr> sequence;
  
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
        return (name!=LITESQL_L("id")) && (name!=LITESQL_L("type"));  
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
            return LITESQL_L("\"") + default_ + LITESQL_L("\"");
        if (default_.size() == 0)
        {
          switch(type) {
            case A_field_type_float: 
            case A_field_type_double: 
              return LITESQL_L("0.0");
            case A_field_type_blob: 
              return LITESQL_L("Blob::nil");
            default:
              return LITESQL_L("0");
          }
        }
        return default_;
    }
    LITESQL_String getSQLType() const {
       switch(type) {
           case A_field_type_integer: return LITESQL_L("INTEGER");
           case A_field_type_string: return LITESQL_L("TEXT");
           case A_field_type_float: return LITESQL_L("FLOAT");
           case A_field_type_double: return LITESQL_L("DOUBLE");
           case A_field_type_boolean: return LITESQL_L("INTEGER");
           case A_field_type_date: return LITESQL_L("INTEGER");
           case A_field_type_time: return LITESQL_L("INTEGER");
           case A_field_type_datetime: return LITESQL_L("INTEGER");
           case A_field_type_blob: return LITESQL_L("BLOB");
           default: return LITESQL_L("");
       }
    }
    LITESQL_String getCPPType() const {
       switch(type) {
           case A_field_type_integer: return LITESQL_L("int");
           case A_field_type_string: return LITESQL_L("LITESQL_String");
           case A_field_type_float: return LITESQL_L("float");
           case A_field_type_double: return LITESQL_L("double");
           case A_field_type_boolean: return LITESQL_L("bool");
           case A_field_type_date: return LITESQL_L("litesql::Date");
           case A_field_type_time: return LITESQL_L("litesql::Time");
           case A_field_type_datetime: return LITESQL_L("litesql::DateTime");
           case A_field_type_blob: return LITESQL_L("litesql::Blob");
           default: return LITESQL_L("");
       }
    }
};

inline bool operator==(const Field::counted_ptr& lhs,const Field::counted_ptr& rhs)
{
  return lhs.get()==rhs.get();
}

class Param {
public:
    static const LITESQL_Char* TAG;

  LITESQL_String name;
    AT_param_type type;
    Param(const LITESQL_String& n, AT_param_type t) : name(n), type(t) {}
    
};

class Method {
public:
  typedef counted_ptr<Method> counted_ptr;
  typedef std::vector<counted_ptr> sequence;

  static const LITESQL_Char* TAG;

    LITESQL_String name;
    LITESQL_String returnType;
    std::vector<Param> params;
    Method(const LITESQL_String& n, const LITESQL_String& rt) 
        : name(n), returnType(rt) {}
    void param(const Param& p) {
        params.push_back(p);
    }
};

inline bool operator==(const Method::counted_ptr& lhs,const Method::counted_ptr& rhs)
{
  return lhs.get()==rhs.get();
}

class Relation;
class Relate;
class Object;

typedef counted_ptr<Object> ObjectPtr;
typedef std::vector<ObjectPtr> ObjectSequence;

inline bool operator==(const ObjectPtr& lhs,const ObjectPtr& rhs)
{
  return lhs.get()==rhs.get();
}

class Relate {
public:
  typedef counted_ptr<Relate> counted_ptr;
  typedef std::vector<counted_ptr> sequence;

    static const LITESQL_Char* TAG;
  
    LITESQL_String objectName;
    LITESQL_String fieldTypeName;
    LITESQL_String fieldName;
    LITESQL_String getMethodName;
    size_t paramPos;
    AT_relate_limit limit;
    AT_relate_unique unique;
    LITESQL_String handle;
    Relate(const LITESQL_String& on, AT_relate_limit l, AT_relate_unique u, const LITESQL_String& h) 
        : objectName(on), limit(l), unique(u), handle(h) {
        if (hasLimit() && isUnique())
            throw std::logic_error("both limit and unique specified in relate: line "/*+ 
                              toString(yylineno)*/);
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
    
  struct CompareByObjectName
      : public std::binary_function<counted_ptr, counted_ptr, bool>
	{	// functor for operator<
	bool operator()(const counted_ptr& _Left, const counted_ptr& _Right) const
		{	// apply operator< to operands
		return (_Left->objectName < _Right->objectName);
		}
	};
};

inline bool operator==(const Relate::counted_ptr& lhs,const Relate::counted_ptr& rhs)
{ return lhs.get()==rhs.get();  }

class Relation {
public:
  typedef counted_ptr<Relation> counted_ptr;
  typedef std::vector<counted_ptr> sequence;

    static const LITESQL_Char* TAG;

    LITESQL_String id, name;
    LITESQL_String table;
    AT_relation_unidir unidir;
    Relate::sequence related;
    Field::sequence fields;
    Index::sequence indices;
    Relation(const LITESQL_String& i, const LITESQL_String& n, AT_relation_unidir ud) 
        : id(i), name(n), unidir(ud) {}
    LITESQL_String getName() const {
        if (name.size() == 0) {
            LITESQL_String result;
            for (size_t i = 0; i < related.size(); i++) 
                result += related[i]->objectName;
            return result + LITESQL_L("Relation") + id;
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
    size_t countTypes(const LITESQL_String& name) const {
        size_t res = 0;
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

inline bool operator==(const Relation::counted_ptr& lhs,const Relation::counted_ptr& rhs)
{
  return lhs.get()==rhs.get();
}

inline bool operator<(const Relation::counted_ptr& lhs,const Relation::counted_ptr& rhs)
{
  return lhs.get() < rhs.get();
}

class RelationHandle {
public:
  typedef counted_ptr<RelationHandle> counted_ptr;
  typedef std::vector<counted_ptr> sequence;

  LITESQL_String name;
  Relation::counted_ptr relation;
  Relate::counted_ptr relate;
  ObjectPtr object;
  std::vector< std::pair< ObjectPtr,Relate::counted_ptr > > destObjects;

  RelationHandle(const LITESQL_String& n, Relation::counted_ptr& r, Relate::counted_ptr& rel, ObjectPtr& o) 
    : name(n), relation(r), relate(rel), object(o) {}
};


class Object {
public:
  typedef counted_ptr<Object> counted_ptr;
  typedef std::vector<counted_ptr> sequence;


    static ObjectPtr DEFAULT_BASE;
    static Field::counted_ptr ID_FIELD;
    static Field::counted_ptr TYPE_FIELD;

    static const LITESQL_Char* TAG;

    LITESQL_String name, inherits;
    Field::sequence fields;
    Method::sequence methods;
    Index::sequence indices;
    RelationHandle::sequence handles;
    std::map<Relation::counted_ptr, Relate::sequence > relations;
    ObjectPtr parentObject;
    ObjectSequence children;

    Object(const LITESQL_String& n, const LITESQL_String& i) 
      : name(n), 
        inherits(i),
        parentObject(NULL) {
        if (i.size() == 0) {
            inherits = LITESQL_L("litesql::Persistent");
            fields.push_back(ID_FIELD);
            fields.push_back(TYPE_FIELD);
        }
    }

    bool inheritsFromDefault() const
    {
      return inherits == LITESQL_L("litesql::Persistent");
    }

    size_t getLastFieldOffset() const {
        if (!parentObject.get())
            return fields.size();
        else return parentObject->getLastFieldOffset() + fields.size();
    }
    void getAllFields(Field::sequence& flds) const {
        if (parentObject.get())
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
    const ObjectPtr getBaseObject() const{
      if (!parentObject.get())
            return ObjectPtr(NULL);
        else
            return parentObject->getBaseObject();
    }
    LITESQL_String getTable() const {
        return makeDBName(name + LITESQL_L("_"));
    }
    LITESQL_String getSequence() const {
        return makeDBName(name + LITESQL_L("_seq"));
    }
};

class Database {
public:

    static const LITESQL_Char* TAG;
    
    class Sequence {
    public:
      typedef counted_ptr<Sequence> counted_ptr;
      typedef std::vector<counted_ptr> sequence;
    
      LITESQL_String name;
      LITESQL_String table;
      
      LITESQL_String getSQL() {
            return LITESQL_L("CREATE SEQUENCE ") + name + LITESQL_L(" START 1 INCREMENT 1");
        }
    };
    
    class DBField {
    public:
      typedef counted_ptr<DBField> counted_ptr;
      typedef std::vector<counted_ptr> sequence;

        LITESQL_String name, type, extra;
        bool primaryKey;
        Field::counted_ptr field;
        sequence references;
        DBField() : primaryKey(false) {}
        LITESQL_String getSQL(const LITESQL_String& rowIDType) {
            if (primaryKey)
                type = rowIDType;
            return name + LITESQL_L(" ") + type + extra;
        }
    };
    
    class DBIndex {
    public:
        typedef counted_ptr<DBIndex> counted_ptr;
        typedef std::vector<counted_ptr> sequence;

        LITESQL_String name;
        LITESQL_String table;
        bool unique;
        DBField::sequence fields;
        DBIndex() : unique(false) {}
        LITESQL_String getSQL() {
            litesql::Split flds;
            for (size_t i = 0; i < fields.size(); i++)
                flds.push_back(fields[i]->name);
            LITESQL_String uniqueS;
            if (unique)
                uniqueS = LITESQL_L(" UNIQUE");
            return LITESQL_L("CREATE") + uniqueS + LITESQL_L(" INDEX ") + name + LITESQL_L(" ON ") + table + LITESQL_L(" (") + flds.join(LITESQL_L(",")) + LITESQL_L(")");
        }
    };
    
    class Table {
    public:
      typedef counted_ptr<Table> counted_ptr;
      typedef std::vector<counted_ptr> sequence;

      LITESQL_String name;
        DBField::sequence fields;
        LITESQL_String getSQL(const LITESQL_String& rowIDType) {
            litesql::Split flds;
            for (size_t i = 0; i < fields.size(); i++)
                flds.push_back(fields[i]->getSQL(rowIDType));
            return LITESQL_L("CREATE TABLE ") + name + LITESQL_L(" (") + flds.join(LITESQL_L(",")) + LITESQL_L(")");
        }

    };
    
    Sequence::sequence sequences;
    DBIndex::sequence indices;
    Table::sequence tables;
    LITESQL_String name;
    LITESQL_String include;
    LITESQL_String nspace;

    bool hasNamespace() const { return !nspace.empty(); }
};

typedef counted_ptr<Database> DatabasePtr;

void init(DatabasePtr& db, 
          Object::sequence& objects,
          Relation::sequence& relations);


}

#endif