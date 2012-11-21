template< typename ArgsType, template< typename T > class RefCountBaseType >
Helium::Delegate< ArgsType, RefCountBaseType >::Delegate()
{

}

template< typename ArgsType, template< typename T > class RefCountBaseType >
Helium::Delegate< ArgsType, RefCountBaseType >::Delegate( const Delegate& rhs )
	: m_Impl( rhs.m_Impl )
{

}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template < typename FunctionType >
Helium::Delegate< ArgsType, RefCountBaseType >::Delegate( FunctionType function )
{
	m_Impl = new Function (function);
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template < class ClassType, typename MethodType >
Helium::Delegate< ArgsType, RefCountBaseType >::Delegate( ClassType* instance, MethodType method )
{
	m_Impl = new Method<ClassType> (instance, method);
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template < typename FunctionType >
Helium::Delegate< ArgsType, RefCountBaseType > Helium::Delegate< ArgsType, RefCountBaseType >::Create( FunctionType function )
{
	return Delegate(function);
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template < class ClassType, typename MethodType >
Helium::Delegate< ArgsType, RefCountBaseType > Helium::Delegate< ArgsType, RefCountBaseType >::Create( ClassType* instance, MethodType method )
{
	return Delegate (instance, method);
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Clear()
{
	m_Impl = NULL;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
bool Helium::Delegate< ArgsType, RefCountBaseType >::Valid() const
{
	return m_Impl.ReferencesObject();
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Set( const Delegate& delegate )
{
	m_Impl = delegate.m_Impl;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template < typename FunctionType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Set( FunctionType function )
{
	m_Impl = new Function (function);
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template < class ClassType, typename MethodType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Set( ClassType* instance, MethodType method )
{
	m_Impl = new Method<ClassType> (instance, method);
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
bool Helium::Delegate< ArgsType, RefCountBaseType >::Equals( const Delegate& rhs ) const
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template <typename FunctionType>
bool Helium::Delegate< ArgsType, RefCountBaseType >::Equals( FunctionType function ) const
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType, typename MethodType >
bool Helium::Delegate< ArgsType, RefCountBaseType >::Equals( const ClassType* instance, MethodType method ) const
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Invoke( ArgsType parameter ) const
{
	if (m_Impl.ReferencesObject())
	{
		m_Impl->Invoke(parameter);
	}
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
Helium::Delegate< ArgsType, RefCountBaseType >::Function::Function( FunctionType function )
	: m_Function( function )
{
	HELIUM_ASSERT( function );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
Helium::DelegateType Helium::Delegate< ArgsType, RefCountBaseType >::Function::GetType() const
{
	return DelegateTypes::Function;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
bool Helium::Delegate< ArgsType, RefCountBaseType >::Function::Equals( const DelegateImpl* rhs ) const
{
	if ( GetType() != rhs->GetType() )
	{
		return false;
	}

	const Function* f = static_cast<const Function*>(rhs);

	return m_Function == f->m_Function;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Function::Invoke( ArgsType parameter ) const
{
	m_Function( parameter );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType >
Helium::Delegate< ArgsType, RefCountBaseType >::Method< ClassType >::Method( ClassType* instance, MethodType method )
	: m_Instance( instance )
	, m_Method( method )
{
	HELIUM_ASSERT( instance );
	HELIUM_ASSERT( method );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType >
Helium::DelegateType Helium::Delegate< ArgsType, RefCountBaseType >::Method< ClassType >::GetType() const
{
	return DelegateTypes::Method;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType >
bool Helium::Delegate< ArgsType, RefCountBaseType >::Method< ClassType >::Equals( const DelegateImpl* rhs ) const
{
	if ( GetType() != rhs->GetType() )
	{
		return false;
	}

	const Method* m = static_cast<const Method*>(rhs);

	return m_Instance == m->m_Instance && m_Method == m->m_Method;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType >
void Helium::Delegate< ArgsType, RefCountBaseType >::Method< ClassType >::Invoke( ArgsType parameter ) const
{
	(m_Instance->*m_Method)( parameter );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
uint32_t Helium::Event< ArgsType, RefCountBaseType >::Count() const
{
	return m_Impl.ReferencesObject() ? (uint32_t)m_Impl->Count() : 0;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
bool Helium::Event< ArgsType, RefCountBaseType >::Valid() const
{
	return Count() > 0;
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::Add( const Delegate& delegate )
{
	if ( !m_Impl.ReferencesObject() )
	{
		m_Impl = new EventImpl;
	}

	m_Impl->Add( delegate );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< typename FunctionType >
void Helium::Event< ArgsType, RefCountBaseType >::AddFunction( FunctionType function )
{
	if ( !m_Impl.ReferencesObject() )
	{
		m_Impl = new EventImpl;
	}

	m_Impl->AddFunction( function );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType, typename MethodType >
void Helium::Event< ArgsType, RefCountBaseType >::AddMethod( ClassType* instance, MethodType method )
{
	if ( !m_Impl.ReferencesObject() )
	{
		m_Impl = new EventImpl;
	}

	m_Impl->AddMethod( instance, method );
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::Remove( const Delegate& delegate )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< typename FunctionType >
void Helium::Event< ArgsType, RefCountBaseType >::RemoveFunction( FunctionType function )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType, typename MethodType >
void Helium::Event< ArgsType, RefCountBaseType >::RemoveMethod( const ClassType* instance, MethodType method )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::Raise( ArgsType parameter )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::RaiseWithEmitter( ArgsType parameter, const Delegate& emitter )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
Helium::Event< ArgsType, RefCountBaseType >::EventImpl::EventImpl()
	: m_EntryCount (0)
	, m_EmptySlots (0)
{

}

template< typename ArgsType, template< typename T > class RefCountBaseType >
uint32_t Helium::Event< ArgsType, RefCountBaseType >::EventImpl::Count() const
{
	return (uint32_t)m_Delegates.size();
}

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::Compact()
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::Add( const Delegate& delegate )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< typename FunctionType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::AddFunction( FunctionType function )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType, typename MethodType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::AddMethod( ClassType* instance, MethodType method )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::Remove( const Delegate& delegate )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< typename FunctionType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::RemoveFunction( FunctionType function )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
template< class ClassType, typename MethodType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::RemoveMethod( const ClassType* instance, MethodType method )
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

template< typename ArgsType, template< typename T > class RefCountBaseType >
void Helium::Event< ArgsType, RefCountBaseType >::EventImpl::Raise( ArgsType parameter, const Delegate& emitter )
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