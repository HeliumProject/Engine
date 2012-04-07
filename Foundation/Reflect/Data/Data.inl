template<class T>
Helium::Reflect::DataPointer<T>::DataPointer()
    : m_Target( NULL )
    , m_Owned( false )
{

}

template<class T>
Helium::Reflect::DataPointer<T>::~DataPointer()
{
    Deallocate();
}

template<class T>
void Helium::Reflect::DataPointer<T>::Allocate() const
{
    Deallocate();
    m_Target = new T;
    m_Owned = true;
}

template<class T>
void Helium::Reflect::DataPointer<T>::Deallocate() const
{
    if ( m_Owned && m_Target )
    {
        delete m_Target;
        m_Target = NULL;
    }
}

template<class T>
void Helium::Reflect::DataPointer<T>::Connect(void* pointer)
{
    Deallocate();
    m_Target = reinterpret_cast< T* >( pointer );
    m_Owned = false;
}

template<class T>
void Helium::Reflect::DataPointer<T>::Disconnect()
{
    Deallocate();
}

template<class T>
const T* Helium::Reflect::DataPointer<T>::operator->() const
{
    if ( !m_Target )
    {
        Allocate();
    }

    return m_Target;
}

template<class T>
T* Helium::Reflect::DataPointer<T>::operator->()
{
    if ( !m_Target )
    {
        Allocate();
    }

    return m_Target;
}

template<class T>
Helium::Reflect::DataPointer<T>::operator const T*() const
{
    if ( !m_Target )
    {
        Allocate();
    }

    return m_Target;
}

template<class T>
Helium::Reflect::DataPointer<T>::operator T*()
{
    if ( !m_Target )
    {
        Allocate();
    }

    return m_Target;
}

Helium::Reflect::VoidDataPointer::VoidDataPointer()
    : m_Target( NULL )
    , m_Owned( false )
#ifdef REFLECT_CHECK_MEMORY
    , m_Size( 0 )
#endif
{

}

Helium::Reflect::VoidDataPointer::~VoidDataPointer()
{
    Deallocate();
}

void Helium::Reflect::VoidDataPointer::Allocate( uint32_t size ) const
{
    REFLECT_CHECK_MEMORY_ASSERT( m_Size == 0 || m_Size == size );
    m_Target = new uint8_t[ size ];
    m_Owned = true;
#ifdef REFLECT_CHECK_MEMORY
    m_Size = size;
#endif

    Helium::MemoryZero(m_Target, size);
}

void Helium::Reflect::VoidDataPointer::Deallocate() const
{
    if ( m_Owned && m_Target )
    {
        delete[] m_Target;
        m_Target = NULL;
    }
}

void Helium::Reflect::VoidDataPointer::Connect(void* pointer, uint32_t size)
{
    Deallocate();
    m_Target = pointer;
    m_Owned = false;
#ifdef REFLECT_CHECK_MEMORY
    m_Size = size;
#endif
}

void Helium::Reflect::VoidDataPointer::Disconnect()
{
    Deallocate();
}

const void* Helium::Reflect::VoidDataPointer::Get(uint32_t size) const
{
    if ( !m_Target )
    {
        Allocate(size);
    }

    REFLECT_CHECK_MEMORY_ASSERT( m_Size == size );
    return m_Target;
}

void* Helium::Reflect::VoidDataPointer::Get(uint32_t size)
{
    if ( !m_Target )
    {
        Allocate(size);
    }

    REFLECT_CHECK_MEMORY_ASSERT( m_Size == size );
    return m_Target;
}

template<class T>
T* Helium::Reflect::Data::GetData(Data*)
{
    return NULL;
}

template <class T>
Helium::Reflect::DataPtr Helium::Reflect::Data::Create()
{
    const Class* dataClass = Reflect::GetDataClass<T>();
    HELIUM_ASSERT( dataClass );

    return AssertCast<Data>( Registry::GetInstance()->CreateInstance( dataClass ) );
}

template <class T>
Helium::Reflect::DataPtr Helium::Reflect::Data::Create(const T& value)
{
    DataPtr ser = Create<T>();

    if (ser.ReferencesObject())
    {
        SetValue(ser, value);
    }

    return ser;
}

// See also special implementation for Data::BindStructure in .cpp
template <class T>
Helium::Reflect::DataPtr Helium::Reflect::Data::Bind(T& value, void* instance, const Field* field)
{
    DataPtr ser = Create<T>();

    if (ser.ReferencesObject())
    {
        // pmd - ConnectData might require the instance/field, so moving that above the ConnectData call. (Structures
        // definitely require this, but FYI should use BindStructure).
        ser->m_Instance = instance;
        ser->m_Field = field;
        ser->ConnectData( &value );
    }

    return ser;
}

template <class T>
bool Helium::Reflect::Data::GetValue(Data* ser, T& value)
{
    if ( ser == NULL )
    {
        return false;
    }

    bool result = false;
    const Class* dataClass = Reflect::GetDataClass<T>();

    // if you die here, then you are not using data objects that
    //  fully implement the type deduction functions above
    HELIUM_ASSERT( dataClass != NULL );

    // sanity check our object type
    if ( ser->IsClass( dataClass ) )
    {
        // get internal data pointer
        const T* data = GetData<T>( ser );
        HELIUM_ASSERT( data != NULL );

        // make the copy
        value = *data;
        result = true;
    }
    else
    {
        // create a temporary data of the value type
        DataPtr temp = AssertCast<Data>( Registry::GetInstance()->CreateInstance( dataClass ) );

        // connect the temp data to the temp value
        T tempValue; temp->ConnectData( &tempValue );

        // cast into the temp value
        if (Data::CastValue( ser, temp ))
        {
            // make the copy
            value = tempValue;
            result = true;
        }
    }

    HELIUM_ASSERT(result);
    return result;
}

template <class T>
bool Helium::Reflect::Data::SetValue(Data* ser, T value, bool raiseEvents)
{
    if ( ser == NULL )
    {
        return false;
    }

    bool result = false;
    const Class* dataClass = Reflect::GetDataClass< T >();

    // if you die here, then you are not using data objects that
    //  fully implement the type deduction functions above
    HELIUM_ASSERT( dataClass != NULL );

    // sanity check our object type
    if ( ser->IsClass( dataClass ) )
    {
        // get internal data pointer
        T* data = GetData<T>( ser );
        HELIUM_ASSERT( data != NULL );

        // if you die here, then you are probably in release and this should crash
        (*data) = value;
        result = true;
    }
    else
    {
        // create a temporary data of the value type
        DataPtr temp = AssertCast<Data>( Registry::GetInstance()->CreateInstance( dataClass ) );

        // connect the temp data to the temp value
        temp->ConnectData( &value );

        // cast into the data
        if (Data::CastValue( ser, temp ))
        {
            result = true;
        }
    }

    if (result)
    {
        // Notify interested listeners that the data has changed.
        if ( raiseEvents && ser && ser->m_Instance && ser->m_Field && ser->m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
        {
            Object* object = static_cast< Object* >( ser->m_Instance );
            object->RaiseChanged( ser->m_Field );
        }
    }

    HELIUM_ASSERT(result);
    return result;
}

Helium::Reflect::Data& Helium::Reflect::Data::operator=(Data* rhs)
{
    Set(rhs);
    return *this;
}

Helium::Reflect::Data& Helium::Reflect::Data::operator=(Data& rhs)
{
    Set(&rhs);
    return *this;
}

bool Helium::Reflect::Data::operator==(Data* rhs)
{
    return Equals(rhs);
}

bool Helium::Reflect::Data::operator== (Data& rhs)
{
    return Equals(&rhs);
}

bool Helium::Reflect::Data::operator!=(Data* rhs)
{
    return !Equals(rhs);
}

bool Helium::Reflect::Data::operator!=(Data& rhs)
{
    return !Equals(&rhs);
}

tostream& Helium::Reflect::operator<<(tostream& stream, const Data& s)
{
    return s >> stream;
}

tistream& Helium::Reflect::operator>>(tistream& stream, Data& s)
{
    return s << stream;
}

template<class T>
static const Helium::Reflect::Class* Helium::Reflect::GetDataClass()
{
    return NULL;
}