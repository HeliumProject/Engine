#include "Precompile.h"
#include "InstancePanel.h"
#include "Instance.h"
#include "Volume.h"

#include "Editor/ClassChooserDlg.h"
#include "Inspect/Choice.h"
#include "Inspect/Button.h"

#include "Attribute/AttributeHandle.h"
#include "Content/RuntimeDataAttribute.h"
#include "Console/Console.h"
#include "Symbol/Inheritance.h"

#include <algorithm>

using namespace Luna;

InstancePanel::InstancePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
, m_RuntimeClassesChoice (NULL)
, m_RuntimeClassesButton (NULL)
, m_RuntimeDataPanel (NULL)
, m_AllInstancesHaveRuntimeData (false)
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

  CreateRuntimeData();

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
  
  bool discoveredBlocker = false;
  bool allVolumes = true;

  OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator end = m_Selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::Instance* instance = Reflect::ObjectCast<Luna::Instance>(*itr);
    if(instance && instance->GetIsWeatherBlocker())
    {
      discoveredBlocker = true;
    }

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

  if(discoveredBlocker)
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Skip Particles");

      m_Enumerator->AddCheckBox<Luna::Instance, bool>( m_Selection, &Luna::Instance::GetSkipParticles, &Luna::Instance::SetSkipParticles );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Border Size");

      Inspect::Slider* slider = m_Enumerator->AddSlider<Luna::Instance, float>(m_Selection, &Luna::Instance::GetBorderSize, &Luna::Instance::SetBorderSize );
      slider->SetRangeMin( 0.0f );
      slider->SetRangeMax( 4.0f );
    }
    m_Enumerator->Pop();
  }

}

void InstancePanel::CreateRuntimeData()
{
  RectifyRuntimeData();
  
  m_Enumerator->PushPanel("Runtime Class");

  m_Enumerator->PushContainer();
  {
    // Controls for changing the runtime class (only enabled if the instance has a runtime data attribute)
    m_Enumerator->AddLabel( "Runtime Class" );

    m_RuntimeClassesChoice = m_Enumerator->AddChoice< Luna::Instance, std::string >( m_Selection, &Luna::Instance::GetRuntimeClassName, &Luna::Instance::SetRuntimeClassName );
    m_RuntimeClassesChoice->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &InstancePanel::OnRuntimeClassChanged ) );

    m_RuntimeClassesButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &InstancePanel::OnRuntimeClassButton ) );
    m_RuntimeClassesButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &InstancePanel::OnRuntimeClassChanged ) );

    RefreshClassPicker(); 
    
  }
  m_Enumerator->Pop();

  m_RuntimeDataPanel = m_Enumerator->PushPanel("Runtime Data");
  {
    m_SymbolInterpreter = m_Enumerator->CreateInterpreter<Inspect::SymbolInterpreter> ( m_RuntimeDataPanel );

    RefreshInstances(false);
  }
  m_Enumerator->Pop();
  m_Enumerator->Pop(); 

}

void InstancePanel::RefreshClassPicker()
{
  S_string runtimeClassNames; 

  GetSelectableClasses(runtimeClassNames); 
  
  Inspect::V_Item items;

  if ( runtimeClassNames.empty() )
  {
    items.resize( 1 );
    items[0].m_Key = "<NONE>";
    items[0].m_Data = "";
  }
  else
  {
    items.resize( runtimeClassNames.size() );
    S_string::const_iterator itr = runtimeClassNames.begin();
    S_string::const_iterator end = runtimeClassNames.end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
      Inspect::Item& item = items[index];
      item.m_Key = *itr;
      item.m_Data = *itr;
    }
  }

  m_RuntimeClassesChoice->SetItems( items );
  m_RuntimeClassesChoice->SetEnabled( m_EnableClassPicker );
  m_RuntimeClassesButton->SetEnabled( m_EnableClassBrowser );
}

void InstancePanel::RectifyRuntimeData()
{
  static Platform::Mutex mutex;
  Platform::TakeMutex taken (mutex);
  
  m_AllInstancesHaveRuntimeData = true; 

  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::Instance* instance = Reflect::ObjectCast< Luna::Instance >( *selItr );
    if ( instance )
    {
      Content::InstancePtr content = instance->GetPackage< Content::Instance >();

      // make runtime data current
      m_AllInstancesHaveRuntimeData &= content->RectifyRuntimeData();
    }
  }

  m_EnableClassPicker       = true; 
  m_EnableClassBrowser      = true; 
  m_EnableSymbolInterpreter = m_AllInstancesHaveRuntimeData; 

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

void InstancePanel::GetSelectableClasses( S_string& classList )
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();

  bool seenInstance = false; 
  classList.clear(); 
  
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::Instance* instance = Reflect::ObjectCast< Luna::Instance >( *selItr );
    if ( !instance )
    {
      continue;
    }

    Content::InstancePtr content = instance->GetPackage< Content::Instance >();

    S_string instanceClasses; 

    V_string classes;
    content->GetValidRuntimeClasses(classes);

    V_string::const_iterator validItr = classes.begin();
    V_string::const_iterator validEnd = classes.end();
    for ( ; validItr != validEnd; ++validItr )
    {
      const std::string& currentValidClass = *validItr;

      if ( Symbol::Inheritance::IsClass( currentValidClass ) )
      {
        if(m_RecurseSelectableClasses)
        {
          RecurseAddDerivedClasses( currentValidClass, instanceClasses );
        }
        else
        {
          instanceClasses.insert(currentValidClass); 
        }
      }
      else
      {
        Console::Warning( "%s is no longer a valid runtime class.\n", currentValidClass.c_str() );
      }
    }

    if(!seenInstance)
    {
      classList = instanceClasses; 
      seenInstance = true; 
    }
    else
    {
      Intersect(classList, instanceClasses); 
    }
  }
}


void InstancePanel::GetSelectableBaseClasses( S_string& classList )
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
  
  bool seenInstance = false; 
  classList.clear(); 

  for ( ; selItr != selEnd; ++selItr )
  {
    
    Luna::Instance* instance = Reflect::ObjectCast< Luna::Instance >( *selItr );
    if (!instance)
    {
      continue; 
    }

    Content::InstancePtr content = instance->GetPackage< Content::Instance >();

    S_string instanceClasses; 
    V_string classes;

    content->GetValidRuntimeClasses(classes);
    V_string::const_iterator validItr = classes.begin();
    V_string::const_iterator validEnd = classes.end();
    for ( ; validItr != validEnd; ++validItr )
    {
      const std::string& currentValidClass = *validItr;

      if ( Symbol::Inheritance::IsClass( currentValidClass ) )
      {
        instanceClasses.insert(currentValidClass); 
      }
      else
      {
        Console::Warning( "%s is no longer a valid runtime class.\n", currentValidClass.c_str() );
      }
    }

    if(!seenInstance)
    {
      classList = instanceClasses; 
      seenInstance = true; 
    }
    else
    {
      Intersect(classList, instanceClasses); 
    }
  }
}

void InstancePanel::GetCurrentSelectedClass( std::string& selectedClass )
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();

  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::Instance* instance = Reflect::ObjectCast< Luna::Instance >( *selItr );
    if (!instance )
    {
      continue; 
    }

    Content::InstancePtr content = instance->GetPackage< Content::Instance >();

    if(selItr == m_Selection.Begin())
    {
      selectedClass = content->GetRuntimeClass(); 
    }
    else
    {
      if(selectedClass != content->GetRuntimeClass())
      {
        selectedClass = ""; 
        break; 
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// Static helper function to recursively populate classList with baseClass and
// all of the derived classes of baseClass.
// 
void InstancePanel::RecurseAddDerivedClasses( const std::string& baseClass, S_string& classList )
{
  classList.insert( baseClass );

  V_string derivedClasses;
  Symbol::Inheritance::GetDerived( baseClass, derivedClasses );
  V_string::const_iterator derivedItr = derivedClasses.begin();
  V_string::const_iterator derivedEnd = derivedClasses.end();
  for ( ; derivedItr != derivedEnd; ++derivedItr )
  {
    const std::string& derivedClass = *derivedItr;
    RecurseAddDerivedClasses( derivedClass, classList );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the UI button to change the runtime class.
// Pops up a dialog that shows the available classes and lets the user choose 
// one.
// 
void InstancePanel::OnRuntimeClassButton( Inspect::Button* button )
{
  S_string validBaseClasses;
  std::string currentSelectedClass;

  // Fetch the client data off the button to populate the class chooser dialog
  GetSelectableBaseClasses(validBaseClasses); 
  GetCurrentSelectedClass(currentSelectedClass); 
  
  if ( !validBaseClasses.empty() )
  {
    // Create the dialog and set the inital value in the tree to match what is displayed
    // in the combo box.
    ClassChooserDlg classChooserDlg( button->GetCanvas()->GetControl(), validBaseClasses );
    classChooserDlg.SetSelectedClass( currentSelectedClass );
    if ( classChooserDlg.ShowModal() == wxID_OK )
    {
      // The user clicked OK to change the runtime class, so set the new value on all
      // selected items.
      std::string runtimeClass = classChooserDlg.GetSelectedClass();
      if ( !runtimeClass.empty() )
      {
        OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
        OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
        for ( ; selItr != selEnd; ++selItr )
        {
          Luna::Instance* instance = Reflect::ObjectCast< Luna::Instance >( *selItr );
          if ( instance )
          {
            instance->SetRuntimeClassName( runtimeClass );
          }
        }
        
        
        RefreshInstances(true);
        m_RuntimeClassesChoice->Read(); 
        m_RuntimeDataPanel->GetCanvas()->Read(); 

      }
    }
  }
}

void InstancePanel::OnRuntimeClassChanged( const Inspect::ChangeArgs& args )
{
  RectifyRuntimeData(); 

  RefreshInstances(true);
}

void InstancePanel::RefreshInstances( bool layout )
{
  m_SymbolInterpreter->Reset();

  if(m_EnableSymbolInterpreter)
  {
    std::vector< Symbol::VarInstance* > varInstances;
    OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.End();
    for ( ; itr != end; ++itr )
    {
      Content::InstancePtr instance = Reflect::AssertCast< Luna::Instance >( *itr )->GetPackage< Content::Instance >();

      Attribute::AttributeViewer< Content::RuntimeDataAttribute > runtimeDataAttribute ( instance );

      Content::RuntimeData* runtimeData = runtimeDataAttribute.Valid() ? runtimeDataAttribute->GetRuntimeData() : NULL;

      Symbol::UDTInstancePtr runtimeInstance = runtimeData ? runtimeData->GetRuntimeInstance() : NULL; 

      // this should be true here if we were in the m_Selection list and hasRuntimeData is true
      // depending on how events fall out, this *could* be null

      if(runtimeInstance)
      {
        // queue to be Reflected upon
        varInstances.push_back(runtimeInstance); 
      }
      else
      {
        varInstances.clear(); 
        break; 
      }
    }

    if (!varInstances.empty())
    {
      m_SymbolInterpreter->Interpret( varInstances );
    }

    if ( m_RuntimeDataPanel->IsRealized() )
    {
      m_RuntimeDataPanel->RefreshControls();
    }

    m_RuntimeDataPanel->SetEnabled( !varInstances.empty() );
    m_RuntimeDataPanel->SetExpanded( !varInstances.empty() );
  }

  // do this outside of the if so that when we change from
  // enabled to not enabled, we'll adjust the size of the panel
  //
  if (layout)
  {
    m_RuntimeDataPanel->GetCanvas()->Layout();
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
