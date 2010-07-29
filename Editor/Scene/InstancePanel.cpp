#include "Precompile.h"
#include "InstancePanel.h"
#include "Instance.h"
#include "Volume.h"

#include "Application/Inspect/Controls/Choice.h"
#include "Application/Inspect/Controls/InspectButton.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Editor;

InstancePanel::InstancePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
, m_EnableClassPicker (false)
, m_EnableClassBrowser (false)
, m_EnableSymbolInterpreter (false)
, m_RecurseSelectableClasses (true )
{
  m_Interpreter = m_Generator = generator;
  m_Expanded = true;
  m_Text = TXT( "Instance" );
}

void InstancePanel::Create()
{
  CreateAppearanceFlags();

  Inspect::Panel::Create();
}

void InstancePanel::CreateAppearanceFlags()
{
  m_Generator->PushContainer();
  {
    m_Generator->AddLabel( TXT( "Solid" ) );
    
    m_SolidOverride = m_Generator->AddCheckBox<Editor::Instance, bool>( m_Selection, &Editor::Instance::GetSolidOverride, &Editor::Instance::SetSolidOverride );

    m_Solid = m_Generator->AddCheckBox<Editor::Instance, bool>( m_Selection, &Editor::Instance::GetSolid, &Editor::Instance::SetSolid );
    m_Solid->Read();

    m_SolidOverride->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &InstancePanel::OnSolidOverride ) );
    m_SolidOverride->RaiseBoundDataChanged();
  }
  m_Generator->Pop();

  m_Generator->PushContainer();
  {
    m_Generator->AddLabel( TXT( "Transparent" ) );
    
    m_TransparentOverride = m_Generator->AddCheckBox<Editor::Instance, bool>( m_Selection, &Editor::Instance::GetTransparentOverride, &Editor::Instance::SetTransparentOverride );
    
    m_Transparent = m_Generator->AddCheckBox<Editor::Instance, bool>( m_Selection, &Editor::Instance::GetTransparent, &Editor::Instance::SetTransparent );
    m_Transparent->Read();

    m_TransparentOverride->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &InstancePanel::OnTransparentOverride ) );
    m_TransparentOverride->RaiseBoundDataChanged();
  }
  m_Generator->Pop();
  
  bool allVolumes = true;

  OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator end = m_Selection.End();
  for ( ; itr != end; ++itr )
  {
    Editor::VolumePtr volume = Reflect::ObjectCast< Volume >( *itr );
    if ( !volume )
    {
      allVolumes = false;
    }
  }

  if ( allVolumes )
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Show Pointer" ) );
      m_Generator->AddCheckBox<Editor::Volume, bool>( m_Selection, &Editor::Volume::IsPointerVisible, &Editor::Volume::SetPointerVisible );
    }
    m_Generator->Pop();
  }
}

void InstancePanel::Intersect(std::set< tstring >& intersection, const std::set< tstring >& classList)
{
  for(std::set< tstring >::iterator it = intersection.begin(); it != intersection.end(); )
  {
    std::set< tstring >::const_iterator input = std::find(classList.begin(), classList.end(), *it); 
    if(input == classList.end())
    {
      it = intersection.erase(it); 
    }
    else
    {
      ++it; 
    }
  }
}


void InstancePanel::OnSolidOverride( const Inspect::ChangeArgs& args )
{
  m_Solid->SetEnabled( m_SolidOverride->GetChecked() );
  m_Solid->Read();
}

void InstancePanel::OnTransparentOverride( const Inspect::ChangeArgs& args )
{
  m_Transparent->SetEnabled( m_TransparentOverride->GetChecked() );
  m_Transparent->Read();
}
