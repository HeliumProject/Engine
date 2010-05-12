#pragma once

#include "Attribute.h"

namespace Asset
{
  //
  // Yes, this is very ugly.  Chris is going to add support for an editable int map in Luna and then we can replace this.
  //

  class ASSET_API BangleRemapAttribute : public Attribute
  {
  public:
    M_i32 m_BangleRemap;
    bool  m_DiscardMainGeom;

    BangleRemapAttribute()
      : m_DiscardMainGeom( false )
    {
    }

    REFLECT_DECLARE_CLASS( BangleRemapAttribute, Attribute );

    static void EnumerateClass( Reflect::Compositor<BangleRemapAttribute>& comp );

    virtual AttributeUsage GetAttributeUsage() const override;
    virtual AttributeCategoryType GetCategoryType() const override;
  };

  typedef Nocturnal::SmartPtr< BangleRemapAttribute > BangleRemapAttributePtr;
}