#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Foundation/SmartPtr.h"

#include <vector>

namespace Helium
{
    //////////////////////////////////////////////////////////////////////////
    //
    // C++ Delegate/Event System
    //
    // Classes:
    //
    //  Signature is a template that contains all classes for a given function
    //   signature.  Typedef Signature as a starting point for use in code.
    //
    //  Delegate is an encapsulation of a function that matches the signature.
    //   Delegate::Function delegates invocation to a standard C function.
    //   Delegate::Method delegates invocation to a member function.
    //
    //  Event is a set of delegates that are invoked together.
    //
    // Comments:
    //
    //  The reliance on internally allocated worker (Impl) classes is there to
    //   allow for experimentation with memory allocation without having to
    //   refactor tons of code to test changes.  Since the allocation is masked
    //   client code will never call new itself.  There is lots of overloading
    //   and template member functions to support as much efficiency as possible.
    //   Utilize template member functions over Delegate prototypes to prevent
    //   needless heap thrashing (creation, comparison, and deletion of internal
    //   DelegateImpl objects).
    //
    // Usage:
    //
    //  struct Args {};
    //
    //  void Function( Args args ) {}
    //
    //  class Foo
    //  {
    //    void Method( Args args ) {}
    //  };
    //
    //  // this explicitly instantiates all the classes required for a signature
    //  typedef Helium::Signature<Args> ExampleSignature;
    //
    //  ExampleSignature::Event g_Event;
    //  Foo                     g_Foo;
    //
    //  g_Event.Add( &Function  );
    //  g_Event.Add( &g_Foo, &Foo::Method ) );
    //  g_Event.Raise( Args () );
    //  g_Event.Remove( &Function  );
    //  g_Event.Remove( &g_Foo, &Foo::Method ) );
    //
    // To Do:
    //
    //  * Add support for stl or 'Helium' allocators in place of C++ heap
    //  * Elminate heap usage in Delegate with some horrific unions:
    //     http://www.codeproject.com/cpp/fastdelegate2.asp
    //
    //////////////////////////////////////////////////////////////////////////

    //
    // This is provided as a placeholder because 'void' is not instantiatable in C++
    //

    enum Void {};

    //
    // The different delegate types that are supported, probably won't change very often
    //

    namespace DelegateTypes
    {
        enum DelegateType
        {
            Function,
            Method,
        };
    }
    typedef DelegateTypes::DelegateType DelegateType;

    //
    // Delegate encapsulates and a function call of multiple types
    //

    template< typename ArgsType, template< typename T > class RefCountBaseType = RefCountBase >
    class Delegate
    {
    public:
        Delegate();

        Delegate( const Delegate& rhs );

        template < typename FunctionType >
        Delegate( FunctionType function );

        template < class ClassType, typename MethodType >
        Delegate( ClassType* instance, MethodType method );

        template < typename FunctionType >
        inline static Delegate Create( FunctionType function );

        template < class ClassType, typename MethodType >
        inline static Delegate Create( ClassType* instance, MethodType method );

        void Clear();
        bool Valid() const;

        void Set( const Delegate& delegate );
        template < typename FunctionType >
        void Set( FunctionType function );
        template < class ClassType, typename MethodType >
        void Set( ClassType* instance, MethodType method );

        bool Equals( const Delegate& rhs ) const;
        template <typename FunctionType>
        bool Equals( FunctionType function ) const;
        template <class ClassType, typename MethodType>
        bool Equals( const ClassType* instance, MethodType method ) const;

        void Invoke( ArgsType parameter ) const;

    private:
        //
        // DelegateImpl implements the guts of Delegate and is heap allocated and reference counted.
        //  Its defines an interface (pure virtual) that must be implemented by derived classes
        //

        class DelegateImpl : public RefCountBaseType< DelegateImpl >
        {
        private:
            friend class Delegate;

        public:
            // Deduce the type of this delegate
            virtual DelegateType GetType() const = 0;

            // Equality checking (so we don't add the same one twice)
            virtual bool Equals( const DelegateImpl* rhs ) const = 0;

            // The type-safe entry point for invoking the event handlers this Delegate encapsulates
            virtual void Invoke ( ArgsType parameter ) const = 0;
        };

        //
        // Function implements Delegate for a static function
        //

        class Function : public DelegateImpl
        {
        public:
            typedef void (*FunctionType)(ArgsType);

			Function( FunctionType function );

            virtual DelegateType GetType() const;
            virtual bool Equals( const DelegateImpl* rhs ) const;
            virtual void Invoke( ArgsType parameter ) const;

        private:
            FunctionType m_Function;

            friend class Delegate;
		};

        //
        // Method implements Delegate for a member function of an instance of a class or struct
        //

        template<class ClassType>
        class Method : public DelegateImpl
        {
        public:
            typedef void (ClassType::*MethodType)(ArgsType);

			Method( ClassType* instance, MethodType method );

            virtual DelegateType GetType() const;
            virtual bool Equals( const DelegateImpl* rhs ) const;
            virtual void Invoke( ArgsType parameter ) const;

		private:
            ClassType* m_Instance;
            MethodType m_Method;

			friend class Delegate;
        };

        Helium::SmartPtr< DelegateImpl > m_Impl;
	};

    //
    // Event is a collection of delegates that are invoked together
    //

    template< typename ArgsType, template< typename T > class RefCountBaseType = RefCountBase >
    class Event
    {
    public:
        typedef Helium::Delegate< ArgsType, RefCountBaseType > Delegate;

        uint32_t Count() const;
        bool Valid() const;

        void Add( const Delegate& delegate );
        template < typename FunctionType >
        void AddFunction( FunctionType function );
        template < class ClassType, typename MethodType >
        void AddMethod( ClassType* instance, MethodType method );

        void Remove( const Delegate& delegate );
        template < typename FunctionType >
        void RemoveFunction( FunctionType function );
        template < class ClassType, typename MethodType >
        void RemoveMethod( const ClassType* instance, MethodType method );

        void Raise( ArgsType parameter );
        void RaiseWithEmitter( ArgsType parameter, const Delegate& emitter );

    private:

        //
        // EventImpl implements the guts of Event and is heap allocated and reference counted.
        //  The choice to make this heap allocated is so that we can handled the "owner" of the
        //  event being destroyed while the event is raised while at the same time supporting the
        //  removal of delegates from the event.
        //

        class EventImpl : public RefCountBaseType< EventImpl >
        {
        public:
            EventImpl();

            // Query for count
            uint32_t Count() const;

            // Compact dead pointers caused by Remove() inside Raise()
            void Compact();

            // Add the delegate function to the list
            void Add( const Delegate& delegate );
            template < typename FunctionType >
            void AddFunction( FunctionType function );
            template < class ClassType, typename MethodType >
            void AddMethod( ClassType* instance, MethodType method );

            // Remove the delegate function from the list
            void Remove( const Delegate& delegate );
            template < typename FunctionType >
            void RemoveFunction( FunctionType function );
            template < class ClassType, typename MethodType >
            void RemoveMethod( const ClassType* instance, MethodType method );

            // Invoke all of the delegates for this event occurrence. Pays no mind about the return value of the invocation
            void Raise( ArgsType parameter, const Delegate& emitter );

        private:
            std::vector<Delegate>   m_Delegates;
            uint32_t                     m_EntryCount;
            uint32_t                     m_EmptySlots;
        };

        Helium::SmartPtr< EventImpl > m_Impl;
    };

    //
    // Signature intantiates all the template classes necessary for working with a particular signature
    //

    template< typename ArgsType, template< typename T > class RefCountBaseType = RefCountBase >
    class Signature
    {
    public:
        typedef Helium::Delegate< ArgsType, RefCountBaseType >  Delegate;
        typedef Helium::Event< ArgsType, RefCountBaseType >     Event;
    };

    typedef Helium::Signature<Helium::Void> VoidSignature;
}

#include "Foundation/Event.inl"