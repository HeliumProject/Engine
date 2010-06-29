#include "util.h"

/*template<class T> void
Set<T>::Add(T t)
{
	if (!Contains(t))
	{
		map.insert(std::pair<T, size_t>(t, vec.size()));
		vec.push_back(t);
	}
}

template<class T> void
Set<T>::Delete(T t)
{
	std::map<T, size_t>::iterator it = map.find(t);
	if (it != map.end())
	{
		vec.erase(vec.begin() + it->second);
		map.erase(it);
	}
}

template<class T> bool
Set<T>::Contains(T t) const
{
	std::map<T, size_t>::const_iterator it = m_Map.find(t);
	return it != m_Map.end();
}*/

namespace Util
{
	wxString
	ToID(const wxString& name)
	{
		wxString id;
		for (size_t i = 0; i < name.Len(); i++)
		{
			wxChar k = name[i];
			int ok = k >= wxT('a') && k <= wxT('z');
			ok |= k >= wxT('A') && k <= wxT('Z');
			ok |= k == wxT('_');
			ok |= k >= wxT('0') && k <= wxT('9') && i != 0;
			if (ok != 0)
			{
				id.Append(k);
			}
			else
			{
				id.Append(wxT('_'));
			}
		}
		return id;
	}

	wxColor
	TextColorFromBG(const wxColor& bg)
	{
		int gray = 77 * bg.Red() + 151 * bg.Green() + 28 * bg.Blue();
		gray >>= 8;
		gray = gray < 128 ? 255 : 0;
		return wxColor(gray, gray, gray);
	}
};
