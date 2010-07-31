#include "Precompile.h"
#include "SceneNodePanel.h"

using namespace Helium;
using namespace Helium::Editor;

SceneNodePanel::SceneNodePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
{
  m_Interpreter = m_Generator = generator;
  m_Expanded = true;
  m_Text = TXT( "Scene Node" );
}

SceneNodePanel::~SceneNodePanel()
{
}

void SceneNodePanel::Create()
{
  m_Generator->PushContainer();
  {
    m_Generator->AddLabel( TXT( "ID" ) );
    Inspect::Value* textBox = m_Generator->AddValue<Editor::SceneNode, TUID>(m_Selection, &Editor::SceneNode::GetID, &Editor::SceneNode::SetID);
    textBox->SetReadOnly(true);
  }
  m_Generator->Pop();

  m_Generator->PushContainer();
  {
    m_Generator->AddLabel( TXT( "Name" ) );
    m_Generator->AddValue<Editor::SceneNode, tstring>( m_Selection, &Editor::SceneNode::GetName, &Editor::SceneNode::SetGivenName );
  }
  m_Generator->Pop();

  m_Generator->PushContainer();
  {
    m_Generator->AddLabel( TXT( "Auto Name" ) );
    m_Generator->AddCheckBox<Editor::SceneNode, bool>( m_Selection, &Editor::SceneNode::UseAutoName, &Editor::SceneNode::SetUseAutoName );
  }
  m_Generator->Pop();

  m_Generator->PushPanel( TXT( "Membership" ) );
  {
    m_Generator->PushContainer();
    m_Generator->AddList< Editor::SceneNode, tstring >( m_Selection, &Editor::SceneNode::GetMembership, &Editor::SceneNode::SetMembership );
    m_Generator->Pop();
  }
  m_Generator->Pop();

  __super::Create();
}