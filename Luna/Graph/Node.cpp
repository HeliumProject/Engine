#include "Precompile.h"
#include "Graph/Node.h"

#include <stdio.h>

#include "Graph/NodeDefinition.h"
#include "Graph/RulerMemoryDC.h"
#include "Graph/LuaUtilities.h"
#include "Graph/Method.h"
#include "Graph/ProjectNotebook.h"
#include "Graph/XML.h"
#include "Graph/Debug.h"

#include "Platform/String.h"

#undef DrawText

Node::Node()
: ShaderObject()
{
    //AddMember(new Member(this, wxT("id"), &m_id));
    AddMember(NEW(Member, (this, wxT("path"), &m_path)));
    AddMember(NEW(Member, (this, wxT("type"), &m_type)));
    AddMember(NEW(Member, (this, wxT("description"), &m_description)))->SetCData(true);
    AddMember(NEW(Member, (this, wxT("tooltip"), &m_tool_tip)));
    AddMember(NEW(Member, (this, wxT("deletable"), &m_deletable)))->SetValue(true);
    AddMember(NEW(Member, (this, wxT("Name"), &m_name)));
}

void
Node::EvalSize()
{
    m_bbox.SetHeight((int)((NumInputs() + NumOutputs() + 1) * GetLineHeight()));
    wxString name = GetMember(wxT("Name"))->GetString();
    RulerMemoryDC ruler;
    ruler.SetFont(GetFont());
    int width = ruler.GetTextExtent(name).GetWidth() + GetSeparatorLength() + GetLineHeight();
    for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
    {
        int w = ruler.GetTextExtent((*i)->GetMember(wxT("Name"))->GetString()).GetWidth();
        if (w > width)
        {
            width = w;
        }
    }
    width += GetSeparatorLength() * 2;
    m_bbox.SetWidth(width);
    int y = GetLineHeight();
    for (List<InputPort *>::Iterator i = m_inputs.Iterate(); !i; i++, y += GetLineHeight())
    {
        Shape *child = *i;
        child->SetPosition(m_bbox.GetX(), m_bbox.GetY() + y);
        child->SetSize(width, Node::GetLineHeight());
    }
    for (List<OutputPort *>::Iterator i = m_outputs.Iterate(); !i; i++, y += GetLineHeight())
    {
        Shape *child = *i;
        child->SetPosition(m_bbox.GetX(), m_bbox.GetY() + y);
        child->SetSize(width, Node::GetLineHeight());
    }
}

void
Node::GenerateCode(CodeTextCtrl *codectrl, Report *reportctrl) const
{
    switch (GetStatus())
    {
    case Ok:
        break;
    case InconsistentInputs:
        reportctrl->Error(wxT("Invalid type combination in input ports"), (void *)this);
    }
    for (List<InputPort *>::Iterator i = InputIterator(); !i; i++)
    {
        switch ((*i)->GetStatus())
        {
        case Port::Ok:
            break;
        case Port::Disconnected:
            reportctrl->Warning(wxT("Port disconnected"), *i);
            break;
        case Port::Required:
            reportctrl->Error(wxT("Required port disconnected"), *i);
            break;
        case Port::TypeMismatch:
            reportctrl->Error(wxT("Type mismatch in port"), *i);
        }
    }
    for (List<OutputPort *>::Iterator i = OutputIterator(); !i; i++)
    {
        switch ((*i)->GetStatus())
        {
        case Port::Ok:
            break;
        case Port::Disconnected:
            reportctrl->Warning(wxT("Port disconnected"), *i);
            break;
        case Port::Required:
            reportctrl->Error(wxT("Required port disconnected"), *i);
            break;
        case Port::TypeMismatch:
            reportctrl->Error(wxT("Type mismatch in port"), *i);
        }
    }
    codectrl->Append(wxT("/*\n")).Append(GetMember(wxT("path"))->GetString()).Append(wxT("\n"));
    for (List<InputPort *>::Iterator i = InputIterator(); !i; i++)
    {
        codectrl->Append(wxT("Input port  ")).Append((*i)->GetMember(wxT("Name"))->GetString()).Append(wxT(": ")).Append((*i)->GetType()).Append(wxT("\n"));
    }
    for (List<OutputPort *>::Iterator i = OutputIterator(); !i; i++)
    {
        codectrl->Append(wxT("Output port ")).Append((*i)->GetMember(wxT("Name"))->GetString()).Append(wxT(": ")).Append((*i)->GetType()).Append(wxT("\n"));
    }
    codectrl->Append(wxT("*/\n"));
    Method *code = GetMethod(wxT("code"), Project::GetProject()->GetTarget());
    if (code != NULL)
    {
        codectrl->Append(code->GenerateCode(this));
    }
    else
    {
        codectrl->Append(wxT("/* No code! */\n"));
    }
    codectrl->Append(wxT("\n"));
}

bool
Node::CheckInputConstraints() const
{
    Method *code = GetMethod(wxT("input-constraints"), Project::GetProject()->GetTarget());
    if (code != NULL)
    {
        return code->InputConstraints(this);
    }
    return true;
}

void           
Node::Deserialize(const wxXmlNode& root)
{
    const wxString  c_target_string = wxT("Cg/Arithmetic Operators/Two Operand/Two Operand");

    ShaderObject::Deserialize(root);
    wxString        path_string     = GetMember(wxT("path"))->GetString();

    if(path_string == c_target_string)
    {
        wxString name = GetMember(wxT("Name"))->GetString();

        if (name == wxT("Add"))
        {
            GetMember(wxT("operator_ndx"))->SetValue(0);
        }
        else if (name == wxT("Subtract"))
        {
            GetMember(wxT("operator_ndx"))->SetValue(1);
        }
        else if (name == wxT("Multiply"))
        {
            GetMember(wxT("operator_ndx"))->SetValue(2);
        }
        else if (name == wxT("Divide"))
        {
            GetMember(wxT("operator_ndx"))->SetValue(3);
        }
    }
}

Node::Status
Node::GetStatus() const
{
    return CheckInputConstraints() ? Ok : InconsistentInputs;
}

void
Node::Draw(wxDC& dc) const
{
    // Enlarge the node area to avoid flickering when hovering.
    wxRect border = GetBBox();
    border.Inflate(5, 5);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(border);
    // Draw.
    wxRect rect = GetBBox();
    const int x0 = rect.GetLeft();
    int y0 = rect.GetTop();
    const int width = rect.GetWidth();
    const int height = rect.GetHeight();
    const wxString name = GetMember(wxT("Name"))->GetString();
    const int lh = GetLineHeight();
    const int lh2 = lh / 2;
    const int sl = GetSeparatorLength();
    const int yy0 = y0;
    // Draw rectangle.
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(wxBrush(GetMember(wxT("Fill_color"))->GetColor()));
    dc.DrawRectangle(x0, y0, width, height);
    // Draw title.
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(x0, y0, width, lh);
    dc.SetFont(GetFont());
    wxSize size = dc.GetTextExtent(name);
    dc.SetTextForeground(wxColor(255, 255, 255));
    int x = x0 + (width - lh - size.GetWidth()) / 2;
    int y = y0 + (lh - size.GetHeight()) / 2;
    dc.DrawText(name, x, y);
    y0 += lh;
    dc.SetTextForeground(wxColor(0, 0, 0));
    // Draw ports, taking note of the worst status.
    Port::Status pstatus = Port::Ok;
    List<InputPort *>::Iterator i = m_inputs.Iterate();
    if (!i)
    {
        (*i)->Draw(dc);
        pstatus = (*i)->GetStatus();
        for (i++; !i; i++)
        {
            (*i)->Draw(dc);
            Port::Status st = (*i)->GetStatus();
            if (st > pstatus)
            {
                pstatus = st;
            }
        }
    }
    for (List<OutputPort *>::Iterator i = m_outputs.Iterate(); !i; i++)
    {
        (*i)->Draw(dc);
        Port::Status st = (*i)->GetStatus();
        if (st > pstatus)
        {
            pstatus = st;
        }
    }
    // Draw led.
    wxColor color;
    switch (pstatus)
    {
    case Port::Ok:
        color.Set(0, 255, 0);
        break;
    case Port::Disconnected:
        color.Set(255, 255, 0);
        break;
    case Port::Required:
    case Port::TypeMismatch:
        color.Set(255, 0, 0);
        break;
    }
    if (GetStatus() == Node::InconsistentInputs)
    {
        color.Set(255, 0, 0);
    }
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(wxBrush(color));
    dc.DrawCircle(x0 + width - lh2, yy0 + lh2, lh2 - 3);
}

void
Node::DeserializeChildren(const wxXmlNode& root)
{
    wxXmlNode *child = root.GetChildren();
    while (child != NULL)
    {
        wxString uid = XML::GetStringAttribute(*child, wxT("uid"));
        Serialized *obj = Serialized::GetObjectByUID(uid);
        obj->Deserialize(*child);
        if (obj->GetClassName() == wxT("input"))
        {
            AddInput(static_cast<InputPort *>(obj));
        }
        else
        {
            AddOutput(static_cast<OutputPort *>(obj));
        }
        child = child->GetNext();
    }
}

int
Node::LuaIndex(lua_State *L)
{
    ShaderObject *obj = *(ShaderObject **)lua_touserdata(L, 1);
    Node *node = dynamic_cast<Node *>(obj);
    size_t len;
    const char *key = luaL_checklstring(L, 2, &len);

    tstring temp;
    Platform::ConvertString( key, temp );
    Member *member = node->FindMember(temp);
    if (member != NULL)
    {
        *(const Member **)lua_newuserdata(L, sizeof(Member *)) = member;
        lua_newtable(L);
        lua_pushcfunction(L, MemberLuaIndex);
        lua_setfield(L, -2, "__index");
        lua_setmetatable(L, -2);
        return 1;
    }
    for (List<Shape *>::Iterator i = node->ChildIterator(); !i; i++)
    {
        ShaderObject *obj = dynamic_cast<ShaderObject *>(*i);
        if (obj != NULL && obj->GetLuaId() == temp)
        {
            obj->PushLua(L);
            return 1;
        }
    }
    return 0;
}

int
Node::MemberLuaIndex(lua_State *L)
{
    size_t len;
    const char *key = luaL_checklstring(L, 2, &len);
    if (len == 9)
    {
        if (!strcmp(key, "get_value"))
        {
            lua_pushvalue(L, 1);
            lua_pushcclosure(L, MemberLuaGetValue, 1);
            return 1;
        }
        else if (!strcmp(key, "set_value"))
        {
            lua_pushvalue(L, 1);
            lua_pushcclosure(L, MemberLuaSetValue, 1);
            return 1;
        }
    }
    return 0;
}

int
Node::MemberLuaGetValue(lua_State *L)
{
    Member *member = *(Member **)lua_touserdata(L, lua_upvalueindex(1));
    wxString value = member->GetValue();

    std::string temp;
    Platform::ConvertString( (const wxChar*)value.c_str(), temp );
    lua_pushlstring(L, temp.c_str(), temp.length());
    return 1;
}

int
Node::MemberLuaSetValue(lua_State *L)
{
    Member *member = *(Member **)lua_touserdata(L, lua_upvalueindex(1));
    switch (lua_type(L, 1))
    {
    case LUA_TNIL:
        break;
    case LUA_TNUMBER:
        if (member->GetType() == Member::Int)
        {
            member->SetValue((int)lua_tonumber(L, 1));
        }
        else
        {
            member->SetValue(lua_tonumber(L, 1));
        }
        break;
    case LUA_TBOOLEAN:
        member->SetValue(lua_toboolean(L, 1) ? true : false);
        break;
    case LUA_TSTRING:
        {
            tstring temp;
            Platform::ConvertString( lua_tostring(L, 1), temp );

            member->SetValue(wxString(temp));
        }
        break;
    case LUA_TTABLE:
    case LUA_TFUNCTION:
    case LUA_TUSERDATA:
    case LUA_TTHREAD:
    case LUA_TLIGHTUSERDATA:
        break;
    }
    return 0;
}

int
Node::GetLineHeight()
{
    return 16;
}

int
Node::GetSeparatorLength()
{
    return 5;
}

const wxFont
Node::GetFont()
{
    static wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    return font;
}
