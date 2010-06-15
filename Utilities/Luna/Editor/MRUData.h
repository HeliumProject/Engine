#pragma once

#include "Luna/API.h"
#include "Luna/UI/MRU.h"
#include "Reflect/Element.h"
#include "Reflect/ArraySerializer.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // 
  // 
  class LUNA_EDITOR_API MRUData : public Reflect::Element
  {
  private:
    V_string m_Paths;
    //TODO: V_string m_Icons;

    // RTTI
  public:
    REFLECT_DECLARE_CLASS( MRUData, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<MRUData>& comp );

  public:
    static void InitializeType();
    static void CleanupType();

    MRUData();
    virtual ~MRUData();

    const V_string& GetPaths() const;
    void SetPaths( const V_string& paths );
  };
  typedef Nocturnal::SmartPtr< MRUData > MRUDataPtr;
}