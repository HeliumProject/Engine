#pragma once

#include "AssetNode.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards & typedefs
  class FieldNode;
  typedef Nocturnal::SmartPtr< Luna::FieldNode > FieldNodePtr;

  class FieldNode NOC_ABSTRACT : public Luna::AssetNode
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::FieldNode, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  protected:
    Reflect::ElementPtr m_Element;
    const Reflect::Field* m_Field;
    Reflect::SerializerPtr m_Serializer;

  public:
    FieldNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field );
    virtual ~FieldNode();

    Reflect::Element* GetElement() const;
    const Reflect::Field* GetField() const;

    bool HasFlag( u32 flag ) const;
    std::string GetValue() const;

    virtual void HandleFieldChanged();

  private:
    void OnElementChanged( const Reflect::ElementChangeArgs& args );
  };
  typedef std::vector< Luna::FieldNodePtr > V_FieldNodeSmartPtr;
}
