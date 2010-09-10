#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Canvas.h"

using namespace Helium;
using namespace Helium::Inspect;

Container::Container()
{
    a_IsEnabled.Changed().AddMethod( this, &Container::IsEnabledChanged );
    a_IsReadOnly.Changed().AddMethod( this, &Container::IsReadOnlyChanged );
    a_IsFrozen.Changed().AddMethod( this, &Container::IsFrozenChanged );
    a_IsHidden.Changed().AddMethod( this, &Container::IsHiddenChanged );
}

Container::~Container()
{
    Clear();
}

void Container::AddChild(Control* control)
{
    if ( control->GetParent() != this )
    {
        control->SetParent( this );

        control->a_IsEnabled.Set( a_IsEnabled.Get() );
        control->a_IsReadOnly.Set( a_IsReadOnly.Get() );
        control->a_IsFrozen.Set( a_IsFrozen.Get() );
        control->a_IsHidden.Set( a_IsHidden.Get() );

        m_Children.push_back( control );
    }
}

void Container::InsertChild(int index, Control* control)
{
    if ( control->GetParent() != this )
    {
        control->SetParent( this );

        control->a_IsEnabled.Set( a_IsEnabled.Get() );
        control->a_IsReadOnly.Set( a_IsReadOnly.Get() );
        control->a_IsFrozen.Set( a_IsFrozen.Get() );
        control->a_IsHidden.Set( a_IsHidden.Get() );

        m_Children.insert(m_Children.begin() + index, control);
    }
}

void Container::RemoveChild(Control* control)
{
    if ( control->GetParent() == this )
    {
        // unrealize the control
        control->Unrealize();

        // free the control from its parent
        control->SetParent( NULL );

        // remove our reference to the control
        const i32 numControls = static_cast< i32 >( m_Children.size() ) - 1;
        for ( i32 controlIndex = numControls; controlIndex > -1; --controlIndex )
        {
            if ( control == m_Children.at( controlIndex ) )
            {
                // remove control from our list
                m_Children.erase( m_Children.begin() + controlIndex );
                break;
            }
        }
    }
}

void Container::Clear()
{
    while (!m_Children.empty())
    {
        // get the current control to clear
        ControlPtr control = m_Children.back();

        // free widget resources (recursively)
        control->Unrealize();

        // do the remove work
        RemoveChild( control );
    }
}

void Container::Bind(const DataPtr& data)
{
    Base::Bind( data );

    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->Bind(data);
    }
}

bool Container::Process(const tstring& key, const tstring& value)
{
    if ( Base::Process( key, value ) )
    {
        return true;
    }

    if ( key == CONTAINER_ATTR_NAME )
    {
        a_Name.Set( value );
        return true;
    }

    return false;
}

void Container::Populate()
{
    Base::Populate();

    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->Populate();
    }
}

void Container::Read()
{
    Base::Read();

    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->Read();
    }
}

bool Container::Write()
{
    bool result = Base::Write();

    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        result &= (*itr)->Write();
    }

    return result;
}

void Container::IsEnabledChanged( const Attribute<bool>::ChangeArgs& args )
{
    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->a_IsEnabled.Set( args.m_NewValue );
    }
}

void Container::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->a_IsReadOnly.Set( args.m_NewValue );
    }
}

void Container::IsFrozenChanged( const Attribute<bool>::ChangeArgs& args )
{
    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->a_IsFrozen.Set( args.m_NewValue );
    }
}

void Container::IsHiddenChanged( const Attribute<bool>::ChangeArgs& args )
{
    V_Control::iterator itr = m_Children.begin();
    V_Control::iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        (*itr)->a_IsHidden.Set( args.m_NewValue );
    }
}