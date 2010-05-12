#pragma once

#include "API.h"
#include "EntityClass.h"

namespace Entity
{
  class ENTITY_API AnimationControlAttribute : public Asset::Attribute
  {
  public:
    AnimationControlAttribute() {INITIALIZE_CLASS( AnimationControlAttribute ); }

    i32 m_TranslationCompressionScale;

    DECLARE_ELEMENT( AnimationControlAttribute );

    BEGIN_UDT_MEMBERS( AnimationControlAttribute );

    ADD_SERIALIZER( m_TranslationCompressionScale, RF::I32Serializer::GetTypeID() );
    ADD_DEFAULT( RF::I32Serializer, 11 );

    END_UDT_MEMBERS();

    virtual u32 GetAttributeUsage() const;
  };

  typedef IG::SmartPtr< AnimationControlAttribute > AnimationControlAttributePtr;
}