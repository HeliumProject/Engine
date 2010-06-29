#pragma once

class Serialized
{
public:
    Serialized()
    {
    }

    virtual ~Serialized()
    {
    }

	virtual wxString   GetClassName() const = 0;
	virtual wxXmlNode *Serialize() const = 0;
	virtual void       Deserialize(const wxXmlNode& root) = 0;

	typedef Serialized *(*Creator)(const wxXmlNode& root);

	// Registers the creator for the given class name.
	static void        RegisterSerializedClass(const wxString& classname, Creator creator);
	// Serializes the object.
	static wxXmlNode  *SerializeObject(const Serialized& obj);
	// Returns an object given its UID.
	static Serialized *GetObjectByUID(const wxString& uid);

    // De-serializes an object, first pass.
	static Serialized *DeserializeObject(const wxXmlNode& root, Serialized *obj = NULL);

private:
	static void CreateObjects(const wxXmlNode& root);
	static std::map<wxString, Creator> m_Creators;
	static std::map<wxString, Serialized *> m_Objects;
};
