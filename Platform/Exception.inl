tstring& Helium::Exception::Get()
{
    return m_Message;
}

const tstring& Helium::Exception::Get() const
{
    return m_Message;
}

void Helium::Exception::Set(const tstring& message)
{
    m_Message = message;
}
