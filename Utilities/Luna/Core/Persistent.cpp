#include "Precompile.h"
#include "Persistent.h"

#include "Application/Undo/PropertyCommand.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Persistent );

void Persistent::InitializeType()
{
  Reflect::RegisterClass<Persistent>( "Persistent" );
}

void Persistent::CleanupType()
{
  Reflect::UnregisterClass<Persistent>();
}

Persistent::Persistent( Reflect::Element* package )
: m_Package( package )
{
  if ( m_Package.ReferencesObject() )
  {
    m_Package->AddChangedListener( Reflect::ElementChangeSignature::Delegate ( this, &Persistent::PackageChanged ) );
  }
}

Persistent::~Persistent()
{
  if ( m_Package.ReferencesObject() )
  {
    m_Package->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate ( this, &Persistent::PackageChanged ) );
  }
}

void Persistent::PackageChanged( const Reflect::ElementChangeArgs& args )
{
  NOC_ASSERT( args.m_Element == m_Package.Ptr() );

  // Convert the Element Changed callback into an Object Changed event
  RaiseObjectChanged( args );
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
    return new Undo::PropertyCommand<Reflect::ElementPtr>( new Nocturnal::MemberProperty<Persistent, Reflect::ElementPtr> (this, &Persistent::GetState, &Persistent::SetState) );
  }

  return new Undo::PropertyCommand<Reflect::ElementPtr>( new Nocturnal::MemberProperty<Persistent, Reflect::ElementPtr> (this, &Persistent::GetState, &Persistent::SetState), Reflect::ElementPtr( newState ) );
}
