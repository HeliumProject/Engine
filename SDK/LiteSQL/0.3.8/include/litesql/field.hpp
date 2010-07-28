/* LiteSQL 
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */

#ifndef litesql_field_hpp
#define litesql_field_hpp
#include "litesql_char.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include "litesql/string.hpp"

/** \file field.hpp
    contains FieldType- and Field-classes */
namespace litesql {

/** holds field name, type and table information */
class In;
class Like;
class SelectQuery;

class FieldType {
protected:
    typedef std::vector< std::pair<LITESQL_String, LITESQL_String> > Values;
public:
    FieldType() {};
    FieldType(const LITESQL_String& n, 
              const LITESQL_String& t, 
              const LITESQL_String& tbl,
              const Values& vals = Values())
        : _name(n), _type(t), _table(tbl), _values(vals) {}
    LITESQL_String fullName() const { return  table() +  LITESQL_L(".") + name(); }
    LITESQL_String name() const { return _name; }
    LITESQL_String type() const { return _type; }
    LITESQL_String table() const { return _table; }
    std::vector< std::pair<LITESQL_String, LITESQL_String> > values() { return _values; }
    /** syntactic sugar to Expr-API, Object::field_.in(set) */
    In in(const LITESQL_String& set) const;
    /** syntactic sugar to Expr-API, Object::field_.in(sel) */
    In in(const SelectQuery& sel) const;
    /** syntactic sugar to Expr-API, Object::field_.like(s) */
    Like like(const LITESQL_String& s);
    bool operator==(const FieldType & fd) const {
        return fd.fullName() == fullName();
    }
    bool operator!=(const FieldType & fd) const {
        return ! (*this == fd);
    }
private:
    LITESQL_String _name, _type, _table;
    Values _values;
};

/** convert function */
template <class From, class To>
To convert(From value);

/** store function */
template <class T>
LITESQL_String store(const T& value) {
    return litesql::toString(value);
//  return convert<T,LITESQL_String>(value);
}

template <class T>
T load(const LITESQL_String& value) {
    return convert<const LITESQL_String&, T>(value);
}
/** holds field value */
template <class T>
class Field {
    const FieldType * field; 
    bool _modified;
    T _value;
public:
    Field(const FieldType & f) : field(&f), _modified(true) {}
    LITESQL_String fullName() const { return field->fullName(); }   
    LITESQL_String name() const { return field->name(); }
    LITESQL_String type() const { return field->type(); }
    LITESQL_String table() const { return field->table(); }
    T value() const { return _value; }
    const FieldType & fieldType() const { return *field; } 
    bool modified() const { return _modified; }
    void setModified(bool state) { _modified = state; }
    const Field & operator=(const LITESQL_String& v) { 
        _value = convert<const LITESQL_String&, T>(v);
        _modified = true;
        return *this;
    }
    const Field & operator=(const T& v) {
        _value = v;
        _modified = true;
        return *this;
    }
    template <class T2>
    const Field & operator=(T2 v) { 
        _modified = true;
        _value = litesql::convert<T2, T>(v); 
        return *this;
    }
    template <class T2>
    bool operator==(const T2& v) const {
        return litesql::toString(_value) == litesql::toString(v);
    }
    template <class T2>
    bool operator!=(const T2& v) const { return !(*this == v); }
    

    operator LITESQL_String() const { return toString(value()); }

    operator T() const { return value(); }
};

template <>
class Field<LITESQL_String> {
    const FieldType * field; 
    bool _modified;
    LITESQL_String _value;
public:
    Field(const FieldType & f) : field(&f), _modified(true) {}
    LITESQL_String fullName() const { return field->fullName(); }   
    LITESQL_String name() const { return field->name(); }
    LITESQL_String type() const { return field->type(); }
    LITESQL_String table() const { return field->table(); }
    LITESQL_String value() const { return _value; }
    const FieldType & fieldType() const { return *field; } 
    bool modified() const { return _modified; }
    void setModified(bool state) { _modified = state; }
    const Field & operator=(LITESQL_String v) { 
        _value = v;
        _modified = true;
        return *this;
    }
    const Field& operator=(const LITESQL_Char * v) {
        _value = v;
        _modified = true;
        return *this;
    }
    template <class T2>
    const Field & operator=(T2 v) { 
        _modified = true;
        _value = litesql::convert<T2, LITESQL_String>(v); 
        return *this;
    }
    template <class T2>
    bool operator==(const T2& v) const {
        return litesql::toString(_value) == litesql::toString(v);
    }
    template <class T2>
    bool operator!=(const T2& v) const { return !(*this == v); }

    operator LITESQL_String() const { return value(); }
};

typedef unsigned char u8_t;

class Blob {
public:
  static const Blob nil;
  Blob()                            : m_data(NULL),m_length(0)               {};
  Blob(const LITESQL_String & value) : m_data(NULL),m_length(0)
  {
    initWithHexString(value);
  };


  Blob(const Blob & b) : m_data(NULL)
  { 
    initWithData(b.m_data,b.m_length);
  };

  Blob(const void* data, size_t length) : m_data(NULL), m_length(0)
  {
    initWithData((u8_t*)data,length);
  };
  
  virtual ~Blob();
  const Blob& operator=(const Blob& v) {
    initWithData(v.m_data,v.m_length);
    return *this;
  }

  LITESQL_String toHex()            const ;
  size_t length()           const  { return m_length;      };
  bool   isNull()           const  { return m_data==NULL;  }; 
  u8_t   data(size_t index) const  { return m_data[index]; };
  void   data(const LITESQL_Char* pszData);
private:
  u8_t* m_data;
  size_t m_length;

  void initWithData(const u8_t* data, size_t length);
  void initWithHexString(const LITESQL_String& hexString);
};

LITESQL_oStream& operator << (LITESQL_oStream& os, const Blob& blob);
template <>
Blob convert<const LITESQL_String&, Blob>(const LITESQL_String& value);
template <>
LITESQL_String convert<const Blob&, LITESQL_String>(const Blob& value);

template <>
class Field<Blob> {
    const FieldType * field; 
    bool _modified;
    Blob _value;
public:
    Field(const FieldType & f) : field(&f), _modified(true) {}
    LITESQL_String fullName() const { return field->fullName(); }   
    LITESQL_String name() const { return field->name(); }
    LITESQL_String type() const { return field->type(); }
    LITESQL_String table() const { return field->table(); }
    Blob value() const { return _value; }
    const FieldType & fieldType() const { return *field; } 
    bool modified() const { return _modified; }
    void setModified(bool state) { _modified = state; }
    const Field & operator=(const Blob& v) { 
        _value = v;
        _modified = true;
        return *this;
    }

/*
const Field& operator=(const LITESQL_Char * v) {
        _value = v;
        _modified = true;
        return *this;
    }
    template <class T2>
    const Field & operator=(T2 v) { 
        _modified = true;
        _value = litesql::convert<T2, Blob>(v); 
        return *this;
    }
    template <class T2>
    bool operator==(const T2& v) const {
        return litesql::toString(_value) == litesql::toString(v);
    }
    template <class T2>
    bool operator!=(const T2& v) const { return !(*this == v); }
*/

    operator LITESQL_String() const { return _value.toHex(); }
};

template <class T>
LITESQL_String operator+(LITESQL_String a, litesql::Field<T> f) {
    return a + LITESQL_String(f);
}
template <class T>
LITESQL_String operator+(litesql::Field<T> f, LITESQL_String a) {
    return LITESQL_String(f) + a;    
}
template <class T>
LITESQL_oStream & operator << (LITESQL_oStream & os, const litesql::Field<T> & f) {
    return os << f.value();
}

}
#endif