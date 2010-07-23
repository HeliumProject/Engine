
//
// String types
//

#include <string>
#include <fstream>
#include <sstream>
#include <strstream>

#ifdef WIN32
# include <tchar.h>
#endif

#ifdef LITESQL_UNICODE
typedef wchar_t LiteSQL_Char;
# define LiteSQL_L(s) L##s
#else
typedef char LiteSQL_Char;
#define LiteSQL_L(s) s
#endif

typedef std::basic_string<LiteSQL_Char> LiteSQL_String;

typedef std::basic_istream<LiteSQL_Char, std::char_traits<LiteSQL_Char> > LiteSQL_iStream;
typedef std::basic_ostream<LiteSQL_Char, std::char_traits<LiteSQL_Char> > LiteSQL_oStream;
typedef std::basic_iostream<LiteSQL_Char, std::char_traits<LiteSQL_Char> > LiteSQL_ioStream;

typedef std::basic_ifstream<LiteSQL_Char, std::char_traits<LiteSQL_Char> > LiteSQL_ifStream;
typedef std::basic_ofstream<LiteSQL_Char, std::char_traits<LiteSQL_Char> > LiteSQL_ofSstream;
typedef std::basic_fstream<LiteSQL_Char, std::char_traits<LiteSQL_Char> > LiteSQL_fSstream;

typedef std::basic_stringstream<LiteSQL_Char, std::char_traits<LiteSQL_Char>, std::allocator<LiteSQL_Char> > LiteSQL_Stringstream;
typedef std::basic_istringstream<LiteSQL_Char, std::char_traits<LiteSQL_Char>, std::allocator<LiteSQL_Char> > LiteSQL_iStringstream;
typedef std::basic_ostringstream<LiteSQL_Char, std::char_traits<LiteSQL_Char>, std::allocator<LiteSQL_Char> > LiteSQL_oStringstream;
