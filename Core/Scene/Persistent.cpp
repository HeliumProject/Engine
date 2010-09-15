/*#include "Precompile.h"*/
#include "Core/Scene/Persistent.h"

#include "Foundation/Undo/PropertyCommand.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT( Persistent );

Persistent::Persistent( Reflect::Element* package )
: m_Package( package )
{
}

Persistent::~Persistent()
{
}

void Persistent::GetState( Reflect::ElementPtr& state ) const
{
    const_cast<Persistent*>(this)->Pack();

    state = m_Package->Clone();
}

void Persistent::SetState( const Reflect::ElementPtr& state )
{
    if ( !state->Equals( m_Package ) )
    {
        state->CopyTo( m_Package );
        Unpack();
        m_Package->RaiseChanged();
    }
}

Undo::CommandPtr Persistent::SnapShot( Reflect::Element* newState )
{
    if ( newState == NULL )
    {
        return new Undo::PropertyCommand<Reflect::ElementPtr>( new Helium::MemberProperty<Persistent, Reflect::ElementPtr> (this, &Persistent::GetState, &Persistent::SetState) );
    }

    return new Undo::PropertyCommand<Reflect::ElementPtr>( new Helium::MemberProperty<Persistent, Reflect::ElementPtr> (this, &Persistent::GetState, &Persistent::SetState), Reflect::ElementPtr( newState ) );
}
