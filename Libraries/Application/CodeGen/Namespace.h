#pragma once

#include "Application/API.h"

#include "Node.h"
#include "Enum.h"

namespace CodeGen
{
  // Forwards
  class Namespace;
  typedef Nocturnal::SmartPtr< Namespace > NamespacePtr;
  typedef std::vector< NamespacePtr > V_NamespaceSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing a C++ namespace.
  // 
  class APPLICATION_API Namespace : public Node
  {
  private:
    OS_EnumSmartPtr m_Enums;
    V_NamespaceSmartPtr m_NestedNamespaces;

  public:
    Namespace( const std::string& name );
    ~Namespace();

    bool AppendEnum( const EnumPtr& enumPtr, std::string& error );
    bool PrependEnum( const EnumPtr& enumPtr, std::string& error );
    const OS_EnumSmartPtr& GetEnums() const;

    void AddNamespace( const NamespacePtr& namespacePtr );
    const V_NamespaceSmartPtr& GetNamespaces() const;

    bool Contains( const std::string& name ) const;
    const Namespace* FindNamespace( const std::string& name ) const;
    const Enum* FindEnum( const std::string& name ) const;
    const EnumValue* FindEnumValue( const std::string& name ) const;
  };
}
