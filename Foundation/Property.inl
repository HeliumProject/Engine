Helium::PropertyException::PropertyException( const tchar_t *msgFormat, ... )
{
    va_list msgArgs;
    va_start( msgArgs, msgFormat );
    SetMessage( msgFormat, msgArgs );
    va_end( msgArgs );
}

Helium::PropertyException::PropertyException()
{
}

template <class V>
Helium::StaticProperty< V >::StaticProperty(GetterParam g, SetterReference s)
{
    m_GetParam = g;
    m_GetterType = GetterTypes::Parameter;
    m_SetReference = s;
    m_SetterType = SetterTypes::Reference;
}

template <class V>
Helium::StaticProperty< V >::StaticProperty(GetterReference g, SetterReference s)
{
    m_GetReference = g;
    m_GetterType = GetterTypes::Reference;
    m_SetReference = s;
    m_SetterType = SetterTypes::Reference;
}

template <class V>
Helium::StaticProperty< V >::StaticProperty(GetterValue g, SetterReference s)
{
    m_GetValue = g;
    m_GetterType = GetterTypes::Value;
    m_SetReference = s;
    m_SetterType = SetterTypes::Reference;
}

template <class V>
Helium::StaticProperty< V >::StaticProperty(GetterParam g, SetterValue s)
{
    m_GetParam = g;
    m_GetterType = GetterTypes::Parameter;
    m_SetValue = s;
    m_SetterType = SetterTypes::Value;
}

template <class V>
Helium::StaticProperty< V >::StaticProperty(GetterReference g, SetterValue s)
{
    m_GetReference = g;
    m_GetterType = GetterTypes::Reference;
    m_SetValue = s;
    m_SetterType = SetterTypes::Value;
}

template <class V>
Helium::StaticProperty< V >::StaticProperty(GetterValue g, SetterValue s)
{
    m_GetValue = g;
    m_GetterType = GetterTypes::Value;
    m_SetValue = s;
    m_SetterType = SetterTypes::Value;
}

template <class V>
V Helium::StaticProperty< V >::Get() const
{
    switch(m_GetterType)
    {
    case GetterTypes::Parameter:
        {
            V value;
            (*m_GetParam)(value);
            return value;
        }

    case GetterTypes::Reference:
        {
            return (*m_GetReference)();
        }

    case GetterTypes::Value:
        {
            return (*m_GetValue)();
        }

    default:
        {
            throw PropertyException( TXT( "StaticProperty has no compatible get function" ) );
        }
    }
}

template <class V>
bool Helium::StaticProperty< V >::Set(const V& value)
{
    switch (m_SetterType)
    {
    case SetterTypes::Reference:
        {
            (*m_SetReference)(value);
            return true;
        }

    case SetterTypes::Value:
        {
            (*m_SetValue)(value);
            return true;
        }

    default:
        {
            throw PropertyException( TXT( "StaticProperty has no compatible get function" ) );
        }
    }
}

template <class T, class V>
Helium::MemberProperty< T, V >::MemberProperty(T* o, GetterParam g, SetterReference s)
{
    m_Target = o;
    m_GetParam = g;
    m_GetterType = GetterTypes::Parameter;
    m_SetReference = s;
    m_SetterType = SetterTypes::Reference;
}

template <class T, class V>
Helium::MemberProperty< T, V >::MemberProperty(T* o, GetterReference g, SetterReference s)
{
    m_Target = o;
    m_GetReference = g;
    m_GetterType = GetterTypes::Reference;
    m_SetReference = s;
    m_SetterType = SetterTypes::Reference;
}

template <class T, class V>
Helium::MemberProperty< T, V >::MemberProperty(T* o, GetterValue g, SetterReference s)
{
    m_Target = o;
    m_GetValue = g;
    m_GetterType = GetterTypes::Value;
    m_SetReference = s;
    m_SetterType = SetterTypes::Reference;
}

template <class T, class V>
Helium::MemberProperty< T, V >::MemberProperty(T* o, GetterParam g, SetterValue s)
{
    m_Target = o;
    m_GetParam = g;
    m_GetterType = GetterTypes::Parameter;
    m_SetValue = s;
    m_SetterType = SetterTypes::Value;
}

template <class T, class V>
Helium::MemberProperty< T, V >::MemberProperty(T* o, GetterReference g, SetterValue s)
{
    m_Target = o;
    m_GetReference = g;
    m_GetterType = GetterTypes::Reference;
    m_SetValue = s;
    m_SetterType = SetterTypes::Value;
}

template <class T, class V>
Helium::MemberProperty< T, V >::MemberProperty(T* o, GetterValue g, SetterValue s)
{
    m_Target = o;
    m_GetValue = g;
    m_GetterType = GetterTypes::Value;
    m_SetValue = s;
    m_SetterType = SetterTypes::Value;
}

template <class T, class V>
T* Helium::MemberProperty< T, V >::Target()
{
    return m_Target;
}

template <class T, class V>
V Helium::MemberProperty< T, V >::Get() const
{
    if (m_Target == NULL)
    {
        throw PropertyException( TXT( "MemberProperty has no target" ) );
    }

    switch(m_GetterType)
    {
    case GetterTypes::Parameter:
        {
            V value;
            ((*m_Target).*m_GetParam)(value);
            return value;
        }

    case GetterTypes::Reference:
        {
            return ((*m_Target).*m_GetReference)();
        }

    case GetterTypes::Value:
        {
            return ((*m_Target).*m_GetValue)();
        }

    default:
        {
            throw PropertyException( TXT( "MemberProperty has no compatible get function" ) );
        }
    }
}

template <class T, class V>
bool Helium::MemberProperty< T, V >::Set(const V& value)
{
    if (m_Target == NULL)
    {
        throw PropertyException( TXT( "MemberProperty has no target" ) );
    }

    switch (m_SetterType)
    {
    case SetterTypes::Reference:
        {
            ((*m_Target).*m_SetReference)(value);
            return true;
        }

    case SetterTypes::Value:
        {
            ((*m_Target).*m_SetValue)(value);
            return true;
        }

    default:
        {
            throw PropertyException( TXT( "MemberProperty has no compatible set function" ) );
        }
    }
}