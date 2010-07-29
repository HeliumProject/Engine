#pragma once

#include <wx/stream.h>
#include <wx/variant.h>

class LuaInputStream
{
	public:
		LuaInputStream(wxInputStream *is);

		wxVariant Read();
		void ReadNil();
		bool ReadBoolean();
		long ReadInteger();
		double ReadNumber();
		wxString ReadString();
		
	private:
		wxInputStream *m_is;
};

class LuaOutputStream
{
	public:
		LuaOutputStream(wxOutputStream *os);

		void Write();
		void Write(bool b);
		void Write(int i);
		void Write(double d);
		void Write(const wxString& str);

	private:
		wxOutputStream *m_os;
};

