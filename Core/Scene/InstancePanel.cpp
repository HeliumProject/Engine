/*#include "Precompile.h"*/
#include "InstancePanel.h"
#include "Instance.h"
#include "Volume.h"

#include "Foundation/Inspect/Controls/ChoiceControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Core;

InstancePanel::InstancePanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
: m_Selection (selection)
{
    m_Generator = generator;
    a_Name.Set( TXT( "Instance" ) );

    m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        m_Generator->AddLabel( TXT( "Solid" ) )->a_HelpText.Set( helpText );

        m_SolidOverride = m_Generator->AddCheckBox<Core::Instance, bool>( m_Selection, &Core::Instance::GetSolidOverride, &Core::Instance::SetSolidOverride );
        m_SolidOverride->a_HelpText.Set( helpText );

        m_Solid = m_Generator->AddCheckBox<Core::Instance, bool>( m_Selection, &Core::Instance::GetSolid, &Core::Instance::SetSolid );
        m_Solid->a_HelpText.Set( helpText );
        m_Solid->Read();

        m_SolidOverride->e_ControlChanged.AddMethod( this, &InstancePanel::OnSolidOverride );
        m_SolidOverride->e_ControlChanged.Raise( m_SolidOverride );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        m_Generator->AddLabel( TXT( "Transparent" ) )->a_HelpText.Set( helpText );

        m_TransparentOverride = m_Generator->AddCheckBox<Core::Instance, bool>( m_Selection, &Core::Instance::GetTransparentOverride, &Core::Instance::SetTransparentOverride );
        m_TransparentOverride->a_HelpText.Set( helpText );

        m_Transparent = m_Generator->AddCheckBox<Core::Instance, bool>( m_Selection, &Core::Instance::GetTransparent, &Core::Instance::SetTransparent );
        m_Transparent->a_HelpText.Set( helpText );
        m_Transparent->Read();

        m_TransparentOverride->e_ControlChanged.AddMethod( this, &InstancePanel::OnTransparentOverride );
        m_TransparentOverride->e_ControlChanged.Raise( m_TransparentOverride );
    }
    m_Generator->Pop();

    bool allVolumes = true;

    OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.End();
    for ( ; itr != end; ++itr )
    {
        Core::VolumePtr volume = Reflect::ObjectCast< Volume >( *itr );
        if ( !volume )
        {
            allVolumes = false;
        }
    }

    if ( allVolumes )
    {
        m_Generator->PushContainer();
        {
            static const tstring helpText = TXT( "Determines if a pointer should be drawn in the 3d view at the location where the volume is." );
            m_Generator->AddLabel( TXT( "Show Pointer" ) )->a_HelpText.Set( helpText );
            m_Generator->AddCheckBox<Core::Volume, bool>( m_Selection, &Core::Volume::IsPointerVisible, &Core::Volume::SetPointerVisible )->a_HelpText.Set( helpText );
        }
        m_Generator->Pop();
    }
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
