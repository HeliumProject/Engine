#pragma once

//
// STL includes
//

#include <map>
#include <vector>
#include <string>

//
// Library includes
//

#include "Platform/Assert.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Memory/SmartPtr.h"
#include "UID/GUID.h"
#include "UID/TUID.h"
#include "Foundation/Atomic.h"

//
// API includes
//

#include "Object.h"
#include "Archive.h"
#include "Registry.h"

namespace Reflect
{
  class Field;
  class Class;
  class Structure;

  class Archive;
  namespace ArchiveTypes
  {
    enum ArchiveType;
  }

  //
  // Event delegate to support getting notified if this element changes
  //

  struct ElementChangeArgs
  {
    const Element* m_Element;
    const Field* m_Field;

    ElementChangeArgs(const Element* element, const Field* field)
      : m_Element (element)
      , m_Field (field)
    {

    }
  };
  typedef Nocturnal::Signature<void, const ElementChangeArgs&, Foundation::AtomicRefCountBase> ElementChangeSignature;


  //
  // Reflect::Element is the NOC_ABSTRACT base class of a serializable unit
  //

  class REFLECT_API Element NOC_ABSTRACT : public AbstractInheritor<Element, Object>
  {
  protected:
    Element ();

  public:
    // RTTI prototypes
    static void EnumerateClass( Reflect::Compositor<Element>& comp );

    // Returns the string to use as the title of this element in UI (uses the UI name of the type info by default)
    virtual const std::string&  GetTitle() const { return GetClass()->m_UIName; }

    // Specifies if the value is directly between the start and end short name
    virtual bool                IsCompact() const { return false; }

    // This the process callback for sub and primitive elements to have thier data be aggregated into the parent instance
    virtual bool                ProcessComponent(ElementPtr element, const std::string& fieldName);


    //
    // Serialization
    //

  public:
    // Serialize to a particular data target, just works on this
    void                        ToXML (std::string& xml) const;
    void                        ToStream (std::iostream& stream, ArchiveType type) const;
    void                        ToFile (const std::string& file, const VersionPtr& version = NULL) const;

    // Callbacks are executed at the appropriate time by the archive and cloning APIs
    virtual void                PreSerialize() { }
    virtual void                PostSerialize() { }
    virtual void                PreDeserialize() { }
    virtual void                PostDeserialize() { }


    //
    // Introspection
    //

  public:
    // Visitor introspection support, should never ever change an object (but the visitor may)
    virtual void                Host(Visitor& visitor);

    // Do comparison logic against other object, checks type and field data
    virtual bool                Equals(const ElementPtr& rhs) const;

    // Deep copy this object into the specified object.
    virtual void                CopyTo(const ElementPtr& destination);

    // Deep copy this object into a new object, this is not const because derived classes may need to do work before cloning
    virtual ElementPtr          Clone();


    //
    // Mutation
    //

  private:
    mutable ElementChangeSignature::Event m_Changed;
  public:
    void AddChangedListener(const ElementChangeSignature::Delegate& d) const
    {
      m_Changed.Add(d);
    }
    void RemoveChangedListener(const ElementChangeSignature::Delegate& d) const
    {
      m_Changed.Remove(d);
    }
    virtual void RaiseChanged(const Field* field = NULL) const
    {
      m_Changed.Raise( ElementChangeArgs (this, field) );
    }
  };
}
