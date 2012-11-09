template <size_t N>
int Helium::StringPrint( tchar_t (&dest)[N], const tchar_t* fmt, ... )
{
	va_list args;
	va_start(args, fmt);
	int result = StringPrintArgs( dest, N, fmt, args );
	va_end(args);
	return result;
}

template <size_t N>
int Helium::StringPrintArgs( tchar_t (&dest)[N], const tchar_t* fmt, va_list args )
{
	return StringPrintArgs( dest, N, fmt, args );
}	
