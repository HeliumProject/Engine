#pragma once

#include "Engine/Engine.h"

#include "Platform/String.h"
#include "Foundation/Name.h"
#include "Foundation/HashMap.h"
#include "Reflect/Class.h"
#include "Reflect/ReflectionInfo.h"
#include "Engine/GameObject.h"
#include "Engine/Package.h"

namespace Helium
{
    class Package;
    typedef Helium::StrongPtr< Package > PackagePtr;
    typedef Helium::StrongPtr< const Package > ConstPackagePtr;

    class GameObjectType;
    typedef SmartPtr< GameObjectType > GameObjectTypePtr;

    /// Run-time type information for GameObject classes.
    class HELIUM_ENGINE_API GameObjectType : public Helium::AtomicRefCountBase< GameObjectType >
    {
    public:
        /// Type lookup hash map.
        typedef HashMap< Name, GameObjectTypePtr > LookupMap;

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
        class HELIUM_ENGINE_API ConstIterator
        {
            friend class GameObjectType;

        public:
            /// @name Construction/Destruction
            //@{
            inline ConstIterator();
            //@}

            /// @name Overloaded Operators
            //@{
            inline const GameObjectType& operator*() const;
            inline const GameObjectType* operator->() const;

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
        inline const Reflect::Class* GetClass() const;
        inline const GameObjectType* GetBaseType() const;
        inline GameObject* GetTemplate() const;

        inline uint32_t GetFlags() const;
        //@}

        /// @name Static Type Registration
        //@{
        inline static Package* GetTypePackage();
        static void SetTypePackage( Package* pPackage );

        static GameObjectType* Create( const Reflect::Class* pClass, Package* pTypePackage, const GameObjectType* pParent, GameObject* pTemplate, uint32_t flags );
        static void Unregister( const GameObjectType* pType );

        static GameObjectType* Find( Name typeName );

        static ConstIterator GetTypeBegin();
        static ConstIterator GetTypeEnd();

        static void Shutdown();
        //@}

    private:
        /// Reflection class information.
        const Reflect::Class* m_class;
        /// Name table entry for this type.
        Name m_name;
        /// Type flags.
        uint32_t m_flags;

        /// Main package containing all template objects.
        static PackagePtr sm_spTypePackage;
        /// Type lookup hash map instance.
        static LookupMap* sm_pLookupMap;
    };
}

#include "Engine/GameObjectType.inl"
