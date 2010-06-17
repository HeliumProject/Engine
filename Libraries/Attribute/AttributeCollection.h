#pragma once

#include "API.h"

#include <hash_map>

#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

#include "Attribute.h"

namespace Attribute
{
  class ATTRIBUTE_API AttributeBase;
  class ATTRIBUTE_API AttributeCollection;

  struct AttributeCollectionChanged
  {
    AttributeCollectionChanged(const AttributeCollection* collection, const AttributeBase* attribute)
      : m_Collection(collection)
      , m_Attribute(attribute)
    {
      
    }

    const AttributeCollection*  m_Collection;
    const AttributeBase*        m_Attribute;
  };

  typedef Nocturnal::Signature<void, const AttributeCollectionChanged&> AttributeCollectionChangedSignature;

  typedef std::map<Reflect::TypeID, AttributePtr> M_Attribute;
   
  typedef Nocturnal::SmartPtr<AttributeCollection> AttributeCollectionPtr;

  class ATTRIBUTE_API AttributeCollection : public Reflect::Element
  {
  public:
      REFLECT_DECLARE_CLASS(AttributeCollection, Reflect::Element);
    static void EnumerateClass( Reflect::Compositor<AttributeCollection>& comp );

    AttributeCollection();
    AttributeCollection( const AttributePtr& attr );
    virtual ~AttributeCollection(); 


    //
    // Attribute Management
    //

    // clear all attributes
    virtual void Clear();

    // get all attributes
    const M_Attribute& GetAttributes() const;

    // retrieve attribute from a slot
    virtual const AttributePtr& GetAttribute(i32 slotID) const;

    // casting helper will get you what you need, baby ;)
    template <class T>
    Nocturnal::SmartPtr<T> GetAttribute() const
    {
      return Reflect::ObjectCast<T>( GetAttribute( Reflect::GetType<T>() ) );
    }

    // template helper function for removing by type... 
    template <class T>
    void RemoveAttribute()
    {
      return RemoveAttribute( Reflect::GetType<T>() ); 
    }
      
    // Set attribute into a slot.  If validate param is false, ValidateAttribute
    // will not be called, and it is up to the caller to do any necessary
    // checking, including for duplicate attributes.
    virtual void SetAttribute(const AttributePtr& attr, bool validate = true );

    // remove attribute from a slot
    virtual void RemoveAttribute(i32 slotID);

    // queries the container for the existence of the specified attribute
    virtual bool ContainsAttribute(i32 slotID) const;

    // Validates the attribute for add to this attribute collection.  If the addition is
    // not valid, the return value will be false and "error" will have additional info about
    // the problem.  The steps carried out by this function are:
    // 1. Makes sure the attribute is not already in this collection.
    // 2. Gives derived classes the chance to NOC_OVERRIDE ValidateCompatible.
    // 3. Iterates over each attribute already in the collection and calls ValidateSibling.
    virtual bool ValidateAttribute( const AttributePtr &attr, std::string& error ) const;

    // Basic implementation just checks the behavior of the attribute, but you can NOC_OVERRIDE
    // this to allow exclusive attributes, or prohibit inclusive attributes
    virtual bool ValidateCompatible( const AttributePtr& attr, std::string& error ) const;

    // Basic implementation just allows the attribute to be persisted, but you can NOC_OVERRIDE
    // this to avoid persisting attributes that are at a redundant state (as an example)
    virtual bool ValidatePersistent( const AttributePtr& attr ) const;

    // Comparision API (for AssetType classification) -- IDEALLY SHOULD GO AWAY!
    bool IsSubset( const AttributeCollection* collection ) const;


    //
    // Change API
    //

  public:
    // this is called by AttributeHandle when an attribute changes, or by code at large
    virtual void AttributeChanged( const AttributeBase* attr = NULL );

  protected:
    // this is a callback called by elements being changed by procedurally generated UI (LunaProperties)
    void AttributeChanged( const Reflect::ElementChangeArgs& args )
    {
      // call into the virtual prototype in case it gets overridden in a derived class
      AttributeChanged( Reflect::ConstAssertCast<AttributeBase>(args.m_Element) );
    }

  
    //
    // Events
    //

  protected:
    AttributeCollectionChangedSignature::Event m_SingleAttributeChanged; 
  public:
    void AddAttributeChangedListener(const AttributeCollectionChangedSignature::Delegate& d)
    {
      m_SingleAttributeChanged.Add(d);
    }
    void RemoveAttributeChangedListener(const AttributeCollectionChangedSignature::Delegate& d)
    {
      m_SingleAttributeChanged.Remove(d);
    }

  protected:
    AttributeCollectionChangedSignature::Event m_AttributeAdded; 
  public: 
    void AddAttributeAddedListener(const AttributeCollectionChangedSignature::Delegate& d)
    {
      m_AttributeAdded.Add(d); 
    }

    void RemoveAttributeAddedListener(const AttributeCollectionChangedSignature::Delegate& d)
    {
      m_AttributeAdded.Remove(d); 
    }

  protected:
    AttributeCollectionChangedSignature::Event m_AttributeRemoved; 
  public: 
    void AddAttributeRemovedListener(const AttributeCollectionChangedSignature::Delegate& d)
    {
      m_AttributeRemoved.Add(d); 
    }

    void RemoveAttributeRemovedListener(const AttributeCollectionChangedSignature::Delegate& d)
    {
      m_AttributeRemoved.Remove(d); 
    }


    //
    // Element overrides
    //

  public:
    // migrate legacy attributes
    virtual bool ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName) NOC_OVERRIDE;

    // setup changed callback
    virtual void PreSerialize() NOC_OVERRIDE;
    virtual void PostDeserialize() NOC_OVERRIDE;

    // copy all attributes from one collection to another
    virtual void CopyTo(const Reflect::ElementPtr& destination) NOC_OVERRIDE;

    // helper function for CopyTo
    bool CopyAttributeTo( const AttributeCollectionPtr& destCollection, const AttributePtr& destAttrib, const AttributePtr& srcAttrib );


    //
    // Members
    //

  protected:
    // indicates if the attribute collection has been modified in memory
    bool m_Modified;

  private:
    M_Attribute m_Attributes;
  };
}