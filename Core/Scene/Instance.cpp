/*#include "Precompile.h"*/
#include "Instance.h"

#include "Foundation/Log.h"

#include "Core/Scene/InstanceType.h"
#include "Core/Scene/InstancePanel.h"
#include "Core/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

// RTTI
REFLECT_DEFINE_ABSTRACT(Core::Instance);

void Instance::InitializeType()
{
    Reflect::RegisterClassType< Core::Instance >( TXT( "Core::Instance" ) );
    PropertiesGenerator::InitializePanel( TXT( "Instance" ), CreatePanelSignature::Delegate( &Instance::CreatePanel ) );
}

void Instance::CleanupType()
{
    Reflect::UnregisterClassType< Core::Instance >();
}

Instance::Instance(Core::Scene* scene, Content::Instance* instance)
: Core::PivotTransform ( scene, instance )
{
}

Instance::~Instance()
{
}

void Instance::Pack()
{
    __super::Pack();
}

void Instance::Unpack()
{
    __super::Unpack();
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

SceneNodeTypePtr Instance::CreateNodeType( Core::Scene* scene ) const
{
    // Overridden to create an light-specific type
    Core::InstanceType* nodeType = new Core::InstanceType( scene, GetType() );

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
// Static function for creating the UI panel that allows users to edit Core::Instance.
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
    Core::InstanceType* type = Reflect::AssertCast<Core::InstanceType>(m_NodeType);

    return GetPackage< Content::Instance >()->m_SolidOverride;
}
void Instance::SetSolid( bool b )
{
    GetPackage< Content::Instance >()->m_Solid = b;
}

bool Instance::GetSolidOverride() const
{
    return GetPackage< Content::Instance >()->m_SolidOverride;
}
void Instance::SetSolidOverride( bool b )
{
    GetPackage< Content::Instance >()->m_SolidOverride = b;
}

bool Instance::GetTransparent() const
{
    Core::InstanceType* type = Reflect::AssertCast<Core::InstanceType>(m_NodeType);

    return GetPackage< Content::Instance >()->m_TransparentOverride;
}
void Instance::SetTransparent( bool b )
{
    GetPackage< Content::Instance >()->m_Transparent = b;
}

bool Instance::GetTransparentOverride() const
{
    return GetPackage< Content::Instance >()->m_TransparentOverride;
}
void Instance::SetTransparentOverride( bool b )
{
    GetPackage< Content::Instance >()->m_TransparentOverride = b;
}

