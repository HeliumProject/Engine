//----------------------------------------------------------------------------------------------------------------------
// Type.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_TYPE_H
#define LUNAR_ENGINE_TYPE_H

#include "Foundation/Name.h"
#include "Foundation/Container/ConcurrentHashMap.h"
#include "Engine/GameObject.h"

namespace Lunar
{
    /// Run-time type information for GameObject classes.
    class LUNAR_ENGINE_API Type : public GameObject
    {
        L_DECLARE_OBJECT( Type, GameObject );

    public:
        /// Type lookup hash map.
        typedef ConcurrentHashMap< Name, Type* > LookupMap;

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
        ///
        /// This wraps a non-exclusive accessor to the type lookup map.  As such, multiple threads can iterate over the
        /// lookup map at the same time, but holding onto a valid iterator will block types from being registered or
        /// unregistered.
        class LUNAR_ENGINE_API ConstIterator : Lunar::NonCopyable
        {
            friend class Type;

        public:
            /// @name Accessor Information
            //@{
            inline bool IsValid() const;
            inline void Release();
            //@}

            /// @name Overloaded Operators
            //@{
            inline Type& operator*() const;
            inline Type* operator->() const;

            inline ConstIterator& operator++();
            inline ConstIterator& operator--();

            inline bool operator==( const ConstIterator& rOther ) const;
            inline bool operator!=( const ConstIterator& rOther ) const;
            //@}

        private:
            /// Type map accessor.
            LookupMap::ConstAccessor m_accessor;
        };

        /// @name Construction/Destruction
        //@{
        Type();
        virtual ~Type();
        //@}

        /// @name Initialization
        //@{
        bool Initialize( Type* pParent, GameObject* pTemplate, uint32_t flags );
        //@}

        /// @name Data Access
        //@{
        inline Type* GetTypeParent() const;
        inline GameObject* GetTypeTemplate() const;

        inline uint32_t GetTypeFlags() const;
        //@}

        /// @name Type Information
        //@{
        bool IsSubtypeOf( const Type* pType ) const;
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
        //@}

        /// @name Static Type Registration
        //@{
        inline static Package* GetTypePackage();
        static void SetTypePackage( Package* pPackage );

        static Type* Register( Type* pType );
        static void Unregister( Type* pType );

        static Type* Find( Name typeName );
        static Type* Find( GameObjectPath typePath );

        static bool GetFirstType( ConstIterator& rIterator );

        static void Shutdown();
        //@}

    private:
        /// Parent type.
        TypePtr m_spTypeParent;
        /// Default template object for this type.
        GameObjectPtr m_spTypeTemplate;

        /// Type flags.
        uint32_t m_typeFlags;

        /// Main package containing all Type objects.
        static PackagePtr sm_spTypePackage;
        /// Type lookup hash map instance.
        static LookupMap* sm_pLookupMap;
    };
}

#include "Engine/Type.inl"

#endif  // LUNAR_ENGINE_TYPE_H
