#include "stdafx.h"
#include "Indent.h"

using namespace Reflect;

void Indent::Push()
{
    m_Indent++;
}

void Indent::Pop()
{
    if (m_Indent > 0)
        m_Indent--;
}

void Indent::Get(Stream& stream)
{
    if (m_Indent > 0)
    {
        int indent = m_Indent;

        while(indent > 0)
        {
            stream << " ";
            indent--;
        }
    }
}

void Indent::Get(FILE* file)
{
    if (file != NULL && m_Indent > 0)
    {
        int indent = m_Indent;

        while(indent > 0)
        {
            fprintf(file, " ");
            indent--;
        }
    }
}
