#include "PipelinePch.h"
#include "SceneNodePanel.h"

using namespace Helium;
using namespace Helium::SceneGraph;

SceneNodePanel::SceneNodePanel(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection)
: m_Selection (selection)
{
    m_Generator = generator;
    a_Name.Set( TXT("Scene Node") );

    m_Generator->PushContainer( TXT( "Scene ID Display" ) );
    {
        const tstring helpText = TXT( "This is the internal ID of this node, it's used to uniquely identify objects in the scene. This is primarly here for debugging purposes." );
        m_Generator->AddLabel( TXT( "ID" ) )->a_HelpText.Set( helpText );
        Inspect::Value* textBox = m_Generator->AddValue<SceneGraph::SceneNode, TUID>(m_Selection, &SceneGraph::SceneNode::GetID, &SceneGraph::SceneNode::SetID);
        textBox->a_HelpText.Set( helpText );
        textBox->a_IsReadOnly.Set(true);
    }
    m_Generator->Pop();

    m_Generator->PushContainer( TXT( "Scene Name Control" ) );
    {
        const tstring helpText = TXT( "This is the name of this node." );
        m_Generator->AddLabel( TXT( "Name" ) )->a_HelpText.Set( helpText );
        m_Generator->AddValue<SceneGraph::SceneNode, tstring>( m_Selection, &SceneGraph::SceneNode::GetName, &SceneGraph::SceneNode::SetGivenName )->a_HelpText.Set( helpText );
    }
    m_Generator->Pop();

    m_Generator->PushContainer( TXT( "Scene Auto Name Control" ) );
    {
        const tstring helpText = TXT( "When enabled, this node's name will be automatically generated and will change depending on the type of object it is.  Auto-generated names tend to end with a number, making them unique amongst other nodes of the same type." );
        m_Generator->AddLabel( TXT( "Auto Name" ) )->a_HelpText.Set( helpText );
        m_Generator->AddCheckBox<SceneGraph::SceneNode, bool>( m_Selection, &SceneGraph::SceneNode::UseAutoName, &SceneGraph::SceneNode::SetUseAutoName )->a_HelpText.Set( helpText );
    }
    m_Generator->Pop();

    m_Generator->PushContainer( TXT( "Membership" ) );
    {
        m_Generator->PushContainer();
        const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        m_Generator->AddList< SceneGraph::SceneNode, tstring >( m_Selection, &SceneGraph::SceneNode::GetMembership, &SceneGraph::SceneNode::SetMembership )->a_HelpText.Set( helpText );
        m_Generator->Pop();
    }
    m_Generator->Pop();
}