#pragma once

#include "Graph/Debug.h"

template<class T>
class List
{
public:
	//typedef std::map<unsigned int, T>::iterator Iterator;
	class Iterator
	{
	public:
		T    operator* ()     { return m_iter->second; }
		void operator++ ()    { ++m_iter; }
		void operator++ (int) { ++m_iter; }
		bool operator! ()     { return m_iter != m_map->end(); }
	private:
		friend class List;
		Iterator(const std::map<unsigned int, T> *map): m_map(map), m_iter(map->begin()) {}
		const std::map<unsigned int, T> *m_map;
		typename std::map<unsigned int, T>::const_iterator m_iter;
	};
	List(): m_count(0) {}
	// Adds a new element to the set.
	void     Add(T t) { m_map.insert(std::pair<unsigned int, T>(m_count++, t)); m_set.insert(t); }
	// Deletes the element from the set if it exists.
	void     Delete(T t)
	{
		for (std::map<unsigned int, T>::iterator i = m_map.begin(); i != m_map.end(); ++i)
		{
			if (i->second == t)
			{
				m_map.erase(i);
				m_set.erase(t);
				return;
			}
		}
	}
	// Returns the number of elements in the set.
	size_t   Size() const { return m_map.size(); }
	// True if the set contains the element.
	bool     Contains(T t) const { return m_set.find(t) != m_set.end(); }
	// Returns an iterator for the set.
	Iterator Iterate() const { return Iterator(&m_map); }
	// Returns the last item of the set.
	T Last() const { std::map<unsigned int, T>::const_iterator i = m_map.end(); --i; return i->second; }
	// Clears the set.
	void     Clear() { m_map.clear(); m_set.clear(); }
	// Compares the set with another set.
	bool     Equals(const List<T>& other) const
	{
		if (Size() != other.Size())
		{
			return false;
		}
		List<T>::Iterator i1 = Iterate();
		List<T>::Iterator i2 = other.Iterate();
		while (!i1)
		{
			if (*i1 != *i2)
			{
				return false;
			}
			i1++, i2++;
		}
		return true;
	}
private:
	std::map<unsigned int, T> m_map;
	std::set<T> m_set;
	unsigned int m_count;
};

namespace Util
{
	wxString ToID(const wxString& name);

	wxColor  TextColorFromBG(const wxColor& bg);
};

