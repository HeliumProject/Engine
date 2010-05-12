#pragma once

#include "API.h"
#include "AttributeCategoryTypes.h"
#include "Common/Automation/Event.h"
#include "Exceptions.h"
#include "Reflect/Element.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

namespace Attribute
{
  class AttributeCollection;

  class AttributeCategory;
  typedef Nocturnal::SmartPtr< AttributeCategory > AttributeCategoryPtr;

  namespace AttributeUsages
  {
    enum AttributeUsage
    {
      // Can exist only in a Class of Assets/Entities/etc.
      Class,

      // Can exist only in an instance of an Asset/Entity/etc.
      Instance,

      // Can exist in either an Instance or Class:
      //   o in the class its the default value for all instances of that class,
      //   o in the instance, its the overrides for that attribute for that instance
      Overridable,  
    };
  }
  typedef AttributeUsages::AttributeUsage AttributeUsage;

  namespace AttributeBehaviors
  {
    enum AttributeBehavior
    {
      // Can inherently be added to any collection.
      Inclusive,

      // Cannot inherently be added to any collection (the collection's validation
      // code must explicitly let this attribute in).
      Exclusive
    };
  }
  typedef AttributeBehaviors::AttributeBehavior AttributeBehavior;


  //
  // Abstract Base Class - Subclass this for structured attribute data you can add to a collection
  //

  class ATTRIBUTE_API AttributeBase : public Reflect::Element
  {
  private:
    REFLECT_DECLARE_ABSTRACT( AttributeBase, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AttributeBase>& comp );

  public:
    AttributeBase();
    virtual ~AttributeBase();


    // 
    // Attribute slots
    // 

    // Attributes are designed so that there can only be one of each kind in a
    // collection.  This function specifies what slot in the collection this
    // attribute should occupy.  By default, every attribute will occupy an
    // individual slot.  Derived classes can override this function to specify
    // what slot they belong to.
    virtual i32 GetSlot() const
    {
      return GetType();
    }


    //
    // Attribute Usage API
    //

    virtual AttributeUsage GetAttributeUsage() const = 0;

    virtual AttributeBehavior GetAttributeBehavior() const
    {
      return AttributeBehaviors::Exclusive;
    }


    // 
    // Categories and other UI niceties
    // 

    // Each attribute belongs to a single category (specified by GetCategoryType below).
    const AttributeCategoryPtr& GetCategory() const;

    // Derived classes must implement this function to return what category they belong to.
    virtual AttributeCategoryType GetCategoryType() const = 0;


    // 
    // Collection information
    // 

    // Get the collection we are an attribute of
    AttributeCollection* GetCollection() const;

    // Set the collection we are an attribute of
    void SetCollection(AttributeCollection* collection);

    // Check to make sure that this attribute can be added a collection with the sibling.
    // Return false if the attribute canot be added to that collection, and fill out the error 
    // string with a meaningful explanation that will be shown to the user.
    virtual bool ValidateSibling( const AttributeBase* attribute, std::string& error ) const;

  protected:
    // the collection we are an attribute of
    AttributeCollection* m_Collection;

  public:
    // should we utilized or ignored?
    bool m_Enabled;
  };

  typedef Nocturnal::SmartPtr<AttributeBase> AttributePtr;
  typedef std::vector<AttributePtr> V_Attribute;
}