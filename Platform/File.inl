const tstring& Helium::Directory::GetPath()
{
	return m_Path;
}

void Helium::Directory::SetPath( const tstring& path )
{
	Close();
	m_Path = path;
}
