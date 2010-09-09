/*#include "Precompile.h"*/
#include "SceneNodePanel.h"

using namespace Helium;
using namespace Helium::Core;

SceneNodePanel::SceneNodePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
{
    m_Generator = generator;
    a_Name.Set( TXT("Scene Node") );

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "ID" ) );
        Inspect::Value* textBox = m_Generator->AddValue<Core::SceneNode, TUID>(m_Selection, &Core::SceneNode::GetID, &Core::SceneNode::SetID);
        textBox->a_IsReadOnly.Set(true);
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Name" ) );
        m_Generator->AddValue<Core::SceneNode, tstring>( m_Selection, &Core::SceneNode::GetName, &Core::SceneNode::SetGivenName );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Auto Name" ) );
        m_Generator->AddCheckBox<Core::SceneNode, bool>( m_Selection, &Core::SceneNode::UseAutoName, &Core::SceneNode::SetUseAutoName );
    }
    m_Generator->Pop();

    m_Generator->PushContainer( TXT( "Membership" ) );
    {
        m_Generator->PushContainer();
        m_Generator->AddList< Core::SceneNode, tstring >( m_Selection, &Core::SceneNode::GetMembership, &Core::SceneNode::SetMembership );
        m_Generator->Pop();
    }
    m_Generator->Pop();
}