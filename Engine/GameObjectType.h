//----------------------------------------------------------------------------------------------------------------------
// GameObjectType.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_GAME_OBJECT_TYPE_H
#define LUNAR_ENGINE_GAME_OBJECT_TYPE_H

#include "Engine/Engine.h"

#include "Platform/String.h"
#include "Foundation/Name.h"
#include "Foundation/Container/HashMap.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/ReflectionInfo.h"
#include "Engine/GameObject.h"

namespace Lunar
{
    class GameObjectType;
    typedef SmartPtr< GameObjectType > GameObjectTypePtr;

    /// Run-time type information for GameObject classes.
    class LUNAR_ENGINE_API GameObjectType : public Reflect::Class
    {
    public:
        REFLECTION_TYPE( Reflect::ReflectionTypes::GameObjectType );

        /// Type lookup hash map.
        typedef HashMap< Name, GameObjectTypePtr > LookupMap;

        /// Static type release callback.
        typedef void ( RELEASE_STATIC_TYPE_CALLBACK )();

        /// General type flags.
        enum EFlag
        {
            /// Type should not be instantiated.
            FLAG_ABSTRACT    = 1 << 0,
            /// Instances of this type should never be saved or loaded in a package.
            FLAG_TRANSIENT   = 1 << 1,
            /// Instances of this type cannot be used as templates.
            FLAG_NO_TEMPLATE = 1 << 2
        };

        /// Type iterator.
        class LUNAR_ENGINE_API ConstIterator
        {
            friend class GameObjectType;

        public:
            /// @name Construction/Destruction
            //@{
            inline ConstIterator();
            //@}

            /// @name Overloaded Operators
            //@{
            inline GameObjectType& operator*() const;
            inline GameObjectType* operator->() const;

            inline ConstIterator& operator++();
            inline ConstIterator operator++( int );
            inline ConstIterator& operator--();
            inline ConstIterator operator--( int );

            inline bool operator==( const ConstIterator& rOther ) const;
            inline bool operator!=( const ConstIterator& rOther ) const;
            inline bool operator<( const ConstIterator& rOther ) const;
            inline bool operator>( const ConstIterator& rOther ) const;
            inline bool operator<=( const ConstIterator& rOther ) const;
            inline bool operator>=( const ConstIterator& rOther ) const;
            //@}

        private:
            /// Type map iterator.
            LookupMap::ConstIterator m_iterator;

            /// @name Construction/Destruction, Private
            //@{
            inline explicit ConstIterator( LookupMap::ConstIterator iterator );
            //@}
        };

        /// @name Construction/Destruction
        //@{
        GameObjectType();
        virtual ~GameObjectType();
        //@}

        /// @name Data Access
        //@{
        inline Name GetName() const;
        inline const GameObjectType* GetBaseType() const;
        inline GameObject* GetTemplate() const;

        inline uint32_t GetFlags() const;
        //@}

        /// @name Type Information
        //@{
        bool IsSubtypeOf( const GameObjectType* pType ) const;
        //@}

        /// @name Static Type Registration
        //@{
        inline static Package* GetTypePackage();
        static void SetTypePackage( Package* pPackage );

        static GameObjectType* Create(
            Name name, Package* pTypePackage, const GameObjectType* pParent, GameObject* pTemplate,
            RELEASE_STATIC_TYPE_CALLBACK* pReleaseStaticTypeCallback, uint32_t flags );
        static void Unregister( const GameObjectType* pType );

        static GameObjectType* Find( Name typeName );

        static ConstIterator GetTypeBegin();
        static ConstIterator GetTypeEnd();

        static void Shutdown();
        //@}

    private:
        /// Cached from the null-terminated name string in Type.
        mutable Name m_cachedName;
        /// Default template object for this type.
        mutable GameObjectPtr m_spTemplate;

        /// Static type release callback.
        RELEASE_STATIC_TYPE_CALLBACK* m_pReleaseStaticTypeCallback;

        /// Type flags.
        uint32_t m_flags;

        /// Main package containing all template objects.
        static PackagePtr sm_spTypePackage;
        /// Type lookup hash map instance.
        static LookupMap* sm_pLookupMap;
    };
}

#include "Engine/GameObjectType.inl"

#endif  // LUNAR_ENGINE_GAME_OBJECT_TYPE_H
