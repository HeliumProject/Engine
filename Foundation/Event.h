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

    class Void
    {

    };

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
        private:
            friend class Delegate;

            typedef void (*FunctionType)(ArgsType);

            FunctionType m_Function;

        public:
            Function( FunctionType function )
                : m_Function( function )
            {
                HELIUM_ASSERT( function );
            }

            virtual DelegateType GetType() const
            {
                return DelegateTypes::Function;
            }

            virtual bool Equals( const DelegateImpl* rhs ) const
            {
                if ( GetType() != rhs->GetType() )
                {
                    return false;
                }

                const Function* f = static_cast<const Function*>(rhs);

                return m_Function == f->m_Function;
            }

            virtual void Invoke( ArgsType parameter ) const
            {
                m_Function( parameter );
            }
        };

        //
        // Method implements Delegate for a member function of an instance of a class or struct
        //

        template<class ClassType>
        class Method : public DelegateImpl
        {
        private:
            friend class Delegate;

            typedef void (ClassType::*MethodType)(ArgsType);
            ClassType* m_Instance;
            MethodType m_Method;

        public:
            Method( ClassType* instance, MethodType method )
                : m_Instance( instance )
                , m_Method( method )
            {
                HELIUM_ASSERT( instance );
                HELIUM_ASSERT( method );
            }

            virtual DelegateType GetType() const
            {
                return DelegateTypes::Method;
            }

            virtual bool Equals( const DelegateImpl* rhs ) const
            {
                if ( GetType() != rhs->GetType() )
                {
                    return false;
                }

                const Method* m = static_cast<const Method*>(rhs);

                return m_Instance == m->m_Instance && m_Method == m->m_Method;
            }

            virtual void Invoke( ArgsType parameter ) const
            {
                (m_Instance->*m_Method)( parameter );
            }
        };

        Helium::SmartPtr< DelegateImpl > m_Impl;

    public:
        Delegate()
        {

        }

        Delegate( const Delegate& rhs )
            : m_Impl( rhs.m_Impl )
        {

        }

        template < typename FunctionType >
        Delegate( FunctionType function )
        {
            m_Impl = new Function (function);
        }

        template < class ClassType, typename MethodType >
        Delegate( ClassType* instance, MethodType method )
        {
            m_Impl = new Method<ClassType> (instance, method);
        }

        template < typename FunctionType >
        inline static Delegate Create( FunctionType function )
        {
            return Delegate(function);
        }

        template < class ClassType, typename MethodType >
        inline static Delegate Create( ClassType* instance, MethodType method )
        {
            return Delegate (instance, method);
        }

        void Clear()
        {
            m_Impl = NULL;
        }

        bool Valid() const
        {
            return m_Impl.ReferencesObject();
        }

        void Set( const Delegate& delegate )
        {
            m_Impl = delegate.m_Impl;
        }

        template < typename FunctionType >
        void Set( FunctionType function )
        {
            m_Impl = new Function (function);
        }

        template < class ClassType, typename MethodType >
        void Set( ClassType* instance, MethodType method )
        {
            m_Impl = new Method<ClassType> (instance, method);
        }

        bool Equals( const Delegate& rhs ) const
        {
            if (m_Impl.ReferencesObject() != rhs.m_Impl.ReferencesObject())
            {
                return false;
            }

            if (!m_Impl.ReferencesObject())
            {
                return false;
            }

            return m_Impl->Equals( rhs.m_Impl );
        }

        template <typename FunctionType>
        bool Equals( FunctionType function ) const
        {
            if (m_Impl.ReferencesObject() && m_Impl->GetType() == DelegateTypes::Function)
            {
                Function* func = static_cast<Function*>(m_Impl.Ptr());

                return func->m_Function == function;
            }
            else
            {
                return false;
            }
        }

        template <class ClassType, typename MethodType>
        bool Equals( const ClassType* instance, MethodType method ) const
        {
            if (m_Impl.ReferencesObject() && m_Impl->GetType() == DelegateTypes::Method)
            {
                Method<ClassType>* meth = static_cast<Method<ClassType>*>(m_Impl.Ptr());

                return meth->m_Instance == instance && meth->m_Method == method;
            }
            else
            {
                return false;
            }
        }

        void Invoke( ArgsType parameter ) const
        {
            if (m_Impl.ReferencesObject())
            {
                m_Impl->Invoke(parameter);
            }
        }
    };

    //
    // Event is a collection of delegates that are invoked together
    //

    template< typename ArgsType, template< typename T > class RefCountBaseType = RefCountBase >
    class Event
    {
    public:
        typedef Helium::Delegate< ArgsType, RefCountBaseType > Delegate;

        uint32_t Count() const
        {
            return m_Impl.ReferencesObject() ? (uint32_t)m_Impl->Count() : 0;
        }

        bool Valid() const
        {
            return Count() > 0;
        }

        void Add( const Delegate& delegate )
        {
            if ( !m_Impl.ReferencesObject() )
            {
                m_Impl = new EventImpl;
            }

            m_Impl->Add( delegate );
        }

        template < typename FunctionType >
        void AddFunction( FunctionType function )
        {
            if ( !m_Impl.ReferencesObject() )
            {
                m_Impl = new EventImpl;
            }

            m_Impl->AddFunction( function );
        }

        template < class ClassType, typename MethodType >
        void AddMethod( ClassType* instance, MethodType method )
        {
            if ( !m_Impl.ReferencesObject() )
            {
                m_Impl = new EventImpl;
            }

            m_Impl->AddMethod( instance, method );
        }

        void Remove( const Delegate& delegate )
        {
            if ( m_Impl.ReferencesObject() )
            {
                m_Impl->Remove( delegate );

                if (m_Impl->Count() == 0)
                {
                    m_Impl = NULL;
                }
            }
        }

        template < typename FunctionType >
        void RemoveFunction( FunctionType function )
        {
            if ( m_Impl.ReferencesObject() )
            {
                m_Impl->RemoveFunction( function );

                if (m_Impl->Count() == 0)
                {
                    m_Impl = NULL;
                }
            }
        }

        template < class ClassType, typename MethodType >
        void RemoveMethod( const ClassType* instance, MethodType method )
        {
            if ( m_Impl.ReferencesObject() )
            {
                m_Impl->RemoveMethod( instance, method );

                if (m_Impl->Count() == 0)
                {
                    m_Impl = NULL;
                }
            }
        }

        void Raise( ArgsType parameter )
        {
            if ( m_Impl.ReferencesObject() )
            {
                // hold a pointer on the stack in case the object we are aggregated into deletes inside this function
                // use impl and not m_Impl in case _we_ are deleted and m_Impl is trashed
                Helium::SmartPtr<EventImpl> impl = m_Impl;

                return impl->Raise( parameter, Delegate () );
            }

            return void ();
        }

        void RaiseWithEmitter( ArgsType parameter, const Delegate& emitter )
        {
            if ( m_Impl.ReferencesObject() )
            {
                // hold a pointer on the stack in case the object we are aggregated into deletes inside this function
                // use impl and not m_Impl in case _we_ are deleted and m_Impl is trashed
                Helium::SmartPtr<EventImpl> impl = m_Impl;

                return impl->Raise( parameter, emitter );
            }

            return void ();
        }

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
            EventImpl()
                : m_EntryCount (0)
                , m_EmptySlots (0)
            {

            }

            //
            // Query for count
            //

            uint32_t Count() const
            {
                return (uint32_t)m_Delegates.size();
            }

            //
            // Compact dead pointers caused by Remove() inside Raise()
            //

            void Compact()
            {
                if (m_EmptySlots)
                {
                    typename std::vector<Delegate>::iterator itr = m_Delegates.begin();
                    typename std::vector<Delegate>::iterator end = m_Delegates.end();
                    for ( uint32_t slotsLeft = m_EmptySlots; itr != end && slotsLeft; ++itr )
                    {
                        if ( !itr->Valid() )
                        {
                            typename std::vector<Delegate>::iterator next = itr + 1;
                            for ( ; next != end; ++next )
                            {
                                if (next->Valid())
                                {
                                    *itr = *next;
                                    next->Clear();
                                    --slotsLeft;
                                    break;
                                }
                            }
                        }
                    }

                    m_Delegates.resize( m_Delegates.size() - m_EmptySlots );
                    m_EmptySlots = 0;
                }
            }

            //
            // Add the delegate function to the list
            //

            void Add( const Delegate& delegate )
            {
                typename std::vector<Delegate>::const_iterator itr = m_Delegates.begin();
                typename std::vector<Delegate>::const_iterator end = m_Delegates.end();
                for ( ; itr != end; ++itr )
                {
                    if ( itr->Valid() && itr->Equals(delegate) )
                    {
                        return;
                    }
                }

                m_Delegates.push_back( delegate );
            }

            template < typename FunctionType >
            void AddFunction( FunctionType function )
            {
                typename std::vector<Delegate>::const_iterator itr = m_Delegates.begin();
                typename std::vector<Delegate>::const_iterator end = m_Delegates.end();
                for ( ; itr != end; ++itr )
                {
                    if ( itr->Valid() && itr->Equals(function) )
                    {
                        return;
                    }
                }

                m_Delegates.push_back( Delegate (function) );
            }

            template < class ClassType, typename MethodType >
            void AddMethod( ClassType* instance, MethodType method )
            {
                typename std::vector<Delegate>::const_iterator itr = m_Delegates.begin();
                typename std::vector<Delegate>::const_iterator end = m_Delegates.end();
                for ( ; itr != end; ++itr )
                {
                    if ( itr->Valid() && itr->Equals(instance, method ))
                    {
                        return;
                    }
                }

                m_Delegates.push_back( Delegate (instance, method) );
            }

            //
            // Remove the delegate function from the list
            //

            void Remove( const Delegate& delegate )
            {
                typename std::vector<Delegate>::iterator itr = m_Delegates.begin();
                typename std::vector<Delegate>::iterator end = m_Delegates.end();
                for ( ; itr != end; ++itr )
                {
                    if ( itr->Valid() && itr->Equals(delegate) )
                    {
                        if ( GetRefCount() == 1 )
                        {
                            m_Delegates.erase( itr );
                        }
                        else
                        {
                            m_EmptySlots++;
                            itr->Clear();
                        }
                        break;
                    }
                }
            }

            template < typename FunctionType >
            void RemoveFunction( FunctionType function )
            {
                typename std::vector<Delegate>::iterator itr = m_Delegates.begin();
                typename std::vector<Delegate>::iterator end = m_Delegates.end();
                for ( ; itr != end; ++itr )
                {
                    if ( itr->Valid() && itr->Equals(function) )
                    {
                        if ( GetRefCount() == 1 )
                        {
                            m_Delegates.erase( itr );
                        }
                        else
                        {
                            m_EmptySlots++;
                            itr->Clear();
                        }
                        break;
                    }
                }
            }

            template < class ClassType, typename MethodType >
            void RemoveMethod( const ClassType* instance, MethodType method )
            {
                typename std::vector<Delegate>::iterator itr = m_Delegates.begin();
                typename std::vector<Delegate>::iterator end = m_Delegates.end();
                for ( ; itr != end; ++itr )
                {
                    if ( itr->Valid() && itr->Equals(instance, method) )
                    {
                        if ( GetRefCount() == 1 )
                        {
                            m_Delegates.erase( itr );
                        }
                        else
                        {
                            m_EmptySlots++;
                            itr->Clear();
                        }
                        break;
                    }
                }
            }

            //
            // Invoke all of the delegates for this event occurrence
            //  Pays no mind about the return value of the invocation
            //

            void Raise( ArgsType parameter, const Delegate& emitter )
            {
                ++m_EntryCount;

                for ( size_t i=0; i<m_Delegates.size(); ++i )
                {
                    Delegate& d ( m_Delegates[i] );

                    if ( !d.Valid() || ( emitter.Valid() && emitter.Equals( d ) ) )
                    {
                        continue;
                    }

                    d.Invoke(parameter); 
                }

                if ( --m_EntryCount == 0 )
                {
                    Compact();
                }
            }

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
