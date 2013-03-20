
#pragma once

#ifndef HELIUM_FRAMEWORK_PARAMETER_SET_H
#define HELIUM_FRAMEWORK_PARAMETER_SET_H

#include "Foundation/DynamicArray.h"

#include "Reflect/Data/Data.h"

namespace Helium
{
    class HELIUM_FRAMEWORK_API ParameterSet
    {
    public:
        struct IParameter
        {
            virtual Name GetName() = 0;
            virtual Reflect::DataPtr GetDataPtr() = 0;

            // This has to be here or I get warnings. TODO: Figure out what needs to be in this function if anything.
            void operator delete(void *, Helium::MemoryHeap &) { }
            virtual ~IParameter() { }
        };
    
        template <class T>
        struct Parameter : public IParameter
        {
            virtual ~Parameter() { }

            virtual Name GetName() { return m_Name; }
            virtual Reflect::DataPtr GetDataPtr();

            Name m_Name;
            T m_Parameter;
        };

        ParameterSet(size_t _block_size = 128);
        ~ParameterSet();

        DynamicArray<IParameter *> m_Parameters;
        Helium::StackMemoryHeap<> m_Heap;

        template <class T>
        inline void SetParameter(Name name, T value);
    };
}

#include "Framework/ParameterSet.h"

#endif