#pragma once

#include "Attribute/Attribute.h"
#include "Attribute/AttributeCollection.h"
#include "Core/Persistent.h"
#include "Application/Inspect/Widgets/Group.h"
#include "InspectReflect/ReflectInterpreter.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Template class to display the members of an attribute in a panel.  The 
  // template parameter specifies what type of attribute to show.  Uses the
  // Inspect::ReflectInterpreter to display the members.
  // 
  template < class T >
  class AttributeGroup : public Inspect::Group
  {
  protected:
    typedef Nocturnal::SmartPtr< T > TPtr;

  protected:
    Inspect::ReflectInterpreterPtr m_ReflectInterpreter;
    OS_SelectableDumbPtr m_Selection;

  public:
    ///////////////////////////////////////////////////////////////////////////
    // Constructor
    // 
    AttributeGroup( Inspect::Interpreter* interpreter, const OS_SelectableDumbPtr& selection )
      : m_Selection( selection )
    {
      m_Interpreter = interpreter;
      DisableBorder();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Destructor
    // 
    virtual ~AttributeGroup()
    {
      if ( m_ReflectInterpreter )
      {
        m_ReflectInterpreter->Reset();
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Creates the control for the first time.
    // 
    virtual void Create() NOC_OVERRIDE
    {
      m_ReflectInterpreter = m_Interpreter->CreateInterpreter< Inspect::ReflectInterpreter >( this );

      __super::Create();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Reinterprets the members for all items in the selection list that have
    // the specified attribute.
    // 
    void Refresh()
    {
      m_ReflectInterpreter->Reset();

      std::vector< Reflect::Element* > instances;
      instances.reserve( m_Selection.Size() );

      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Persistent* persistent = Reflect::ObjectCast< Persistent >( *itr );
        if ( persistent )
        {
          Attribute::AttributeCollection* collection = persistent->GetPackage< Attribute::AttributeCollection >();
          if ( collection )
          {
            TPtr attrib = collection->GetAttribute< T >();
            if ( attrib.ReferencesObject() )
            {
              instances.push_back( attrib.Ptr() );
            }
          }
        }
      }

      if ( !instances.empty() )
      {
        m_ReflectInterpreter->Interpret( instances );
      }
    }
  };
}
