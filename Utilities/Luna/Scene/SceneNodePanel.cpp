#include "Precompile.h"
#include "SceneNodePanel.h"

using namespace Luna;

SceneNodePanel::SceneNodePanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
{
  m_Interpreter = m_Enumerator = enumerator;
  m_Expanded = true;
  m_Text = "Scene Node";
}

SceneNodePanel::~SceneNodePanel()
{
}

void SceneNodePanel::Create()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel("ID");
    Inspect::Value* textBox = m_Enumerator->AddValue<Luna::SceneNode, UniqueID::TUID>(m_Selection, &Luna::SceneNode::GetID, &Luna::SceneNode::SetID);
    textBox->SetReadOnly(true);
  }
  m_Enumerator->Pop();

  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel("Name");
    m_Enumerator->AddValue<Luna::SceneNode, std::string>( m_Selection, &Luna::SceneNode::GetName, &Luna::SceneNode::SetGivenName );
  }
  m_Enumerator->Pop();

  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel("Auto Name");
    m_Enumerator->AddCheckBox<Luna::SceneNode, bool>( m_Selection, &Luna::SceneNode::UseAutoName, &Luna::SceneNode::SetUseAutoName );
  }
  m_Enumerator->Pop();

  m_Enumerator->PushPanel("Membership");
  {
    m_Enumerator->PushContainer();
    m_Enumerator->AddList< Luna::SceneNode, std::string >( m_Selection, &Luna::SceneNode::GetMembership, &Luna::SceneNode::SetMembership );
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();

  __super::Create();
}