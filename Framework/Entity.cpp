//----------------------------------------------------------------------------------------------------------------------
// Entity.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/Entity.h"

#include "Framework/Layer.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( Entity, Framework, 0 );

/// Constructor.
Entity::Entity()
: m_position( 0.0f )
, m_rotation( Simd::Quat::IDENTITY )
, m_scale( 1.0f )
, m_layerIndex( Invalid< size_t >() )
, m_updatePhaseFlags( 0 )
, m_pendingDeferredWorkFlags( 0 )
, m_deferredWorkFlags( 0 )
{
}

/// Destructor.
Entity::~Entity()
{
    HELIUM_ASSERT( !m_spLayer );
}

/// @copydoc GameObject::Serialize()
void Entity::Serialize( Serializer& s )
{
    L_SERIALIZE_SUPER( s );

    s << L_TAGGED( m_position );
    s << L_TAGGED( m_rotation );
    s << L_TAGGED( m_scale );
}

/// Perform any necessary work upon attaching this entity to the world.
///
/// The World instance to which this entity is being attached can be accessed using the GetWorld() method.
///
/// When overriding this function in subclasses, make sure to chain onto the parent class's implementation first.
///
/// @see Detach()
void Entity::Attach()
{
#ifndef NDEBUG
    // Entities cannot be attached during asynchronous updates.
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();
    HELIUM_ASSERT_MSG(
        updatePhase != WorldManager::UPDATE_PHASE_PRE && updatePhase != WorldManager::UPDATE_PHASE_POST,
        TXT( "Entity::Attach() cannot be called during pre- or post-update phases." ) );
#endif
}

/// Perform any necessary work upon detaching this entity to the world.
///
/// The World instance from which this entity is being detached can be accessed using the GetWorld() method.
///
/// When overriding this function in subclasses, make sure to chain onto the parent class's implementation last.
///
/// @see Attach()
void Entity::Detach()
{
#ifndef NDEBUG
    // Entities cannot be detached during asynchronous updates.
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();
    HELIUM_ASSERT_MSG(
        updatePhase != WorldManager::UPDATE_PHASE_PRE && updatePhase != WorldManager::UPDATE_PHASE_POST,
        TXT( "Entity::Detach() cannot be called during pre- or post-update phases." ) );
#endif
}

/// Flag this this entity to be reattached after its synchronous update.
///
/// This function can be safely called on an entity during its own pre-update.
///
/// @see Attach(), Detach()
void Entity::DeferredReattach()
{
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();

#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    // This function can only be called during asynchronous updating if this entity is the one being updated.
    const Entity* pCurrentEntity = rWorldManager.GetCurrentThreadUpdateEntity();
    HELIUM_ASSERT_MSG(
        ( pCurrentEntity == this ||
        ( updatePhase != WorldManager::UPDATE_PHASE_PRE && updatePhase != WorldManager::UPDATE_PHASE_POST ) ),
        TXT( "SafeReattach() can only be called on the entity being updated during asynchronous update phases." ) );
    if( pCurrentEntity != this &&
        ( updatePhase == WorldManager::UPDATE_PHASE_PRE || updatePhase == WorldManager::UPDATE_PHASE_POST ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "SafeReattach() called on an entity other than the one being updated by the current " )
            TXT( "thread during %s-update.\n" ) ),
            ( updatePhase == WorldManager::UPDATE_PHASE_PRE ? TXT( "pre" ) : TXT( "post" ) ) );
    }
#endif

    if( updatePhase == WorldManager::UPDATE_PHASE_PRE )
    {
        m_pendingDeferredWorkFlags |= DEFERRED_WORK_FLAG_REATTACH;
    }
    else
    {
        m_deferredWorkFlags |= DEFERRED_WORK_FLAG_REATTACH;
    }
}

/// Perform an entity pre-update.
///
/// During pre-update, an entity can perform read-only access to itself, as well as any other entities in the world.
/// Changes can also be made to private data (i.e. internal script state information) as long as it is not
/// accessible to any other entities in the world.  Messages can be passed to other entities, as well as this
/// entity, for handling during the post-update phase.
///
/// Note that this function is non-constant to allow for private data modification, although care should be taken to
/// avoid making non-thread safe modifications that are viewable externally.
///
/// @param[in] deltaSeconds  Seconds elapsed since the previous frame update.
///
/// @see PostUpdate()
void Entity::PreUpdate( float32_t /*deltaSeconds*/ )
{
}

/// Perform an entity post-update.
///
/// During post-update, messages passed to this entity are resolved internally.  An entity can both read and modify
/// its own data, but cannot read data from any other entities.  Messages cannot be passed to other entities at
/// this time, either.
///
/// @param[in] deltaSeconds  Seconds elapsed since the previous frame update.
///
/// @see PreUpdate()
void Entity::PostUpdate( float32_t /*deltaSeconds*/ )
{
}

/// Perform synchronous updating for this entity.
///
/// Synchronous updating is performed after the pre- and post-updating phases.  It is only performed on an entity if
/// the DEFERRED_WORK_FLAG_DESTROY flag is set.
///
/// @param[in] deltaSeconds  Seconds elapsed since the previous frame update.
///
/// @see GetDeferredWorkFlags()
void Entity::SynchronousUpdate( float32_t /*deltaSeconds*/ )
{
}

/// Directly set the position of this entity.
///
/// Note that this interface does not guarantee proper updating of scenes in which this entity belongs.
///
/// @param[in] rPosition  Position to set.
///
/// @see GetPosition(), SetRotation(), SetScale()
void Entity::SetPosition( const Simd::Vector3& rPosition )
{
    VerifySafety();
    m_position = rPosition;
}

/// Directly set the rotation of this entity.
///
/// Note that this interface does not guarantee proper updating of scenes in which this entity belongs.
///
/// @param[in] rRotation  Rotation to set.
///
/// @see GetRotation(), SetPosition(), SetScale()
void Entity::SetRotation( const Simd::Quat& rRotation )
{
    VerifySafety();
    m_rotation = rRotation;
}

/// Directly set the scale of this entity.
///
/// Note that this interface does not guarantee proper updating of scenes in which this entity belongs.
///
/// @param[in] rScale  Scale to set.
///
/// @see GetScale(), SetPosition(), SetRotation()
void Entity::SetScale( const Simd::Vector3& rScale )
{
    VerifySafety();
    m_scale = rScale;
}

/// Set the layer to which this entity is currently bound, along with the index of this entity within the layer.
///
/// @param[in] pLayer      Layer to set.
/// @param[in] layerIndex  Index within the layer to set.
///
/// @see SetLayerIndex(), GetLayer(), GetLayerIndex(), ClearLayerInfo()
void Entity::SetLayerInfo( Layer* pLayer, size_t layerIndex )
{
    HELIUM_ASSERT( pLayer );
    HELIUM_ASSERT( IsValid( layerIndex ) );

    m_spLayer = pLayer;
    m_layerIndex = layerIndex;
}

/// Update the index of this entity within its layer.
///
/// @param[in] layerIndex  Index within the layer to set.
///
/// @see SetLayerInfo(), GetLayer(), GetLayerIndex(), ClearLayerInfo()
void Entity::SetLayerIndex( size_t layerIndex )
{
    HELIUM_ASSERT( m_spLayer );
    HELIUM_ASSERT( IsValid( layerIndex ) );

    m_layerIndex = layerIndex;
}

/// Clear out any currently set layer binding information.
///
/// @see SetLayerInfo(), SetLayerIndex(), GetLayer(), GetLayerIndex()
void Entity::ClearLayerInfo()
{
    m_spLayer.Release();
    SetInvalid( m_layerIndex );
}

/// Get the world to which this entity is currently bound.
///
/// @return  Entity world.
///
/// @see GetLayer()
WorldWPtr Entity::GetWorld() const
{
    VerifySafety();
    return ( m_spLayer ? m_spLayer->GetWorld() : WorldWPtr() );
}

/// Set the update phase flags for this entity.
///
/// The set of flags specified will replace the current set of flags entirely.
///
/// @param[in] flags  Required update phase flags.
void Entity::SetUpdatePhaseFlags( uint32_t flags )
{
    VerifySafety();
    m_updatePhaseFlags = flags;
}
