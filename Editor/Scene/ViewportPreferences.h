#pragma once

#include "Foundation/Reflect/Element.h"
#include "CameraPreferences.h"
#include "Viewport.h"

namespace Editor
{
  namespace ViewColorModes
  {
    enum ViewColorMode
    {
      Layer,
      Type,
      Scene,
    };
    static void ViewColorModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement( Layer, TXT( "Layer" ) );
      info->AddElement( Type, TXT( "Type" ) );
      info->AddElement( Scene, TXT( "Scene" ) );
    }
  }
  typedef ViewColorModes::ViewColorMode ViewColorMode;

  class ViewportPreferences : public Reflect::ConcreteInheritor< ViewportPreferences, Reflect::Element >
  {
  public: 
    ViewportPreferences(); 

    void ApplyToViewport(Editor::Viewport* view); 
    void LoadFromViewport(Editor::Viewport* view); 

    ViewColorMode GetColorMode() const;
    void SetColorMode( ViewColorMode mode );
    const Reflect::Field* ColorModeField() const;

  private: 
    CameraMode           m_CameraMode; 
    GeometryMode         m_GeometryMode; 
    V_CameraPreferences  m_CameraPrefs; // do not use m_CameraMode as an index!
    ViewColorMode        m_ColorMode;

    bool                 m_Highlighting; 
    bool                 m_AxesVisible; 
    bool                 m_GridVisible; 
    bool                 m_BoundsVisible; 
    bool                 m_StatisticsVisible; 

  public:
    static void EnumerateClass( Reflect::Compositor<ViewportPreferences>& comp )
    {
        comp.AddEnumerationField( &ViewportPreferences::m_CameraMode, "m_CameraMode" );
        comp.AddEnumerationField( &ViewportPreferences::m_GeometryMode, "m_GeometryMode" );
        comp.AddField( &ViewportPreferences::m_CameraPrefs, "m_CameraPrefs" );
        comp.AddEnumerationField( &ViewportPreferences::m_ColorMode, "m_ColorMode" );

        comp.AddField( &ViewportPreferences::m_Highlighting, "m_Highlighting" );
        comp.AddField( &ViewportPreferences::m_AxesVisible, "m_AxesVisible" );
        comp.AddField( &ViewportPreferences::m_GridVisible, "m_GridVisible" );
        comp.AddField( &ViewportPreferences::m_BoundsVisible, "m_BoundsVisible" );
        comp.AddField( &ViewportPreferences::m_StatisticsVisible, "m_StatisticsVisible" );
    }
  }; 

  typedef Helium::SmartPtr<ViewportPreferences> ViewportPreferencesPtr; 
}
