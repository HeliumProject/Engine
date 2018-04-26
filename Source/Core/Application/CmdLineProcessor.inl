template <class T>
Helium::CommandLine::SimpleOption<T>::SimpleOption( T* data, const char* token, const char* usage, const char* help )
	: Option( token, usage, help )
	, m_Data( data )
{
}

template <class T>
bool Helium::CommandLine::SimpleOption<T>::Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
	if ( argsBegin != argsEnd )
	{
		const std::string& arg = (*argsBegin);
		++argsBegin;

		std::stringstream str ( arg );
		str >> *m_Data;

		return str.fail();
	}
				
	error = std::string( "Missing parameter for option: " ) + m_Token;
	return false;
}

void Helium::CommandLine::HelpCommand::SetOwner( Processor* owner )
{
	m_Owner = owner;
}
