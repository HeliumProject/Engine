#pragma once

#include "Foundation/Math/Matrix4.h"

#include <vector>

namespace Helium
{
    namespace Core
    {
        struct VisitorState
        {
            Math::Matrix4 m_Matrix;
            bool m_Highlighted;
            bool m_Selected;
            bool m_Live;
            bool m_Selectable;

            VisitorState()
                : m_Highlighted (false)
                , m_Selected (false)
                , m_Live (false)
                , m_Selectable (false)
            {

            }

            VisitorState(const Math::Matrix4& matrix)
                : m_Matrix (matrix)
                , m_Highlighted (false)
                , m_Selected (false)
                , m_Live (false)
                , m_Selectable (false)
            {

            }

            VisitorState(const Math::Matrix4& matrix, bool highlighted, bool selected, bool live, bool selectable)
                : m_Matrix (matrix)
                , m_Highlighted (highlighted)
                , m_Selected (selected)
                , m_Live (live)
                , m_Selectable (selectable)
            {

            }
        };

        class Visitor
        {
        protected:
            // states for handling recursion
            std::vector< VisitorState > m_States;

        public:
            Visitor()
            {
                m_States.push_back( VisitorState (Math::Matrix4::Identity) );
            }

            virtual ~Visitor()
            {
                HELIUM_ASSERT( m_States.size() == 1 );
            }

            VisitorState& State()
            {
                return m_States.back();
            }

            const VisitorState& ParentState() const
            {
                const static VisitorState defaultState;

                if ( m_States.size() > 1 )
                {
                    return m_States.at( m_States.size() - 2 );
                }
                else
                {
                    return defaultState;
                }
            }

            void PushState( const VisitorState& state )
            {
                m_States.push_back( state );
            }

            void PopState()
            {
                m_States.pop_back();
            }
        };
    }
}