#include "Precompile.h"
#include "InstancePanel.h"
#include "Instance.h"
#include "Volume.h"

#include "Application/Inspect/Widgets/Text Controls/Choice.h"
#include "Application/Inspect/Widgets/Button Controls/InspectButton.h"

#include "Attribute/AttributeHandle.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Luna;

InstancePanel::InstancePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
, m_EnableClassPicker (false)
, m_EnableClassBrowser (false)
, m_EnableSymbolInterpreter (false)
, m_RecurseSelectableClasses (true )
{
  m_Interpreter = m_Enumerator = enumerator;
  m_Expanded = true;
  m_Text = "Instance";
}

void InstancePanel::Create()
{
  CreateApplicationType();

  CreateAppearanceFlags();

  Inspect::Panel::Create();
}

void InstancePanel::CreateApplicationType()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Application Type" );
    Inspect::Choice* choice = m_Enumerator->AddChoice< Luna::Instance, std::string >( m_Selection, &Luna::Instance::GetConfiguredTypeName, &Luna::Instance::SetConfiguredTypeName );

    // this is the set of names that are compatible with all selected items
    S_string names;

    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        S_string current = Reflect::AssertCast<Luna::Instance>(*itr)->GetValidConfiguredTypeNames();
        if (itr == m_Selection.Begin())
        {
          names = current;
        }
        else
        {
          S_string::iterator namesItr = names.begin();
          S_string::iterator namesEnd = names.end();
          while ( namesItr != namesEnd )
          {
            if ( current.find( *namesItr ) == current.end() )
            {
              namesItr = names.erase( namesItr );
            }
            else
            {
              ++namesItr;
            }
          }

          if (names.empty())
          {
            break;
          }
        }
      }
    }

    {
      Inspect::V_Item items;
      items.resize( names.size() + 1 );

      items[0].m_Key = "<AUTOMATIC>";
      items[0].m_Data = "";

      S_string::const_iterator itr = names.begin();
      S_string::const_iterator end = names.end();
      for ( size_t index=1; itr != end; ++itr, ++index )
      {
        Inspect::Item& item = items[index];
        item.m_Key = *itr;
        item.m_Data = *itr;
      }

      choice->SetItems(items);
      choice->SetDropDown(true);
    }
  }
  m_Enumerator->Pop();
}

void InstancePanel::CreateAppearanceFlags()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel("Solid");
    
    m_SolidOverride = m_Enumerator->AddCheckBox<Luna::Instance, bool>( m_Selection, &Luna::Instance::GetSolidOverride, &Luna::Instance::SetSolidOverride );

    m_Solid = m_Enumerator->AddCheckBox<Luna::Instance, bool>( m_Selection, &Luna::Instance::GetSolid, &Luna::Instance::SetSolid );
    m_Solid->Read();

    m_SolidOverride->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &InstancePanel::OnSolidOverride ) );
    m_SolidOverride->RaiseBoundDataChanged();
  }
  m_Enumerator->Pop();

  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel("Transparent");
    
    m_TransparentOverride = m_Enumerator->AddCheckBox<Luna::Instance, bool>( m_Selection, &Luna::Instance::GetTransparentOverride, &Luna::Instance::SetTransparentOverride );
    
    m_Transparent = m_Enumerator->AddCheckBox<Luna::Instance, bool>( m_Selection, &Luna::Instance::GetTransparent, &Luna::Instance::SetTransparent );
    m_Transparent->Read();

    m_TransparentOverride->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &InstancePanel::OnTransparentOverride ) );
    m_TransparentOverride->RaiseBoundDataChanged();
  }
  m_Enumerator->Pop();
  
  bool allVolumes = true;

  OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator end = m_Selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::VolumePtr volume = Reflect::ObjectCast< Volume >( *itr );
    if ( !volume )
    {
      allVolumes = false;
    }
  }

  if ( allVolumes )
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Show Pointer");
      m_Enumerator->AddCheckBox<Luna::Volume, bool>( m_Selection, &Luna::Volume::IsPointerVisible, &Luna::Volume::SetPointerVisible );
    }
    m_Enumerator->Pop();
  }
}

void InstancePanel::Intersect(S_string& intersection, const S_string& classList)
{
  for(S_string::iterator it = intersection.begin(); it != intersection.end(); )
  {
    S_string::const_iterator input = std::find(classList.begin(), classList.end(), *it); 
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
