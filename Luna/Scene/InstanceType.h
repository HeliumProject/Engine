#pragma once

#include "InstanceSet.h"
#include "HierarchyNodeType.h"

namespace Luna
{
  class Primitive;
  class PrimitivePointer;

  class InstanceType;

  struct InstanceTypeChangeArgs
  {
    Luna::InstanceType* m_InstanceType;
    Luna::InstanceSet* m_InstanceSet;

    InstanceTypeChangeArgs( Luna::InstanceType* type, Luna::InstanceSet* set )
    : m_InstanceType( type )
    , m_InstanceSet( set )
    {
    }
  };

  typedef Nocturnal::Signature< void, const InstanceTypeChangeArgs& > InstanceTypeChangeSignature;

  class InstanceType : public Luna::HierarchyNodeType
  {
    //
    // Members
    //

  protected:
    // material that describes the color settings of the configuration
    D3DMATERIAL9 m_Material;

    // shared ui pointer is the basic visualization for all instances
    Luna::PrimitivePointer* m_Pointer;

    // the sets for this type
    M_InstanceSetSmartPtr m_Sets;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::InstanceType, Luna::HierarchyNodeType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    InstanceType( Luna::Scene* scene, i32 instanceType );
    virtual ~InstanceType();


    //
    // Graphics
    //

    const Luna::PrimitivePointer* GetPointer() const
    {
      return m_Pointer;
    }

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    bool IsTransparent() NOC_OVERRIDE;
    const D3DMATERIAL9& GetMaterial() const;


    //
    // Sets
    //

    const M_InstanceSetSmartPtr& GetSets() const
    {
      return m_Sets;
    }

    virtual void AddSet(Luna::InstanceSet* set);
    virtual void RemoveSet(Luna::InstanceSet* set);


    //
    // Events
    //

  protected:
    InstanceTypeChangeSignature::Event m_SetAdded;
  public:
    void AddSetAddedListener( const InstanceTypeChangeSignature::Delegate& listener )
    {
      m_SetAdded.Add( listener );
    }
    void RemoveSetAddedListener( const InstanceTypeChangeSignature::Delegate& listener )
    {
      m_SetAdded.Remove( listener );
    }

  protected:
    InstanceTypeChangeSignature::Event m_SetRemoved;
  public:
    void AddSetRemovedListener( const InstanceTypeChangeSignature::Delegate& listener )
    {
      m_SetRemoved.Add( listener );
    }
    void RemoveSetRemovedListener( const InstanceTypeChangeSignature::Delegate& listener )
    {
      m_SetRemoved.Remove( listener );
    }
  };
}
