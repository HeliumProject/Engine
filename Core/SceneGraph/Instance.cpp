/*#include "Precompile.h"*/
#include "Instance.h"

#include "Foundation/Log.h"

#include "Core/SceneGraph/InstanceType.h"
#include "Core/SceneGraph/InstancePanel.h"
#include "Core/SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(Instance);

void Instance::EnumerateClass( Reflect::Compositor<Instance>& comp )
{
    comp.AddField( &Instance::m_Solid,                  "m_Solid" );
    comp.AddField( &Instance::m_SolidOverride,          "m_SolidOverride" );
    comp.AddField( &Instance::m_Transparent,            "m_Transparent" );
    comp.AddField( &Instance::m_TransparentOverride,    "m_TransparentOverride" );
}

void Instance::InitializeType()
{
    Reflect::RegisterClassType< Instance >( TXT( "Instance" ) );
    PropertiesGenerator::InitializePanel( TXT( "Instance" ), CreatePanelSignature::Delegate( &Instance::CreatePanel ) );
}

void Instance::CleanupType()
{
    Reflect::UnregisterClassType< Instance >();
}

Instance::Instance()
: m_Solid (false)
, m_SolidOverride (false)
, m_Transparent (false)
, m_TransparentOverride (false)
{
}

Instance::~Instance()
{
}

i32 Instance::GetImageIndex() const
{
    i32 image = -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "null" ) );

    if ( GetNodeType() )
    {
        image = m_NodeType->GetImageIndex();
    }

    return image;
}

SceneNodeTypePtr Instance::CreateNodeType( SceneGraph::Scene* scene ) const
{
    // Overridden to create an light-specific type
    InstanceType* nodeType = new InstanceType( scene, GetType() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Instance.
// 
bool Instance::ValidatePanel(const tstring& name)
{
    if ( name == TXT( "Instance" ) )
        return true;

    return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit Instance.
// 
void Instance::CreatePanel( CreatePanelArgs& args )
{
    InstancePanel* panel = new InstancePanel ( args.m_Generator, args.m_Selection );

    args.m_Generator->Push( panel );
    {
        panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
    }
    args.m_Generator->Pop();
}

bool Instance::GetSolid() const
{
    InstanceType* type = Reflect::AssertCast<InstanceType>(m_NodeType);

    return m_SolidOverride;
}
void Instance::SetSolid( bool b )
{
    m_Solid = b;
}

bool Instance::GetSolidOverride() const
{
    return m_SolidOverride;
}
void Instance::SetSolidOverride( bool b )
{
    m_SolidOverride = b;
}

bool Instance::GetTransparent() const
{
    InstanceType* type = Reflect::AssertCast<InstanceType>(m_NodeType);

    return m_TransparentOverride;
}
void Instance::SetTransparent( bool b )
{
    m_Transparent = b;
}

bool Instance::GetTransparentOverride() const
{
    return m_TransparentOverride;
}
void Instance::SetTransparentOverride( bool b )
{
    m_TransparentOverride = b;
}

