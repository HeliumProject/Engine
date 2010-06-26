#pragma once

#include "API.h"
#include "Stream.h" 

namespace Reflect
{
    template< class C >
    class Indent
    {
    private:
        static C m_Space;

        // Indent spacing
        unsigned int m_Indent;

    public:
        Indent()
            : m_Indent (0)
        {

        }

        void Push()
        {
            m_Indent++;
        }

        void Pop()
        {
            if (m_Indent > 0)
                m_Indent--;
        }

        void Get(Stream<C>& stream)
        {
            if (m_Indent > 0)
            {
                int indent = m_Indent;

                while(indent > 0)
                {
                    stream << m_Space;
                    indent--;
                }
            }
        }

        void Get(FILE* file)
        {
            if (file != NULL && m_Indent > 0)
            {
                int indent = m_Indent;

                while(indent > 0)
                {
                    fputc( m_Space, file );
                    indent--;
                }
            }
        }
    };
}