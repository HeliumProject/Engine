#pragma once

#include "View.h"
#include "Core/Object.h"

namespace Luna
{
  class Instance;
  typedef std::set< Luna::Instance* > S_InstanceDumbPtr;

  class InstanceSet;

  class InstanceType;

  struct InstanceSetChangeArgs
  {
    Luna::InstanceSet* m_InstanceSet;
    Luna::Instance* m_Instance;

    InstanceSetChangeArgs( Luna::InstanceSet* set, Luna::Instance* entity )
    : m_InstanceSet( set )
    , m_Instance( entity )
    {
    }
  };

  typedef Nocturnal::Signature< void, const InstanceSetChangeArgs& > InstanceSetChangeSignature;

  class InstanceSet : public Object
  {
    //
    // Members
    //

  protected:
    Luna::InstanceType* m_Type;
    S_InstanceDumbPtr m_Instances;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::InstanceSet, Object );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    InstanceSet(Luna::InstanceType* type);

    virtual ~InstanceSet();

    Luna::InstanceType* GetInstanceType();

    virtual void AddInstance(Luna::Instance* i);

    virtual void RemoveInstance(Luna::Instance* i);

    virtual bool ContainsInstance(Luna::Instance* i);

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

  typedef Nocturnal::SmartPtr< Luna::InstanceSet > InstanceSetPtr;
  typedef std::map< tstring, InstanceSetPtr > M_InstanceSetSmartPtr;
}
