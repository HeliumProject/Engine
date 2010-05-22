#pragma once

#include "Reflect/Element.h"
#include "CameraPreferences.h"
#include "View.h"

namespace Luna
{
  namespace ViewColorModes
  {
    enum ViewColorMode
    {
      Layer,
      NodeType,
      Zone,
      AssetType,
      Scale,
      ScaleGradient,
    };
    static void ViewColorModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement( AssetType, "Engine Type" );
      info->AddElement( Layer, "Layer" );
      info->AddElement( NodeType, "NodeType", "Node Type" );
      info->AddElement( Zone, "Zone" );
      info->AddElement( Scale, "Scale" );
      info->AddElement( ScaleGradient, "Scale (Gradient)" );
    }
  }
  typedef ViewColorModes::ViewColorMode ViewColorMode;

  class ViewPreferences : public Reflect::Element
  {
  public: 
    ViewPreferences(); 

    void ApplyToView(Luna::View* view); 
    void LoadFromView(Luna::View* view); 

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
    bool m_PathfindingVisible; 

  public:
    REFLECT_DECLARE_CLASS(ViewPreferences, Reflect::Element);
    static void EnumerateClass( Reflect::Compositor<ViewPreferences>& comp );

    ViewColorMode GetColorMode() const;
    void SetColorMode( ViewColorMode mode );
    const Reflect::Field* ColorModeField() const;
  }; 

  typedef Nocturnal::SmartPtr<ViewPreferences> ViewPreferencesPtr; 

}
