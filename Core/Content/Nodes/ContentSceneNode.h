#pragma once

#include "Core/API.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/Component/ComponentCollection.h"
#include "Foundation/TUID.h"

namespace Helium
{
    namespace Content
    {
        class Scene;

        //
        // This is the base class of all nodes in the XML hierarchy.
        //  It encapsulates the ID (TUID/Uuid) of each SceneNode.
        //

        class CORE_API SceneNode : public Component::ComponentCollection
        {
        public:
            // The ID of the node
            Helium::TUID m_ID;

            // A generated name of the node
            tstring m_DefaultName;

            // A user created name (can be empty)
            tstring m_GivenName;

            // Should the name change when the object does?
            bool m_UseGivenName;

            // utility
            bool m_Selected;

            SceneNode ()
                : m_ID (Helium::TUID::Generate())
                , m_Selected( false )
                , m_UseGivenName( false )
            {

            }

            SceneNode (const Helium::TUID& id)
                : m_ID (id)
                , m_Selected( false )
                , m_UseGivenName( false )
            {

            }

            REFLECT_DECLARE_ABSTRACT(SceneNode, Component::ComponentCollection);

            static void EnumerateClass( Reflect::Compositor<SceneNode>& comp );

            // migrate m_Name
            virtual bool ProcessComponent( Reflect::ElementPtr element, const tstring& fieldName ) HELIUM_OVERRIDE;

            // invalidates class attributes
            virtual bool ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const HELIUM_OVERRIDE;

            const tstring& GetName() const
            {
                return (!m_GivenName.empty() && m_UseGivenName) ? m_GivenName : m_DefaultName;
            }
        };

        typedef Helium::SmartPtr<SceneNode> SceneNodePtr;
        typedef std::vector<SceneNodePtr> V_SceneNode;
    }
}