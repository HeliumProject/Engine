#include "Precompile.h"
#include "RuntimeDataPanel.h"
#include "Inspect/Choice.h"
#include "Inspect/Button.h"
#include "Inspect/Action.h"
#include "Core/Enumerator.h"
#include "Editor/ClassChooserDlg.h"

#include "Symbol/Inheritance.h"

#include <algorithm>

using namespace Luna; 

RuntimeDataPanel::RuntimeDataPanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: m_Selection(selection)
{
  m_Interpreter = m_Enumerator = enumerator;
  m_Text = "Runtime Data"; 
}

void RuntimeDataPanel::Create()
{
  RectifyRuntimeData();

  m_Interpreter->PushContainer();
  {
    // Controls for changing the runtime class (only enabled if the instance has a runtime data attribute)
    m_Interpreter->AddLabel( "Runtime Class" );

    m_RuntimeClassesChoice = m_Enumerator->AddChoice< RuntimeDataAdapterBase, std::string >( m_Selection, &RuntimeDataAdapterBase::GetRuntimeClass, &RuntimeDataAdapterBase::SetRuntimeClass );
    m_RuntimeClassesChoice->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &RuntimeDataPanel::OnRuntimeClassChanged ) );

    m_RuntimeClassesButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &RuntimeDataPanel::OnRuntimeClassButton ) );
    m_RuntimeClassesButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &RuntimeDataPanel::OnRuntimeClassChanged ) );

    RefreshClassPicker(); 
    
  }
  m_Interpreter->Pop();

  m_RuntimeDataPanel = m_Interpreter->PushPanel("Runtime Data");
  {
    m_SymbolInterpreter = m_Interpreter->CreateInterpreter<Inspect::SymbolInterpreter> ( m_RuntimeDataPanel );

    RefreshInstances(false);
  }
  m_Interpreter->Pop();

}

void RuntimeDataPanel::RefreshClassPicker()
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

void RuntimeDataPanel::RectifyRuntimeData()
{
  m_AllInstancesHaveRuntimeData = true; 

  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
  for ( ; selItr != selEnd; ++selItr )
  {
    RuntimeDataAdapterBase* adapter = dynamic_cast<RuntimeDataAdapterBase*>(*selItr); 
    // make runtime data current
    m_AllInstancesHaveRuntimeData &= adapter->RectifyRuntimeData();
  }

  m_EnableClassPicker       = true; 
  m_EnableClassBrowser      = true; 
  m_EnableSymbolInterpreter = m_AllInstancesHaveRuntimeData; 
}

void RuntimeDataPanel::Intersect(S_string& intersection, const S_string& classList)
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

void RuntimeDataPanel::GetSelectableClasses( S_string& classList )
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();

  bool seenInstance = false; 
  classList.clear(); 
  
  for ( ; selItr != selEnd; ++selItr )
  {
    RuntimeDataAdapterBase* adapter = dynamic_cast<RuntimeDataAdapterBase*>(*selItr); 
    
    S_string instanceClasses; 

    V_string classes;
    adapter->GetValidRuntimeClasses(classes);

    V_string::const_iterator validItr = classes.begin();
    V_string::const_iterator validEnd = classes.end();
    for ( ; validItr != validEnd; ++validItr )
    {
      const std::string& currentValidClass = *validItr;

      if(currentValidClass == "")
      {
        instanceClasses.insert(currentValidClass); 
        continue; 
      }

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


void RuntimeDataPanel::GetSelectableBaseClasses( S_string& classList )
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
  
  bool seenInstance = false; 
  classList.clear(); 

  for ( ; selItr != selEnd; ++selItr )
  {
    RuntimeDataAdapterBase* adapter = dynamic_cast<RuntimeDataAdapterBase*>(*selItr); 
    
    S_string instanceClasses; 
    V_string classes;

    adapter->GetValidRuntimeClasses(classes);

    V_string::const_iterator validItr = classes.begin();
    V_string::const_iterator validEnd = classes.end();
    for ( ; validItr != validEnd; ++validItr )
    {
      const std::string& currentValidClass = *validItr;

      // notice that there is no special case here for ""
      // because we must be a class to be a valid base class
      //
      // but "" is still a valid class -- just not a base class
      if ( Symbol::Inheritance::IsClass( currentValidClass ) )
      {
        instanceClasses.insert(currentValidClass); 
      }
      else
      {
        Console::Warning( "%s is no longer a valid runtime base class.\n", currentValidClass.c_str() );
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

void RuntimeDataPanel::GetCurrentSelectedClass( std::string& selectedClass )
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();

  for ( ; selItr != selEnd; ++selItr )
  {
    RuntimeDataAdapterBase* adapter = dynamic_cast<RuntimeDataAdapterBase*>(*selItr); 

    if(selItr == m_Selection.Begin())
    {
      selectedClass = adapter->GetRuntimeClass(); 
    }
    else
    {
      if(selectedClass != adapter->GetRuntimeClass())
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
void RuntimeDataPanel::RecurseAddDerivedClasses( const std::string& baseClass, S_string& classList )
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
void RuntimeDataPanel::OnRuntimeClassButton( Inspect::Button* button )
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
          RuntimeDataAdapterBase* adapter = dynamic_cast<RuntimeDataAdapterBase*>(*selItr); 
          adapter->SetRuntimeClass( runtimeClass );
        }

        RefreshInstances(true);

        m_RuntimeClassesChoice->Read(); 
        m_RuntimeDataPanel->GetCanvas()->Read(); 

      }
    }
  }
}

void RuntimeDataPanel::OnRuntimeClassChanged( const Inspect::ChangeArgs& args )
{
  RectifyRuntimeData(); 

  RefreshInstances(true);
}

void RuntimeDataPanel::RefreshInstances( bool layout )
{
  m_SymbolInterpreter->Reset();

  if(m_EnableSymbolInterpreter)
  {
    std::vector< Symbol::VarInstance* > varInstances;
    OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.End();
    for ( ; itr != end; ++itr )
    {
      RuntimeDataAdapterBase* adapter = dynamic_cast<RuntimeDataAdapterBase*>(*itr); 
      Symbol::UDTInstancePtr runtimeInstance = adapter->GetRuntimeInstance(); 

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