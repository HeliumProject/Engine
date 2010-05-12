#pragma once

#include <map>

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

#include "API.h"
#include "Base.h"
#include "Enumeration.h"

namespace Reflect
{
  //
  // Some basic types
  //

  // fwd decl
  class Composite;


  //
  // Field, contains all the data about a field of a structure
  //

  namespace FieldFlags
  {
    enum FieldFlags
    {
      Discard     = 1 << 0,        // disposable fields are not serialized
      Force       = 1 << 1,        // forced fields are always serialized
      Share       = 1 << 2,        // shared fields are not cloned or compared deeply
      Hide        = 1 << 3,        // hidden fields are not inherently visible in UI
      ReadOnly    = 1 << 4,        // read-only fields cannot be edited in the UI inherently
      FilePath    = 1 << 5,        // file path fields are strings that are file paths
      FileID      = 1 << 6,        // file id fields are identifiers for file paths
      NodeID      = 1 << 7,        // this field is a container of other things
      RuntimeEnum = 1 << 8,        // this field is a runtime enum

      // !!IMPORTANT: MUST BE UPDATED ANY TIME A NEW FLAG IS INTRODUCED!!
      Last        = RuntimeEnum    // the last used flag
    };
  }


  //
  // Field, fully qualified field information
  //

  class REFLECT_API Field : public Base
  {
  public:
    REFLECTION_BASE( ReflectionTypes::Field );

    const Composite*    m_Type;         // the type we are a field of
    std::string         m_Name;         // name of this field
    std::string         m_UIName;       // friendly name
    u32                 m_Size;         // the size of this field
    PointerSizedUInt    m_Offset;       // the offset to the field
    u32                 m_Flags;        // flags for special behavior
    i32                 m_FieldID;      // the unique id of this field
    i32                 m_SerializerID; // type id of the serializer to use
    SerializerPtr       m_Default;      // the value of the default
    CreateObjectFunc    m_Create;       // function to create a new instance for this field (optional)

  protected:
    Field(const Composite* type);
    virtual ~Field();

  public:
    // protect external allocation to keep inlined code in this dll
    static Field* Create(const Composite* type);

    // creates a suitable serializer for this field in the passed object
    virtual SerializerPtr CreateSerializer (Element* instance = NULL) const;

    // checks to see if the default value matches the value of this field in the passed object
    bool HasDefaultValue (Element* instance) const;

    // sets the default value of this field in the passed object
    bool SetDefaultValue (Element* instance) const;

    // set the name (and UI name if its not set)
    void SetName(const std::string& name);
  };

  typedef Nocturnal::SmartPtr< Field > FieldPtr;
  typedef Nocturnal::SmartPtr< const Field > ConstFieldPtr;
  typedef std::map< i32,          ConstFieldPtr > M_FieldIDToInfo;
  typedef std::map< std::string,  ConstFieldPtr > M_FieldNameToInfo;
  typedef std::map< u32,          ConstFieldPtr > M_FieldOffsetToInfo;

  //
  // ElementField store additional information the compile-time type of a pointer
  //

  class REFLECT_API ElementField : public Field
  {
  public:
    REFLECTION_TYPE( ReflectionTypes::ElementField );

    i32 m_TypeID;

  protected:
    ElementField(const Composite* type);
    virtual ~ElementField();

  public:
    // protect external allocation to keep inlined code in this dll
    static ElementField* Create( const Composite* type );

    // creates a suitable serializer (that has a pointer to the enum info)
    virtual SerializerPtr CreateSerializer ( Element* instance = NULL ) const NOC_OVERRIDE;
  };

  //
  // EnumerationField store additional information to map enumerations as string values in the file
  //

  class REFLECT_API EnumerationField : public Field
  {
  public:
    REFLECTION_TYPE( ReflectionTypes::EnumerationField );

    const Enumeration* m_Enumeration;

  protected:
    EnumerationField(const Composite* type, const Enumeration* enumeration);
    virtual ~EnumerationField();

  public:
    // protect external allocation to keep inlined code in this dll
    static EnumerationField* Create( const Composite* type, const Enumeration* enumeration );

    // creates a suitable serializer (that has a pointer to the enum info)
    virtual SerializerPtr CreateSerializer ( Element* instance = NULL ) const NOC_OVERRIDE;
  };
}
