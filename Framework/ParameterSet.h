#pragma once

#ifndef HELIUM_FRAMEWORK_PARAMETER_SET_H
#define HELIUM_FRAMEWORK_PARAMETER_SET_H

#include "Foundation/DynamicArray.h"
#include "Platform/MemoryHeap.h"

namespace Helium
{
	// ParameterSets have strongly typed parameters, this gives us an easy interface to enumerate them so we can
	// dynamically bind data
	struct Parameter
	{
		Name                  GetName() { return m_Name; }
		Reflect::Translator*  GetTranslator() { return m_Translator; }
		Reflect::Pointer      GetPointer() { return m_Pointer; }

		Name                 m_Name;
		Reflect::Translator *m_Translator;
		Reflect::Pointer     m_Pointer;
	};

	typedef Helium::StrongPtr< class ParameterSet > ParameterSetPtr;

	// TODO: Special registration, possibly for precalculating data and verifying no incompatible parameters
	class HELIUM_FRAMEWORK_API ParameterSet : public Reflect::Object
	{
	public:
		HELIUM_DECLARE_ABSTRACT( Helium::ParameterSet, Reflect::Object );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		ParameterSet();
		void EnumerateParameters( DynamicArray<Parameter> &parameters ) const;

		template <class T>
		T *FindParameterSet();

	private:
		friend class ParameterSetBuilder;
		ParameterSetPtr m_NextParams;
	};

	class ParameterSetBuilder
	{
	public:
		ParameterSetBuilder( ParameterSet *parameterSet = 0)
			: m_ParameterSet( parameterSet )
		{
			
		}

		template <class T>
		T *AddParameterSet()
		{
			ParameterSetPtr temp = m_ParameterSet;
			m_ParameterSet = new T();
			m_ParameterSet->m_NextParams = temp;
			return static_cast<T *>(m_ParameterSet.Get());
		}

		ParameterSet *GetSet()
		{
			return m_ParameterSet.Get();
		}

	private:
		ParameterSetPtr m_ParameterSet;
	};

#if 0
	//////////////////////////////////////////////////////////////////////////
	// Easy way to combine parameter sets
	class HELIUM_FRAMEWORK_API ParameterSetDefinition : public Helium::Asset
	{
	public:
		HELIUM_DECLARE_ASSET( Helium::ParameterSetDefinition, Helium::Asset );
		static void PopulateStructure( Reflect::Structure& comp );

		DynamicArray<ParameterSet *> m_ParameterSets;
	};
#endif

	//////////////////////////////////////////////////////////////////////////
	// ParameterSet_InitLocated - Anything with a location will want to use
	// this parameter set
	struct HELIUM_FRAMEWORK_API ParameterSet_InitLocated : public ParameterSet
	{
	public:
		HELIUM_DECLARE_CLASS( Helium::ParameterSet_InitLocated, Helium::ParameterSet );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		Helium::Simd::Vector3 m_Position;
		Helium::Simd::Quat m_Rotation;
	};

	//////////////////////////////////////////////////////////////////////////
	// ParameterSet_InitPhysical - Anything with physics will want to use this
	// parameter set
	struct HELIUM_FRAMEWORK_API ParameterSet_InitPhysical : public ParameterSet
	{
	public:
		HELIUM_DECLARE_CLASS( Helium::ParameterSet_InitPhysical, Helium::ParameterSet );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		Helium::Simd::Vector3 m_Velocity;
	};

	//////////////////////////////////////////////////////////////////////////
	// Old Crap
#if 0
	struct ParameterBase
	{
		virtual ~IParameter() {}

		Name                  GetName() { return m_Name; }
		Reflect::Translator*  GetTranslator() { return m_Translator; }

		Name m_Name;
		Reflect::Translator *m_Translator;
	};

	template <class T>
	struct Parameter : public ParameterBase
	{
		virtual Reflect::Pointer GetPointer() { return Reflect::Pointer( &m_Value, NULL, NULL ); }

		T m_Value;
	};

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
		inline T &SetParameter(Name name, const T& value);

		static const ParameterSet EmptyParameterSet;

		static const Name ParameterNamePosition; // Simd::Vector3
		static const Name ParameterNameRotation; // Simd::Quat
		static const Name ParameterNameVelocity; // Simd::Vector3
	};
#endif
}

#include "Framework/ParameterSet.inl"

#endif