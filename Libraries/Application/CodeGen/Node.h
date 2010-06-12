#pragma once

#include <string>

#include "Foundation/Memory/SmartPtr.h"

#include "Application/API.h"

namespace CodeGen
{
  /////////////////////////////////////////////////////////////////////////////
  // Enumeration to differentiate between the different types of names for a node.
  // 
  namespace NameTypes
  {
    enum NameType
    {
      FullName,
      ShortName,
    };
  }
  typedef NameTypes::NameType NameType;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing a C++ node (namespace, enum, struct, class).
  // 
  class APPLICATION_API Node : public Nocturnal::RefCountBase<Node>
  {
  private:
    Node* m_Parent;
    std::string m_ShortName;
    mutable std::string m_FullName;
    mutable bool m_IsFullNameDirty;

  public:
    Node( const std::string& name );
    virtual ~Node();

    const std::string& GetName( NameType which = NameTypes::ShortName ) const;
    const std::string& GetShortName() const;
    const std::string& GetFullName() const;

    void SetParent( Node* parent );
    const Node* GetParent() const;
  };

  typedef Nocturnal::SmartPtr< Node > NodePtr;
  typedef std::vector< NodePtr > V_NodeSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Helper function to search a string for ':' which is used to determine whether
  // a full name or short name is given.
  static inline NameType GetNameType( const std::string name )
  {
    if ( name.find( ':' ) != std::string::npos )
    {
      return NameTypes::FullName;
    }
    return NameTypes::ShortName;
  }

  // Helper function to get the short name from a full one.
  APPLICATION_API std::string GetShortName( const std::string& fullName );
}
