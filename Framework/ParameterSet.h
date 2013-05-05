
#pragma once

#ifndef HELIUM_FRAMEWORK_PARAMETER_SET_H
#define HELIUM_FRAMEWORK_PARAMETER_SET_H

#include "Foundation/DynamicArray.h"

#include "Reflect/DataDeduction.h"

namespace Helium
{
	class HELIUM_FRAMEWORK_API ParameterSet
	{
	public:
		struct IParameter
		{
			virtual ~IParameter() {}
			virtual Name GetName() = 0;
			virtual Reflect::Data* GetData() = 0;
			virtual Reflect::DataPointer GetPointer() = 0;
		};
	
		template <class T>
		struct Parameter : public IParameter
		{
			virtual ~Parameter() { }

			virtual Name GetName() { return m_Name; }
			virtual Reflect::Data* GetData();
			virtual Reflect::DataPointer GetPointer();

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