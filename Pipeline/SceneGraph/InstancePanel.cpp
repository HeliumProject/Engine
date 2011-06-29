#include "PipelinePch.h"
#include "InstancePanel.h"
#include "Instance.h"

#include "Foundation/Inspect/Controls/ChoiceControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::SceneGraph;

InstancePanel::InstancePanel(PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection)
: m_Selection (selection)
{
    m_Generator = generator;
    a_Name.Set( TXT( "Instance" ) );

    m_Generator->PushContainer( TXT( "Solidity Control" ) );
    {
        const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        m_Generator->AddLabel( TXT( "Solid" ) )->a_HelpText.Set( helpText );

        m_SolidOverride = m_Generator->AddCheckBox<SceneGraph::Instance, bool>( m_Selection, &SceneGraph::Instance::GetSolidOverride, &SceneGraph::Instance::SetSolidOverride );
        m_SolidOverride->a_HelpText.Set( helpText );

        m_Solid = m_Generator->AddCheckBox<SceneGraph::Instance, bool>( m_Selection, &SceneGraph::Instance::GetSolid, &SceneGraph::Instance::SetSolid );
        m_Solid->a_HelpText.Set( helpText );
        m_Solid->Read();

        m_SolidOverride->e_ControlChanged.AddMethod( this, &InstancePanel::OnSolidOverride );
        m_SolidOverride->e_ControlChanged.Raise( m_SolidOverride );
    }
    m_Generator->Pop();

    m_Generator->PushContainer( TXT( "Transparency Control" ) );
    {
        const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        m_Generator->AddLabel( TXT( "Transparent" ) )->a_HelpText.Set( helpText );

        m_TransparentOverride = m_Generator->AddCheckBox<SceneGraph::Instance, bool>( m_Selection, &SceneGraph::Instance::GetTransparentOverride, &SceneGraph::Instance::SetTransparentOverride );
        m_TransparentOverride->a_HelpText.Set( helpText );

        m_Transparent = m_Generator->AddCheckBox<SceneGraph::Instance, bool>( m_Selection, &SceneGraph::Instance::GetTransparent, &SceneGraph::Instance::SetTransparent );
        m_Transparent->a_HelpText.Set( helpText );
        m_Transparent->Read();

        m_TransparentOverride->e_ControlChanged.AddMethod( this, &InstancePanel::OnTransparentOverride );
        m_TransparentOverride->e_ControlChanged.Raise( m_TransparentOverride );
    }
    m_Generator->Pop();
}

void InstancePanel::OnSolidOverride( const Inspect::ControlChangedArgs& args )
{
    tstring val;
    m_SolidOverride->ReadStringData( val );
    m_Solid->a_IsEnabled.Set( val == TXT("1") );
    m_Solid->Read();
}

void InstancePanel::OnTransparentOverride( const Inspect::ControlChangedArgs& args )
{
    tstring val;
    m_TransparentOverride->ReadStringData( val );
    m_Transparent->a_IsEnabled.Set( val == TXT("1") );
    m_Transparent->Read();
}
