
template <class T>
Helium::Reflect::Data* Helium::ParameterSet::Parameter<T>::GetPointer()
{
	return Pointer( &m_Value, NULL, NULL );
}

template <class T>
void Helium::ParameterSet::SetParameter(Name name, T value)
{
	Parameter<T> *param = HELIUM_NEW(m_Heap, Parameter<T>);
	param->m_Name = name;
	param->m_Value = value;
	param->m_Translator = AllocateTranslator( value, value );
	m_Parameters.Add(param);
}
