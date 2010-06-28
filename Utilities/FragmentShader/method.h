#ifndef __METHOD_H__
#define __METHOD_H__

#include <wx/wx.h>
#include <wx/regex.h>

#include <set>
#include <vector>

#include "node.h"
#include "property.h"
#include "shaderobj.h"
#include "persistent.h"

class Method: public Persistent
{
public:
	Method() {}
	virtual ~Method() {}

	virtual bool CheckType(const InputPort *input) const
	{ THROW(TXT("Method \"CheckType\" cannot be used to check a type.")); }

	virtual wxString GetType(const OutputPort *output) const
	{ THROW(TXT("Method \"GetType\" cannot be used to get a type.")); }

	virtual bool InputConstraints(const Node *node) const
	{ THROW(TXT("Method \"InputConstraints\" cannot be used to check input constraints.")); }

	virtual bool Validate(const Property *prop) const
	{ THROW(TXT("Method \"Validate\" cannot be used to validate.")); }

	virtual void OnChanged(const Property *prop) const
	{ THROW(TXT("Method \"OnChanged\" cannot be used for OnChanged events.")); }

	virtual wxString GenerateCode(const Node *node) const
	{ THROW(TXT("Method \"GenerateCode\" cannot be used to generate code.")); }

	// Serialization.
	virtual wxString   GetClassName() const { return wxT("method"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);
};

class MethodAny: public Method
{
public:
	MethodAny(): Method() {}

	virtual bool     CheckType(const InputPort *input) const  { return true; }
	virtual wxString GetType(const OutputPort *output) const;
	virtual bool     InputConstraints(const Node *node) const { return true; }
	virtual bool     Validate(const Property *prop) const     { return true; }

	virtual wxString GetClassName() const { return wxT("any"); }
};

class MethodList: public Method
{
public:
	MethodList(std::set<wxString> *choices = NULL);

	virtual bool CheckType(const InputPort *input) const { return m_choices.count(input->GetType()) == 1; }
	virtual bool Validate(const Property *prop) const    { return m_choices.count(prop->ToString()) == 1; }

	virtual wxString   GetClassName() const { return wxT("list"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

private:
	std::set<wxString> m_choices;
};

class MethodRegex: public Method
{
public:
	MethodRegex(std::vector<wxString> *patterns = NULL);
	virtual ~MethodRegex();

	virtual bool CheckType(const InputPort *input) const { return Matches(input->GetType()); }
	virtual bool Validate(const Property *prop) const    { return Matches(prop->ToString()); }

	virtual wxString   GetClassName() const { return wxT("regex"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

private:
	void Compile();
	bool Matches(const wxString& what) const;
	std::vector<wxString> m_patterns;
	std::vector<wxRegEx *> m_regex;
};

class MethodFixed: public Method
{
public:
	MethodFixed(const wxString& what = wxEmptyString): Method(), m_what(what) {}

	virtual bool     CheckType(const InputPort *input) const { return input->GetType() == m_what; }
	virtual wxString GetType(const OutputPort *output) const { return m_what; }
	virtual bool     InputConstraints(const Node *node) const;
	virtual bool     Validate(const Property *prop) const    { return prop->ToString() == m_what; }
	virtual wxString GenerateCode(const Node *node) const    { return m_what; }

	virtual wxString   GetClassName() const { return wxT("fixed"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

private:
	wxString m_what;
};

class MethodEqual: public Method
{
public:
	MethodEqual(): Method() {}

	virtual bool InputConstraints(const Node *node) const;

	virtual wxString GetClassName() const { return wxT("equal"); }
};

class MethodSameAs: public Method
{
public:
	MethodSameAs(const wxString& id = wxEmptyString): Method(), m_id(id) {}

	wxString GetType(const OutputPort *output) const;

	virtual wxString   GetClassName() const { return wxT("same-as"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

private:
	wxString m_id;
};

class MethodLua: public Method
{
public:
	MethodLua(): Method(), m_lua_ref(LUA_NOREF) {}
	MethodLua(const wxString& source);
	virtual ~MethodLua();

	virtual bool     CheckType(const InputPort *input) const;
	virtual wxString GetType(const OutputPort *output) const;
	virtual bool     InputConstraints(const Node *node) const;
	virtual bool     Validate(const Property *prop) const;
	virtual void     OnChanged(const Property *prop) const;
	virtual wxString GenerateCode(const Node *node) const;

	virtual wxString   GetClassName() const { return wxT("lua"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

protected:
	void Call(const ShaderObject *obj) const;
	wxString m_source;
	int m_lua_ref;
};

class MethodVarParse: public MethodLua
{
public:
	MethodVarParse(): MethodLua() {}
	MethodVarParse(const wxString& pattern): MethodLua(Compile(pattern)), m_pattern(pattern) {}

	virtual bool CheckType(const InputPort *input) const  { return Method::CheckType(input); }
	virtual bool InputConstraints(const Node *node) const { return Method::InputConstraints(node); }
	virtual bool Validate(const Property *prop) const     { return Method::Validate(prop); }

	virtual wxString GetClassName() const { return wxT("var-parse"); }
	virtual wxXmlNode *Serialize() const;
	virtual void Deserialize(const wxXmlNode& root);

private:
	virtual wxString Compile(const wxString& pattern);
	wxString m_pattern;
};

Method *
DeserializeMethod(const wxXmlNode& root);

#endif // __METHOD_H__
