
#pragma once

#ifndef HELIUM_FRAMEWORK_PARAMETER_SET_H
#define HELIUM_FRAMEWORK_PARAMETER_SET_H

#include "Foundation/DynamicArray.h"
#include "Platform/MemoryHeap.h"

namespace Helium
{
	class HELIUM_FRAMEWORK_API ParameterSet
	{
	public:
		struct IParameter
		{
			virtual ~IParameter() {}

			virtual Name                  GetName() = 0;
			virtual Reflect::Translator*  GetTranslator() = 0;
			virtual Reflect::Pointer      GetPointer() = 0;
		};
	
		template <class T>
		struct Parameter : public IParameter
		{
			virtual ~Parameter() { delete m_Translator; }

			virtual Name                  GetName() { return m_Name; }
			virtual Reflect::Translator*  GetTranslator() { return m_Translator; }
			virtual Reflect::Pointer      GetPointer() { return Reflect::Pointer( &m_Value, NULL, NULL ); }

			Name          m_Name;
			T             m_Value;
			Reflect::Translator *  m_Translator;
		};

		ParameterSet(size_t _block_size = 128);
		~ParameterSet();

		DynamicArray<IParameter *> m_Parameters;

#if HELIUM_HEAP
		Helium::StackMemoryHeap<> m_ParameterAllocator;
#else
		Helium::DefaultAllocator m_ParameterAllocator;
#endif

		template <class T>
		inline T &SetParameter(Name name, T value);

		static const ParameterSet EmptyParameterSet;

		static const Name ParameterNamePosition; // Simd::Vector3
		static const Name ParameterNameRotation; // Simd::Quat
	};
}

#include "Framework/ParameterSet.inl"

#endif