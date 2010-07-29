#pragma once

#include "Graph/Node.h"

class NodeDefinition
{
	public:
		NodeDefinition(const wxString& path, const wxXmlNode *root);
		virtual ~NodeDefinition() {}

		wxString GetPath() const        { return m_Path; }
		wxString GetType() const        { return m_NodeType; }
		wxString GetDescription() const { return m_Description->GetNodeContent(); }
		wxString GetTooltip() const     { return m_Tooltip->GetNodeContent(); }

		Node *NewInstance();

	private:
		void ParseNode(Node *node);
		void ParseDescription(Node *node, const wxXmlNode& root);
		void ParseToolTip(Node *node, const wxXmlNode& root);
		void ParsePorts(Node *node, const wxXmlNode& root);
		void ParseInput(Node *node, const wxXmlNode& root);
		void ParseCheckType(InputPort *input, const wxXmlNode& root);
		void ParseOutput(Node *node, const wxXmlNode& root);
		void ParseGetType(OutputPort *output, const wxXmlNode& root);
		void ParseInputConstraints(Node *node, const wxXmlNode& root);
		void ParseProperties(Node *node, const wxXmlNode& root);
		void ParseDefaultProperties(Node *node, const wxXmlNode& root);
		void ParseProperty(Node *node, const wxXmlNode& root);
		void ParseCodeGeneration(Node *node, const wxXmlNode& root);
		void ParseCode(Node *node, const wxXmlNode& root);
		Method *ParseMethod(const wxXmlNode& root, const wxString& dftl);

		const wxXmlNode * m_Root;
		wxString          m_Path;
		wxString          m_Type;
		const wxXmlNode * m_Description;
		const wxXmlNode * m_Tooltip;
		wxString          m_NodeType;
};

namespace NodeLibrary
{
	wxArrayString Add(const wxString& file);
	wxArrayString AddUserNodes();
	Node *Create(wxString type);
	wxString GetDescription(const wxString& type);
	bool SaveGroup(wxXmlNode *root);
	void Clear();
};

