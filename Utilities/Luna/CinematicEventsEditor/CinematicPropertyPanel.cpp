#include "Precompile.h"

#include "CinematicPropertyPanel.h"
#include "CinematicEventsPanels.h"
#include "CinematicEventsUndoCommands.h"
#include "CinematicEventsEditor.h"
#include "CinematicEventsEditorIDs.h"

#include "Inspect/Choice.h"
#include "Inspect/Button.h"
#include "Inspect/Action.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"

#include "Attribute/AttributeHandle.h"
#include "Content/RuntimeDataAttribute.h"

#include "Symbol/Inheritance.h"

#include <algorithm>

using namespace Reflect;
using namespace Luna;
using namespace Asset;
using namespace Attribute;
using namespace Content;

BEGIN_EVENT_TABLE( CinematicPropertyPanel, wxEvtHandler )
END_EVENT_TABLE()

CinematicPropertyPanel::CinematicPropertyPanel( CinematicEventsManager& manager, wxPanel* panel )
: CinematicPanelBase( manager, panel )
, m_Frozen( false )
, m_RuntimeClassesChoice (NULL)
, m_RuntimeClassesButton (NULL)
, m_RuntimeDataPanel (NULL)
{
  m_Manager.AddCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicPropertyPanel::ClipChanged ) );
  m_Manager.AddFrameChangedListener( FrameChangedSignature::Delegate ( this, &CinematicPropertyPanel::FrameChanged ) );
  m_Manager.AddEventExistenceListener( EventExistenceSignature::Delegate ( this, &CinematicPropertyPanel::EventExistence ) );
  m_Manager.AddSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &CinematicPropertyPanel::SelectionChanged ) );
  m_Manager.AddEventsChangedListener( EventsChangedSignature::Delegate ( this, &CinematicPropertyPanel::EventsChanged ) );
  m_Manager.AddCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicPropertyPanel::CinematicLoaded ) );
  m_Manager.AddEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &CinematicPropertyPanel::EventTypesChanged ) );
  m_Manager.AddCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicPropertyPanel::AssetOpened ) );
  m_Manager.AddCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicPropertyPanel::AssetClosed ) );

  m_CanvasWindow = new Inspect::CanvasWindow( m_Panel, wxID_ANY, wxDefaultPosition, m_Panel->GetSize(), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN );
  
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_CanvasWindow, 1, wxEXPAND | wxALL, 0 );

  m_Panel->SetSizer( sizer );
  m_Panel->Layout();
  sizer->Fit( m_Panel );

  m_Canvas.SetControl( m_CanvasWindow );

  DisableAll();
}

CinematicPropertyPanel::~CinematicPropertyPanel()
{
  m_Manager.RemoveCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicPropertyPanel::ClipChanged ) );
  m_Manager.RemoveFrameChangedListener( FrameChangedSignature::Delegate ( this, &CinematicPropertyPanel::FrameChanged ) );
  m_Manager.RemoveEventExistenceListener( EventExistenceSignature::Delegate ( this, &CinematicPropertyPanel::EventExistence ) );
  m_Manager.RemoveSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &CinematicPropertyPanel::SelectionChanged ) );
  m_Manager.RemoveEventsChangedListener( EventsChangedSignature::Delegate ( this, &CinematicPropertyPanel::EventsChanged ) );
  m_Manager.RemoveCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicPropertyPanel::CinematicLoaded ) );
  m_Manager.RemoveEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &CinematicPropertyPanel::EventTypesChanged ) );
  m_Manager.RemoveCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicPropertyPanel::AssetOpened ) );
  m_Manager.RemoveCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicPropertyPanel::AssetClosed ) );
}

void CinematicPropertyPanel::EnableAll()
{
}

void CinematicPropertyPanel::DisableAll()
{
}

void CinematicPropertyPanel::AssetOpened( const CinematicFileChangeArgs& args )
{
  EnableAll();
}

void CinematicPropertyPanel::AssetClosed( const CinematicFileChangeArgs& args )
{
  DisableAll();
}

void CinematicPropertyPanel::EventExistence( const EventExistenceArgs& args )
{
}

void CinematicPropertyPanel::ClipChanged( const CinematicChangeArgs& args )
{
}

void CinematicPropertyPanel::EventsChanged( const EventsChangedArgs& args )
{
  if ( m_Frozen )
    return;

  if ( m_Manager.GetUpdateProperties() )
  {
    UpdatePanel();
  }
}

bool CinematicPropertyPanel::RectifyAndBuildClasses( S_string& classList )
{
  OS_CinematicEvent selection = m_Manager.GetSelection();
  bool hasRuntimeData = !selection.Empty();

  // Notice the early out from this loop if we find an item that does not have runtime data
  OS_CinematicEvent::Iterator selItr = selection.Begin();
  OS_CinematicEvent::Iterator selEnd = selection.End();
  for ( ; selItr != selEnd; ++selItr )
  {
    CinematicEvent* cineEvent = ObjectCast< CinematicEvent >( *selItr );
    if ( cineEvent )
    {
      // make runtime data current
      hasRuntimeData &= cineEvent->RectifyRuntimeData();

      V_string classes;
      cineEvent->GetValidRuntimeClasses(classes);
      V_string::const_iterator validItr = classes.begin();
      V_string::const_iterator validEnd = classes.end();
      for ( ; validItr != validEnd; ++validItr )
      {
        const std::string& currentValidClass = *validItr;

        if ( Symbol::Inheritance::IsClass( currentValidClass ) )
        {
          RecurseAddDerivedClasses( currentValidClass, classList );
        }
        else
        {
          Console::Warning( "%s is no longer a valid runtime class.\n", currentValidClass.c_str() );
        }
      }
    }
  }

  return hasRuntimeData || !classList.empty();
}

void CinematicPropertyPanel::RecurseAddDerivedClasses( const std::string& baseClass, S_string& classList )
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

void CinematicPropertyPanel::OnRuntimeClassButton( Inspect::Button* button )
{
  /*S_string validBaseClasses;
  std::string currentSelectedClass;

  // Fetch the client data off the button to populate the class chooser dialog
  GetSelectableBaseClasses(validBaseClasses); 
  GetCurrentSelectedClass(currentSelectedClass); 
  
  if ( !validBaseClasses.empty() )
  {
    // Create the dialog and set the inital value in the tree to match what is displayed
    // in the combo box.
    LClassChooserDlg classChooserDlg( button->GetCanvas()->GetControl(), validBaseClasses );
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
          LInstance* instance = ObjectCast< LInstance >( *selItr );
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
  }*/
}


void CinematicPropertyPanel::CreateRuntimeData()
{
  bool hasRuntimeData = false;

  m_Interpreter->PushContainer();
  {
    // Controls for changing the runtime class (only enabled if the instance has a runtime data attribute)
    m_Interpreter->AddLabel( "Runtime Class" );

    m_RuntimeClassesChoice = m_Interpreter->AddChoice< std::string >( new Nocturnal::MemberProperty<CinematicPropertyPanel, std::string> (this, &CinematicPropertyPanel::GetRuntimeClassName, &CinematicPropertyPanel::SetRuntimeClassName ) );
    m_RuntimeClassesChoice->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &CinematicPropertyPanel::OnRuntimeClassChanged ) );

    m_RuntimeClassesButton = m_Interpreter->AddAction( Inspect::ActionSignature::Delegate( this, &CinematicPropertyPanel::OnRuntimeClassButton ) );
    m_RuntimeClassesButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &CinematicPropertyPanel::OnRuntimeClassChanged ) );

    hasRuntimeData = RefreshClasses();
  }
  m_Interpreter->Pop();

  m_RuntimeDataPanel = m_Interpreter->PushPanel("Runtime Data");
  {
    m_SymbolInterpreter = m_Interpreter->CreateInterpreter<Inspect::SymbolInterpreter> ( m_RuntimeDataPanel );

    if (hasRuntimeData)
    {
      RefreshInstances(false);
    }
  }
  m_Interpreter->Pop();
}

bool CinematicPropertyPanel::RefreshClasses()
{
  S_string runtimeClassNames;
  bool hasRuntimeData = RectifyAndBuildClasses( runtimeClassNames );

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
  m_RuntimeClassesChoice->SetEnabled( hasRuntimeData );
  m_RuntimeClassesButton->SetEnabled( hasRuntimeData );

  return hasRuntimeData;
}

std::string CinematicPropertyPanel::GetRuntimeClassName() const
{
  std::string result;
  bool hasRuntimeData = false;

  Asset::OS_CinematicEvent::Iterator itr = m_Manager.GetSelection().Begin();
  Asset::OS_CinematicEvent::Iterator end = m_Manager.GetSelection().End();
  for ( ; itr != end; ++itr )
  {
    Asset::CinematicEventPtr instance = *itr;
    AttributeViewer< RuntimeDataAttribute > dataViewer( instance );

    if ( itr == m_Manager.GetSelection().Begin() )
    {
      hasRuntimeData = dataViewer.Valid();
    }
    else if ( hasRuntimeData != dataViewer.Valid() )
    {
      return Inspect::MULTI_VALUE_STRING;
    }

    if ( hasRuntimeData )
    {
      if ( itr == m_Manager.GetSelection().Begin() )
      {
        result = dataViewer->GetRuntimeData()->GetRuntimeClass();
      }
      else if ( result != dataViewer->GetRuntimeData()->GetRuntimeClass() )
      {
        return Inspect::MULTI_VALUE_STRING;
      }
    }
  }

  return result;
}

void CinematicPropertyPanel::SetRuntimeClassName( const std::string& className )
{
  Asset::OS_CinematicEvent::Iterator itr = m_Manager.GetSelection().Begin();
  Asset::OS_CinematicEvent::Iterator end = m_Manager.GetSelection().End();
  for ( ; itr != end; ++itr )
  {
    Asset::CinematicEventPtr instance = *itr;
    AttributeEditor< RuntimeDataAttribute > dataEditor( instance );
    if ( dataEditor.Valid() )
    {
      dataEditor->GetRuntimeData()->SetRuntimeClass( className );
      dataEditor.Commit();
    }
  }
}

void CinematicPropertyPanel::OnRuntimeClassChanged( const Inspect::ChangeArgs& args )
{
  RefreshInstances(true);

  m_RuntimeDataPanel->GetCanvas()->Read(); 
}

void CinematicPropertyPanel::RefreshInstances( bool layout )
{
  m_SymbolInterpreter->Reset();

  OS_CinematicEvent& selection = m_Manager.GetSelection();

  std::vector< Symbol::VarInstance* > varInstances;

  // get an attribute viewer to the content
  OS_CinematicEvent::Iterator itr = selection.Begin();
  OS_CinematicEvent::Iterator end = selection.End();
  for ( ; itr!=end ; ++itr )
  {
    AttributeViewer<RuntimeDataAttribute> viewer ( *itr );

    // this should be true here if we were in the m_Selection list and hasRuntimeData is true
    if (!viewer.Valid())
    {
      varInstances.clear();
      break;
    }

    // depending on how events fall out, this *could* be null
    if (viewer->GetRuntimeData()->GetRuntimeInstance().ReferencesObject())
    {
      // queue to be reflected upon
      varInstances.push_back( viewer->GetRuntimeData()->GetRuntimeInstance() );
    }
  }

  if (!varInstances.empty())
  {
    m_SymbolInterpreter->Interpret( varInstances );
  }

  m_RuntimeDataPanel->SetEnabled( !varInstances.empty() );
  m_RuntimeDataPanel->SetExpanded( !varInstances.empty() );

  if (layout)
  {
    m_RuntimeDataPanel->GetCanvas()->Layout();
  }
}


void CinematicPropertyPanel::UpdatePanel()
{
  m_Canvas.Clear();

  OS_CinematicEvent& selection = m_Manager.GetSelection();

  if ( !selection.Empty() )
  {
    std::vector< Reflect::Element* > elems;

    OS_CinematicEvent::Iterator eventIt = selection.Begin();
    OS_CinematicEvent::Iterator eventEnd = selection.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      elems.push_back( *eventIt );
    }

    m_Interpreter = new Inspect::ReflectInterpreter( &m_Canvas );
    m_Interpreter->Interpret( elems );

    m_Interpreter->AddPropertyChangedListener( Inspect::ChangedSignature::Delegate ( this, &CinematicPropertyPanel::PropertyChanged ) );

    CreateRuntimeData();

    m_Canvas.Freeze();
    m_Canvas.Layout();
    m_Canvas.Read();
    m_Canvas.Thaw();
  }
}

void CinematicPropertyPanel::FrameChanged( const FrameChangedArgs& args )
{
}

void CinematicPropertyPanel::SelectionChanged( const EventSelectionChangedArgs& args )
{
  UpdatePanel();
}

void CinematicPropertyPanel::CinematicLoaded( const CinematicLoadedArgs& args )
{
}

void CinematicPropertyPanel::EventTypesChanged( const EventTypesChangedArgs& args )
{
}

void CinematicPropertyPanel::PropertyChanged( const Inspect::ChangeArgs& args )
{
  m_Frozen = true;

  CinematicEventsChangedCommand* undoCommand = new CinematicEventsChangedCommand( m_Manager );
  m_Manager.GetUndoQueue().Push( undoCommand );
  m_Manager.EventsChanged( m_Manager.GetSelection() );

  m_Frozen = false;
}