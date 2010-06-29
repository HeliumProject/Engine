#include "Precompile.h"
#include "Graph/Port.h"

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

Port::Port()
: ShaderObject()
, m_is_required(false)
{
    AddMember(NEW(Member, (this, wxT("id"), &m_id)));
    AddMember(NEW(Member, (this, wxT("required"), &m_is_required)));
    AddProperty(NEW(TextProperty, (this, wxT("Name"))));
}

void
Port::Draw(wxDC& dc) const
{
    Shape::Draw(dc);
    wxColor fill = m_parent->GetMember(wxT("Fill_color"))->GetColor();
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(wxBrush(fill));
    dc.DrawRectangle(m_bbox);
    dc.SetFont(Node::GetFont());
    dc.SetTextForeground(Util::TextColorFromBG(fill));
    wxString name = GetMember(wxT("Name"))->GetString();
    wxRect text(dc.GetTextExtent(name));
    text = text.CentreIn(m_bbox);
    dc.DrawText(name, text.GetX(), text.GetY());
    wxColor color;
    switch (GetStatus())
    {
    case Ok:
        color.Set(0, 255, 0);
        break;
    case Disconnected:
        color.Set(255, 255, 0);
        break;
    case Required:
    case TypeMismatch:
        color.Set(255, 0, 0);
        break;
    }
    wxPoint pos = GetConnPoint();
    wxPoint points[3] = { wxPoint(pos.x - 3, pos.y - 4), wxPoint(pos.x + 4, pos.y), wxPoint(pos.x - 3, pos.y + 4) };
    dc.SetBrush(wxBrush(color));
    dc.DrawPolygon(3, points);
}

int
Port::LuaIndex(lua_State *L)
{
    ShaderObject *obj = *(ShaderObject **)lua_touserdata(L, 1);
    Port *port = dynamic_cast<Port *>(obj);
    size_t len;
    const char *key = luaL_checklstring(L, 2, &len);
    if (len == 8 && !strcmp(key, "get_type"))
    {
        lua_pushvalue(L, 1);
        lua_pushcclosure(L, LuaGetType, 1);
        return 1;
    }
    return ShaderObject::LuaIndex(L);
}

int
Port::LuaGetType(lua_State *L)
{
    ShaderObject *obj = *(ShaderObject **)lua_touserdata(L, lua_upvalueindex(1));
    Port *port = dynamic_cast<Port *>(obj);
    wxString type = port->GetType();

    std::string temp;
    Platform::ConvertString( type.c_str(), temp );
    lua_pushlstring(L, temp.c_str(), temp.length());
    return 1;
}

wxPoint
InputPort::GetConnPoint() const
{
    return wxPoint(m_bbox.GetLeft(), (m_bbox.GetTop() + m_bbox.GetBottom()) / 2);
}

void
InputPort::AddSource(Shape *source)
{
    if (NumSources() < 2)
    {
        return Port::AddSource(source);
    }
}

wxXmlNode *
InputPort::Serialize() const
{
    wxXmlNode *root = Port::Serialize();
    return root;
}

void
InputPort::Deserialize(const wxXmlNode& root)
{
    Port::Deserialize(root);
}

wxString
InputPort::GetType() const
{
    OutputPort *output = GetSource();
    if (output != NULL)
    {
        return output->GetType();
    }
    return wxT("?");
}

Port::Status
InputPort::GetStatus() const
{
    // Get the output port connected to this port.
    OutputPort *output = GetSource();
    // Check it the port is disconnected...
    if (output == NULL)
    {
        return m_is_required ? Required : Disconnected;
    }
    // Check if the type of this port is ok.
    return CheckType() ? Ok : TypeMismatch;
}

OutputPort *
InputPort::GetSource() const
{
    if (NumSources() == 0)
    {
        return NULL;
    }
    return static_cast<OutputPort *>(m_sources.Last());
}

bool
InputPort::CheckType() const
{
    Method *code = GetMethod(wxT("check-type"), Project::GetProject()->GetTarget());
    if (code != NULL)
    {
        return code->CheckType(this);
    }
    return false;
}

int
InputPort::LuaIndex(lua_State *L)
{
    ShaderObject *obj = *(ShaderObject **)lua_touserdata(L, 1);
    InputPort *input = dynamic_cast<InputPort *>(obj);
    size_t len;
    const char *key = luaL_checklstring(L, 2, &len);
    if (len == 10 && !strcmp(key, "get_source"))
    {
        lua_pushvalue(L, 1);
        lua_pushcclosure(L, LuaGetSource, 1);
        return 1;
    }
    return Port::LuaIndex(L);
}

static int
LuaUID(lua_State *L)
{
    lua_pushfstring(L, "%p", 0);
    return 1;
}

static int
LuaGetType(lua_State *L)
{
    lua_pushliteral(L, "?");
    return 1;
}

int
InputPort::LuaGetSource(lua_State *L)
{
    ShaderObject *obj = *(ShaderObject **)lua_touserdata(L, lua_upvalueindex(1));
    InputPort *input = dynamic_cast<InputPort *>(obj);
    OutputPort *source = input->GetSource();
    if (source != NULL)
    {
        source->PushLua(L);
    }
    else
    {
        lua_newtable(L);
        lua_pushcfunction(L, ::LuaUID);
        lua_setfield(L, -2, "uid");
        lua_pushcfunction(L, ::LuaGetType);
        lua_setfield(L, -2, "get_type");
    }
    return 1;
}

wxPoint
OutputPort::GetConnPoint() const
{
    return wxPoint(m_bbox.GetRight(), (m_bbox.GetTop() + m_bbox.GetBottom()) / 2);
}

wxString
OutputPort::GetType() const
{
    Method *code = GetMethod(wxT("get-type"), Project::GetProject()->GetTarget());
    if (code != NULL)
    {
        return code->GetType(this);
    }
    return wxT("?");
}

Port::Status
OutputPort::GetStatus() const
{
    if (NumTargets() == 0)
    {
        return m_is_required ? Required : Disconnected;
    }
    return Ok;
}
