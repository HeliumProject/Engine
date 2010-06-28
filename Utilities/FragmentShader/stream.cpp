#include "stream.h"

#include <math.h>

#include "debug.h"

// 2.0
#define TYPE_NIL		0
#define TYPE_BOOLEAN	1
#define TYPE_NUMBER		2
#define TYPE_STRING		3
#define TYPE_BYTE		4
#define TYPE_INT		5
// 2.1
#define TYPE_WORD		6
#define TYPE_DWORD		7
#define TYPE_TRUE		8
#define TYPE_FALSE		9
#define TYPE_STRING2	10
#define TYPE_STR_0		128
#define TYPE_STR_127	(TYPE_STR_0 + 127)

LuaInputStream::LuaInputStream(wxInputStream *is)
	: m_is(is)
{
}

wxVariant
LuaInputStream::Read()
{
	wxVariant res;
	int type = (unsigned char)m_is->GetC();
	switch (type)
	{
		case TYPE_BOOLEAN:
			res = (m_is->GetC() == 0) ? false : true;
			break;
		case TYPE_TRUE:
			res = true;
			break;
		case TYPE_FALSE:
			res = false;
			break;
		case TYPE_NUMBER:
		{
			double d;
			m_is->Read((void *)&d, sizeof(d));
			res = d;
			break;
		}
		case TYPE_BYTE:
		{
			tchar b;
			m_is->Read((void *)&b, sizeof(b));
			res = (long)b;
			break;
		}
		case TYPE_WORD:
		{
			short w;
			m_is->Read((void *)&w, sizeof(w));
			res = (long)w;
			break;
		}
		case TYPE_INT:
		case TYPE_DWORD:
		{
			int i;
			m_is->Read((void *)&i, sizeof(i));
			res = (long)i;
			break;
		}
		case TYPE_STRING:
		{
			int len;
			m_is->Read((void *)&len, sizeof(len));
			tchar *str = NEWARRAY(tchar, len + 1);
			m_is->Read((void *)str, len);
			str[len] = 0;
			res = str;
			DESTROYARRAY(str);
			break;
		}
		case TYPE_STRING2:
		{
			int len = (int)ReadNumber();
			tchar *str = NEWARRAY(tchar, len + 1);
			m_is->Read((void *)str, len);
			str[len] = 0;
			res = str;
			DESTROYARRAY(str);
			break;
		}
		default:
			if (type >= TYPE_STR_0 && type <= TYPE_STR_127)
			{
				int len = type - TYPE_STR_0;
				tchar *str = NEWARRAY(tchar, len + 1);
				m_is->Read((void *)str, len);
				str[len] = 0;
				res = str;
				DESTROYARRAY(str);
				break;
			}
			break;
	}
	return res;
}

void
LuaInputStream::ReadNil()
{
	Read();
}

bool
LuaInputStream::ReadBoolean()
{
	wxVariant v = Read();
	return v.GetBool();
}

long
LuaInputStream::ReadInteger()
{
	wxVariant v = Read();
	return v.GetLong();
}

double
LuaInputStream::ReadNumber()
{
	wxVariant v = Read();
	return v.GetDouble();
}

wxString
LuaInputStream::ReadString()
{
	wxVariant v = Read();
	return v.GetString();
}

LuaOutputStream::LuaOutputStream(wxOutputStream *os)
	: m_os(os)
{
}

void
LuaOutputStream::Write()
{
	m_os->PutC(TYPE_NIL);
}

void
LuaOutputStream::Write(bool b)
{
	m_os->PutC(b ? TYPE_TRUE : TYPE_FALSE);
}

void
LuaOutputStream::Write(int i)
{
	if (i >= -128 && i <= 127)
	{
		m_os->PutC(TYPE_BYTE);
		tchar b = i;
		m_os->Write((void *)&b, sizeof(b));
	}
	else if (i >= -32768 && i <= 32767)
	{
		m_os->PutC(TYPE_WORD);
		short w = i;
		m_os->Write((void *)&w, sizeof(w));
	}
	else
	{
		m_os->PutC(TYPE_DWORD);
		m_os->Write((void *)&i, sizeof(i));
	}
}

void
LuaOutputStream::Write(double d)
{
	double f = floor(d);
	if (d == f && f >= -2147483648.0f && f <= 2147483647.0f)
	{
		Write((int)f);
	}
	else
	{
		m_os->PutC(TYPE_NUMBER);
		m_os->Write((void *)&d, sizeof(d));
	}
}

void
LuaOutputStream::Write(const wxString& str)
{
	size_t len = str.Len();
	if (len >= 0 && len <= 127)
	{
		m_os->PutC((tchar)(TYPE_STR_0 + len));
	}
	else
	{
		m_os->PutC(TYPE_STRING2);
		Write((int)len);
	}
	m_os->Write((void *)str.c_str(), len);
}
