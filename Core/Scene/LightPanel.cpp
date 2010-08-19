/*#include "Precompile.h"*/
#include "LightPanel.h"

#include "Light.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"

#include "Core/Asset/Classes/SceneAsset.h"
#include "Foundation/Math/Color3.h"

#include "Foundation/Component/ComponentHandle.h"

using namespace Helium;
using namespace Helium::Core;

///////////////////////////////////////////////////////////////////////////////
// Constructor
//
LightPanel::LightPanel( PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection )
: m_Selection( selection )
{
  m_Interpreter = m_Generator = generator;
  m_Name = TXT( "Light" );
  m_Expanded = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Creates the panel.
//
void LightPanel::Create()
{  
  //Create the interpreter
  m_ReflectInterpreter  = m_Generator->CreateInterpreter< Inspect::ReflectInterpreter >( this );
  Inspect::Panel::Create();
}