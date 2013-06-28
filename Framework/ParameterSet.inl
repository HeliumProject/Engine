template <class T>
T &Helium::ParameterSet::SetParameter(Name name, const T& value)
{
	Parameter<T> *param = /*new(m_Heap.Allocate(sizeof(Parameter<T>))) Parameter<T>();*/ HELIUM_NEW(m_ParameterAllocator, Parameter<T>);
	param->m_Name = name;
	param->m_Value = value;
	param->m_Translator = AllocateTranslator( value, value );
	m_Parameters.Add(param);
	return param->m_Value;
}
