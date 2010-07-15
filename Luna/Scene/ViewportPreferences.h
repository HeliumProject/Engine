#pragma once

#include "Foundation/Reflect/Element.h"
#include "CameraPreferences.h"
#include "Viewport.h"

namespace Luna
{
  namespace ViewColorModes
  {
    enum ViewColorMode
    {
      Layer,
      NodeType,
      Scene,
      AssetType,
      Scale,
      ScaleGradient,
    };
    static void ViewColorModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement( Layer, TXT( "Layer" ) );
      info->AddElement( NodeType, TXT( "NodeType" ), TXT( "Node Type" ) );
      info->AddElement( Scene, TXT( "Scene" ) );
      info->AddElement( AssetType, TXT( "Engine Type" ) );
      info->AddElement( Scale, TXT( "Scale" ) );
      info->AddElement( ScaleGradient, TXT( "Scale (Gradient)" ) );
    }
  }
  typedef ViewColorModes::ViewColorMode ViewColorMode;

  class ViewportPreferences : public Reflect::Element
  {
  public: 
    ViewportPreferences(); 

    void ApplyToView(Luna::Viewport* view); 
    void LoadFromView(Luna::Viewport* view); 

  private: 
    CameraMode            m_CameraMode; 
    GeometryMode          m_GeometryMode; 
    V_CameraPreferences   m_CameraPrefs; // do not use m_CameraMode as an index!
    ViewColorMode         m_ColorMode;

    bool m_Highlighting; 
    bool m_AxesVisible; 
    bool m_GridVisible; 
    bool m_BoundsVisible; 
    bool m_StatisticsVisible; 

  public:
    REFLECT_DECLARE_CLASS(ViewportPreferences, Reflect::Element);
    static void EnumerateClass( Reflect::Compositor<ViewportPreferences>& comp );

    ViewColorMode GetColorMode() const;
    void SetColorMode( ViewColorMode mode );
    const Reflect::Field* ColorModeField() const;
  }; 

  typedef Nocturnal::SmartPtr<ViewportPreferences> ViewPreferencesPtr; 

}
