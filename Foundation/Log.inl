Helium::Log::Statement::Statement( const tstring& string, Stream stream, Level level, int indent )
    : m_String( string )
    , m_Stream( stream )
    , m_Level( level )
    , m_Indent( indent )
{

}

void Helium::Log::Statement::ApplyIndent()
{
    tstring indented;
    ApplyIndent( m_String.c_str(), indented );
    m_String = indented;
}

Helium::Log::PrintingArgs::PrintingArgs( const Statement& statement )
    : m_Statement ( statement )
    , m_Skip( false )
{

}

Helium::Log::PrintedArgs::PrintedArgs( const Statement& statement )
    : m_Statement ( statement )
{

}

template <bool (*AddFunc)(const tstring& fileName, Helium::Log::Stream stream, uint32_t threadId, bool append), void (*RemoveFunc)(const tstring& fileName)>
Helium::Log::FileHandle< AddFunc, RemoveFunc >::FileHandle(const tstring& file, Stream stream, uint32_t threadId, bool append  )
    : m_File (file)
{
    if (!m_File.empty())
    {
        AddFunc(m_File, stream, threadId, append);
    }
}

template <bool (*AddFunc)(const tstring& fileName, Helium::Log::Stream stream, uint32_t threadId, bool append), void (*RemoveFunc)(const tstring& fileName)>
Helium::Log::FileHandle< AddFunc, RemoveFunc >::~FileHandle()
{
    if (!m_File.empty())
    {
        RemoveFunc(m_File);
    }
}

template <bool (*AddFunc)(const tstring& fileName, Helium::Log::Stream stream, uint32_t threadId, bool append), void (*RemoveFunc)(const tstring& fileName)>
const tstring& Helium::Log::FileHandle< AddFunc, RemoveFunc >::GetFile()
{
    return m_File;
}

Helium::Log::Indentation::Indentation()
{
    Indent();
}

Helium::Log::Indentation::~Indentation()
{
    UnIndent();
}