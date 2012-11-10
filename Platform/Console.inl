template <size_t N>
int Helium::StringPrint( char (&dest)[N], const char* fmt, ... )
{
	va_list args;
	va_start(args, fmt);
	int result = StringPrintArgs( dest, N, fmt, args );
	va_end(args);
	return result;
}

template <size_t N>
int Helium::StringPrint( wchar_t (&dest)[N], const wchar_t* fmt, ... )
{
	va_list args;
	va_start(args, fmt);
	int result = StringPrintArgs( dest, N, fmt, args );
	va_end(args);
	return result;
}

template <size_t N>
int Helium::StringPrintArgs( char (&dest)[N], const char* fmt, va_list args )
{
	return StringPrintArgs( dest, N, fmt, args );
}	

template <size_t N>
int Helium::StringPrintArgs( wchar_t (&dest)[N], const wchar_t* fmt, va_list args )
{
	return StringPrintArgs( dest, N, fmt, args );
}	
