#ifndef __PERSISTENT_H__
#define __PERSISTENT_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>

#include <map>

class Persistent
{
public:
	virtual wxString   GetClassName() const = 0;
	virtual wxXmlNode *Serialize() const = 0;
	virtual void       Deserialize(const wxXmlNode& root) = 0;

	typedef Persistent *(*Creator)(const wxXmlNode& root);

	// Registers the creator for the given class name.
	static void        RegisterClass(const wxString& classname, Creator creator);
	// Serializes the object.
	static wxXmlNode  *SerializeObject(const Persistent& obj);
	// Returns an object given its UID.
	static Persistent *GetObjectByUID(const wxString& uid);
	// De-serializes an object, first pass.
	static Persistent *DeserializeObject(const wxXmlNode& root, Persistent *obj = NULL);

private:
	static void CreateObjects(const wxXmlNode& root);
	static std::map<wxString, Creator> m_Creators;
	static std::map<wxString, Persistent *> m_Objects;
};

#endif // __PERSISTENT_H__
