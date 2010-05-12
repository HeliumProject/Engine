#include "Precompile.h"
#include "LightingVolumePanel.h"

#include "AttributeOverrideGroup.h"
#include "LightingVolume.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Editor/SessionManager.h"

#include "Asset/LevelAsset.h"
#include "Asset/CubeMapAttribute.h"
#include "math/Color3.h"

#include "Content/SunShadowMergeAttribute.h"
#include "Content/GlossControlAttribute.h"
#include "Content/GroundLightAttribute.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
//
LightingVolumePanel::LightingVolumePanel( Enumerator* enumerator, const OS_SelectableDumbPtr& selection )
: m_Selection( selection )
{
  m_Interpreter = m_Enumerator = enumerator;
  m_Text = "LightingVolume";
  m_Expanded = true;
}

///////////////////////////////////////////////////////////////////////////////
template <class T>
void AddUIElement(Inspect::ReflectInterpreterPtr& reflectInterpreter, Attribute::AttributeCollection* collection)
{
  Nocturnal::SmartPtr<T> shadowAttr = collection->GetAttribute< T >();
  if(shadowAttr)
  {
    Attribute::AttributeViewer< T >   attributeViewer ( collection );
    std::vector< Reflect::Element* >  elements;
    bool  expandPanel = attributeViewer.Valid();
    elements.push_back( shadowAttr );
    reflectInterpreter->Interpret( elements, 0xFFFFFFFF, 0x0, expandPanel);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Creates the panel.
//
void LightingVolumePanel::Create()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel("Exclude Global Lights");
    m_Enumerator->AddCheckBox<Luna::LightingVolume, bool>(m_Selection, &Luna::LightingVolume::GetExcludeGlobalLights, &Luna::LightingVolume::SetExcludeGlobalLights);
  }
  m_Enumerator->Pop();

  //Create the interpreter
  m_ReflectInterpreter  = m_Enumerator->CreateInterpreter< Inspect::ReflectInterpreter >( this );

  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();

  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::Persistent* persistent = Reflect::ObjectCast< Luna::Persistent >( *selItr );

    if ( persistent )
    {
      Attribute::AttributeCollection* collection = persistent->GetPackage< Attribute::AttributeCollection >();
      if ( collection )
      {
        AddUIElement<Content::SunShadowMergeAttribute>(m_ReflectInterpreter, collection);
        AddUIElement<Content::GlossControlAttribute>(m_ReflectInterpreter, collection);
        AddUIElement<Content::GroundLightAttribute>(m_ReflectInterpreter, collection);
//        AddUIElement<Content::RimLightAttribute>(m_ReflectInterpreter, collection);
        AddUIElement<Asset::CubeMapAttribute>(m_ReflectInterpreter, collection);
      }
    }
  }

  Inspect::Panel::Create();
}