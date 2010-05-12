#pragma once

#include "PivotTransform.h"

#include "Content/Instance.h"

namespace Luna
{
  class InstanceCodeSet;
  class Instance;

  struct InstancePropertiesChangeArgs
  {
    Luna::Instance* m_Instance;
    std::string     m_OldType;
    std::string     m_NewType;

    InstancePropertiesChangeArgs( Luna::Instance*    instance,
                                  const std::string& oldType,
                                  const std::string& newType)
      : m_Instance( instance )
      , m_OldType (oldType)
      , m_NewType (newType)
    {}
  };
  typedef Nocturnal::Signature< void, const InstancePropertiesChangeArgs& > InstancePropertiesChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an entity instance.
  // 
  class LUNA_SCENE_API Instance NOC_ABSTRACT : public Luna::PivotTransform
  {
    //
    // Members
    //

  protected:
    Luna::InstanceCodeSet* m_CodeSet;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Instance, Luna::PivotTransform );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Instance(Luna::Scene* s, Content::Instance* entity);
    virtual ~Instance();

    virtual void Pack() NOC_OVERRIDE;
    virtual void Unpack() NOC_OVERRIDE;

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;
    virtual Luna::SceneNodeType* DeduceNodeType() NOC_OVERRIDE;

    // support for checking class membership when checking type
    virtual void CheckNodeType() NOC_OVERRIDE;

    // generate name
    virtual std::string GenerateName() const NOC_OVERRIDE;

    // find all the valid configured type names
    S_string GetValidConfiguredTypeNames();

    std::string GetConfiguredTypeName() const;
    void SetConfiguredTypeName( const std::string& type );

    std::string GetRuntimeClassName() const;
    void SetRuntimeClassName( const std::string& className );

    Luna::InstanceCodeSet* GetCodeSet();
    const Luna::InstanceCodeSet* GetCodeSet() const;
    void SetCodeSet(Luna::InstanceCodeSet* codeClass);
    virtual void CheckSets();
    virtual void ReleaseSets();

    // match by code class
    virtual void FindSimilar(V_HierarchyNodeDumbPtr& similar) const NOC_OVERRIDE;
    virtual bool IsSimilar(const HierarchyNodePtr& node) const NOC_OVERRIDE;

    // get some useful info
    virtual std::string GetDescription() const;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    bool GetSolid() const;
    void SetSolid( bool b );
    bool GetSolidOverride() const;
    void SetSolidOverride( bool b );

    bool GetTransparent() const;
    void SetTransparent( bool b );
    bool GetTransparentOverride() const;
    void SetTransparentOverride( bool b );

    const bool  GetIsWeatherBlocker() const;

    bool        GetSkipParticles() const;
    void        SetSkipParticles( bool s );

    float       GetBorderSize() const;
    void        SetBorderSize( float s );

    //
    // Events
    //
  protected:
    InstancePropertiesChangeSignature::Event m_Changed;
  public:
    void AddConfiguredTypeChangedListener( const InstancePropertiesChangeSignature::Delegate& listener )
    {
      m_Changed.Add( listener );
    }

    void RemoveConfiguredTypeChangedListener( const InstancePropertiesChangeSignature::Delegate& listener )
    {
      m_Changed.Remove( listener );
    }
  };

  typedef std::vector< Luna::Instance* > V_InstanceDumbPtr;
}
