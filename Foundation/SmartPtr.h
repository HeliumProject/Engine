#pragma once

#include "Platform/Atomic.h"
#include "Platform/Utility.h"

namespace Helium
{
	//
	// This is a simple manager for a C-style array allocated with new [].
	//  We don't use auto_ptr because of the delete semantics. 
	//  This class is probably only good for the simplest use cases.
	// 
	// example: 
	//   AutoPtr<Foo> foo( new Foo ); 
	// 

	template <typename T>
	struct AutoPtr
	{
	public:
		AutoPtr( T* ptr = NULL );

	private:
		AutoPtr( const AutoPtr& rhs );

	public:
		~AutoPtr();

		T* Ptr();

		const T* operator->() const;
		T* operator->();
		T &operator*();

		void Reset(T *_ptr);
		T* Release();

	private: 
		T* m_Ptr; 
	};
	
	//
	// This is a simple manager for a C-style array allocated with new [].
	//  We don't use auto_ptr because of the delete semantics. 
	//  This class is probably only good for the simplest use cases.
	// 
	// example: 
	//   ArrayPtr<char> array( new char[24] ); 
	// 

	template <typename T>
	struct ArrayPtr
	{
	public: 
		ArrayPtr( T* ptr = NULL );

	private:
		ArrayPtr( const ArrayPtr& rhs );

	public:
		~ArrayPtr();

		T* Ptr();

		const T& operator[]( size_t i ) const;
		T& operator[]( size_t i );

	private: 
		T* m_Ptr; 
	};

	/// Base class for non-atomic reference counting support.
	template< typename T >
	class RefCountBase
	{
	private:
		/// Reference count.
		mutable uint32_t m_RefCount;

	public:
		/// @name Construction/Destruction
		//@{
		RefCountBase();
		RefCountBase( const RefCountBase& rSource );
		//@}

		/// @name Reference Counting
		//@{
		uint32_t GetRefCount() const;
		uint32_t IncrRefCount() const;
		uint32_t DecrRefCount() const;
		//@}

		/// @name Overloaded Operators
		//@{
		RefCountBase& operator=( const RefCountBase& rSource );
		//@}
	};

	/// Base class for atomic reference counting support.
	template< typename T >
	class AtomicRefCountBase
	{
	private:
		/// Reference count.
		mutable volatile int32_t m_RefCount;

	public:
		/// @name Construction/Destruction
		//@{
		AtomicRefCountBase();
		AtomicRefCountBase( const AtomicRefCountBase& rSource );
		virtual ~AtomicRefCountBase();
		//@}

		/// @name Reference Counting
		//@{
		uint32_t GetRefCount() const;
		uint32_t IncrRefCount() const;
		uint32_t DecrRefCount() const;
		//@}

		/// @name Overloaded Operators
		//@{
		AtomicRefCountBase& operator=( const AtomicRefCountBase& rSource );
		//@}
	};

	/// SmartPtr safely manages the reference count on the stack.
	template< typename T >
	class SmartPtr
	{
		template< typename U > friend class SmartPtr;

	public:
		/// @name Construction/Destruction
		//@{
		SmartPtr();
		SmartPtr( const T* pPointer );
		SmartPtr( const SmartPtr& rPointer );
		template< typename U > SmartPtr( const SmartPtr< U >& rPointer );
		~SmartPtr();
		//@}

		/// @name Data Access
		//@{
		T* Get() const;
		T* Ptr() const;
		void Set( const T* pResource );
		void Release();

		bool ReferencesObject() const;
		//@}

		/// @name Overloaded Operators
		//@{
		T& operator*() const;
		T* operator->() const;

		operator T* const&() const;

		SmartPtr& operator=( const T* pPointer );
		SmartPtr& operator=( const SmartPtr& rPointer );
		template< typename U > SmartPtr& operator=( const SmartPtr< U >& rPointer );
		//@}

	private:
		/// Object referenced by this smart pointer.
		T* m_Pointer;
	};

	/// DeepCompareSmartPtr is used in containers to compare what's being pointed to by the SmartPtrs rather than the pointers themselves
	template < typename T >
	class DeepCompareSmartPtr : public SmartPtr< T >
	{
	public:
		DeepCompareSmartPtr();
		DeepCompareSmartPtr( const T* pPointer );
		DeepCompareSmartPtr( const Helium::SmartPtr<T>& pPointer );

		inline bool operator<( const DeepCompareSmartPtr& rhs ) const;
		inline bool operator==( const DeepCompareSmartPtr& rhs ) const;
	};
}

#include "Foundation/SmartPtr.inl"
