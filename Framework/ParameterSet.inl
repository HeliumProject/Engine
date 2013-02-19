
namespace Helium
{
    template <class T>
    Reflect::DataPtr ParameterSet::Parameter<T>::GetDataPtr()
    {
        Reflect::DataPtr data(Reflect::AssertCast<Helium::Reflect::Data>(Reflect::GetDataClass<T>()->m_Creator()));
        data->ConnectData(&m_Parameter);
        return data;
    }
    
    template <class T>
    void ParameterSet::SetParameter(Name name, T value)
    {
        Parameter<T> *param = HELIUM_NEW(m_Heap, Parameter<T>);
        param->m_Name = name;
        param->m_Parameter = value;
        m_Parameters.Add(param);
    }
}