#pragma once

#include "Core/Content/Nodes/ContentSceneNode.h"
#include "Foundation/TUID.h"

namespace Helium
{
    namespace Content
    {
        //
        // Shader stores UV modifiers, base material color
        //

        class CORE_API Shader : public SceneNode
        {
        public:
            // Does the texture wrap horizontally
            bool m_WrapU;

            // Does the texture wrap vertically
            bool m_WrapV;

            // Repetition frequency horizontally
            float m_RepeatU;

            // Repetition frequency vertically
            float m_RepeatV;

            // Base material color (128 bit)
            Math::Vector4 m_BaseColor;

            // The file for additional shader details
            Helium::Path m_AssetPath;

            Shader ()
                : m_WrapU (true)
                , m_WrapV (true)
                , m_RepeatU (1.0)
                , m_RepeatV (1.0)
            {
            }

            Shader (const Helium::TUID& id)
                : SceneNode (id)
                , m_WrapU (true)
                , m_WrapV (true)
                , m_RepeatU (1.0)
                , m_RepeatV (1.0)
            {
            }

        public:

            REFLECT_DECLARE_CLASS(Shader, SceneNode);

            static void EnumerateClass( Reflect::Compositor<Shader>& comp );
        };

        typedef Helium::SmartPtr<Shader> ShaderPtr;
        typedef std::vector<ShaderPtr> V_Shader;

        struct CompareShaders
        {
            bool operator ()( const ShaderPtr& lhs, const ShaderPtr& rhs )
            {
                return lhs->m_ID < rhs->m_ID;
            }    
        };
        typedef std::set<ShaderPtr> S_Shader;
    }
}