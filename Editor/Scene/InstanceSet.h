#pragma once

#include "Viewport.h"
#include "Object.h"

namespace Editor
{
  class Instance;
  typedef std::set< Editor::Instance* > S_InstanceDumbPtr;

  class InstanceSet;

  class InstanceType;

  struct InstanceSetChangeArgs
  {
    Editor::InstanceSet* m_InstanceSet;
    Editor::Instance* m_Instance;

    InstanceSetChangeArgs( Editor::InstanceSet* set, Editor::Instance* entity )
    : m_InstanceSet( set )
    , m_Instance( entity )
    {
    }
  };

  typedef Helium::Signature< void, const InstanceSetChangeArgs& > InstanceSetChangeSignature;

  class InstanceSet : public Object
  {
    //
    // Members
    //

  protected:
    Editor::InstanceType* m_Type;
    S_InstanceDumbPtr m_Instances;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Editor::InstanceSet, Object );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    InstanceSet(Editor::InstanceType* type);

    virtual ~InstanceSet();

    Editor::InstanceType* GetInstanceType();

    virtual void AddInstance(Editor::Instance* i);

    virtual void RemoveInstance(Editor::Instance* i);

    virtual bool ContainsInstance(Editor::Instance* i);

    virtual const S_InstanceDumbPtr& GetInstances() const;

    // Must be overridden in derived classes
    virtual const tstring& GetName() const = 0;

    // 
    // Listeners
    // 

  protected:
    InstanceSetChangeSignature::Event m_InstanceAdded;
  public:
    void AddInstanceAddedListener( const InstanceSetChangeSignature::Delegate& listener )
    {
      m_InstanceAdded.Add( listener );
    }
    void RemoveInstanceAddedListener( const InstanceSetChangeSignature::Delegate& listener )
    {
      m_InstanceAdded.Remove( listener );
    }

  protected:
    InstanceSetChangeSignature::Event m_InstanceRemoved;
  public:
    void AddInstanceRemovedListener( const InstanceSetChangeSignature::Delegate& listener )
    {
      m_InstanceRemoved.Add( listener );
    }
    void RemoveInstanceRemovedListener( const InstanceSetChangeSignature::Delegate& listener )
    {
      m_InstanceRemoved.Remove( listener );
    }
  };

  typedef Helium::SmartPtr< Editor::InstanceSet > InstanceSetPtr;
  typedef std::map< tstring, InstanceSetPtr > M_InstanceSetSmartPtr;
}
