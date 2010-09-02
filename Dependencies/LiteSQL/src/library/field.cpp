/* LiteSQL - Syntactic sugar for Expr-API
 * 
 * The list of contributors at http://litesql.sf.net/ 
 * 
 * See LICENSE for copyright information. */
#include "litesql_char.hpp"
#include <cstdlib>
#include "compatibility.hpp"
#include "litesql.hpp"
#include "litesql/field.hpp"

namespace litesql {
In FieldType::in(const LITESQL_String& set) const {
    return In(*this, set);
}
In FieldType::in(const SelectQuery& sel) const {
    return In(*this, sel);
}
Like FieldType::like(const LITESQL_String& s) const {
    return Like(*this, s);
}
template <> 
LITESQL_String convert<int, LITESQL_String>(int value) {
    return toString(value);
}
template <>
LITESQL_String convert<float, LITESQL_String>(float value) {
    return toString(value);
}
template <>
LITESQL_String convert<double, LITESQL_String>(double value) {
    return toString(value);
}


template <> 
LITESQL_String convert<const int&, LITESQL_String>(const int& value) {
    return toString(value);
}
template <>
LITESQL_String convert<const float&, LITESQL_String>(const float& value) {
    return toString(value);
}

template <>
LITESQL_String convert<const double&, LITESQL_String>(const double& value) {
    return toString(value);
}

template <>
LITESQL_String convert<const bool&, LITESQL_String>(const bool& value) {
    return toString(value);
}

template <>
int convert<const LITESQL_String&, int>(const LITESQL_String& value) {
    return _tcstol(value.c_str(), NULL, 10);
}
template <>
bool convert<int, bool>(int value) {
    return value!=0;
}
template <> 
float convert<int, float>(int value) {
    return (float)value;
}

template <> 
double convert<int,double>(int value) {
    return (double)value;
}

template <>
bool convert<const LITESQL_String&, bool>(const LITESQL_String& value) {
    return convert<const LITESQL_String&, int>(value);
}
template <>
long long convert<const LITESQL_String&, long long>(const LITESQL_String& value) {
    return _tcstol(value.c_str(), NULL, 10);
}
template <>
float convert<const LITESQL_String&, float>(const LITESQL_String& value) {
    return strtof(value.c_str(), NULL);
}
template <>
double convert<const LITESQL_String&, double>(const LITESQL_String& value) {
    return _tcstod(value.c_str(), NULL);
}

template <>
LITESQL_String convert<const LITESQL_String&, LITESQL_String>(const LITESQL_String& value) {
    return value;
}



const LITESQL_Char hexDigits[] =  LITESQL_L("0123456789abcdef");

const Blob Blob::nil;

LITESQL_String Blob::toHex(void) const
{
  LITESQL_String result;
  if (!m_data) 
  {
    result = LITESQL_L("NULL");  
  }
  else
  {
    result.reserve(m_length);
    for (size_t i = 0; i < m_length;i++)
    {
      result.push_back(hexDigits[(m_data[i]&0xf0) >>4]);
      result.push_back(hexDigits[m_data[i]&0x0f]);
    }
  }
  return result;
}

int hex(LITESQL_Char c)
{
  switch(c)
  {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'a':
    case 'A':
      return 0xa;
    case 'b':
    case 'B':
      return 0xb;
    case 'c':
    case 'C':
      return 0xc;
    case 'd':
    case 'D':
      return 0xd;
    case 'e':
    case 'E':
      return 0xe;
    case 'f':
    case 'F':
      return 0xf;
    default:
      throw(LITESQL_L("invalid digit"));
  }
}

Blob::~Blob()
{ 
  if (m_data!=NULL) 
    free(m_data); 
}

void Blob::initWithHexString(const LITESQL_String& hexString)
{
  if (LITESQL_L("NULL")==hexString)
  {
    m_data = NULL;
    m_length = 0;
  }
  else
  {
    m_length = hexString.size()/2;
    m_data = (u8_t*) malloc(m_length);
    for (size_t i = 0; i < m_length;i++)
    {
      m_data[i] = (hex(hexString[2*i])<<4) | hex(hexString[2*i+1]);
    }
  }
}

void   Blob::data(const LITESQL_Char* pszData) 
{
  if (m_data!=NULL)
  {
    free(m_data);
  }
  initWithData((u8_t*)pszData,
		  pszData!=NULL ? _tcslen(pszData)+1 : 0);
}


void Blob::initWithData(const u8_t* data, size_t length)
{
  if (data!=m_data)
  {
    if (m_data!=NULL)
    {
      free(m_data);
    }

    if (data==NULL)
    {
      m_data = NULL;
      m_length = 0;
    }
    else
    {
      m_data = (u8_t*)malloc(length); 
      memcpy(m_data,data,length);
      m_length = length;
    }
  }
}

LITESQL_oStream& operator << (LITESQL_oStream& os, const Blob& blob)
{
  return os << convert<const Blob&, LITESQL_String>(blob);
}


template <>
Blob convert<const LITESQL_String&, Blob>(const LITESQL_String& value) 
{
  if (LITESQL_L("NULL")==value)
    return Blob(NULL,0);
  else
    return Blob(value);
}

template <>
LITESQL_String convert<const Blob&, LITESQL_String>(const Blob& value)
{
  if (value.isNull())
  {
    return  LITESQL_L("NULL");
  }
  else 
  {
    LITESQL_String hexVal;
    hexVal.append(LITESQL_L("X'"));
    hexVal.append(value.toHex());
    hexVal.append(LITESQL_L("'"));
    return hexVal;
  }
}


}