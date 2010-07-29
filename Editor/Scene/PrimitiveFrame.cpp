#include "Precompile.h"
#include "PrimitiveFrame.h"

#include "Pick.h"
#include "Color.h"

using namespace Math;
using namespace Editor;

PrimitiveFrame::PrimitiveFrame(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
  SetElementCount( 9 );
  SetElementType( ElementTypes::TransformedColored );

  wxColour color = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

  m_BorderColor = D3DCOLOR_ARGB(255, color.Red(), color.Green(), color.Blue());
  m_InnerColor = Editor::Color::BlendColor(m_BorderColor, D3DCOLOR_ARGB(0, 0, 0, 0), 0.5f);
}

void PrimitiveFrame::Update()
{
  m_Vertices.clear();

  m_Vertices.push_back(TransformedColored ((float)m_Start.x,  (float)m_Start.y,   1.0f,   m_InnerColor));
  m_Vertices.push_back(TransformedColored ((float)m_Start.x,  (float)m_End.y,     1.0f,   m_InnerColor));
  m_Vertices.push_back(TransformedColored ((float)m_End.x,    (float)m_End.y,     1.0f,   m_InnerColor));
  m_Vertices.push_back(TransformedColored ((float)m_End.x,    (float)m_Start.y,   1.0f,   m_InnerColor));

  m_Vertices.push_back(TransformedColored ((float)m_Start.x,  (float)m_Start.y,   1.0f,   m_BorderColor));
  m_Vertices.push_back(TransformedColored ((float)m_Start.x,  (float)m_End.y,     1.0f,   m_BorderColor));
  m_Vertices.push_back(TransformedColored ((float)m_End.x,    (float)m_End.y,     1.0f,   m_BorderColor));
  m_Vertices.push_back(TransformedColored ((float)m_End.x,    (float)m_Start.y,   1.0f,   m_BorderColor));
  m_Vertices.push_back(TransformedColored ((float)m_Start.x,  (float)m_Start.y,   1.0f,   m_BorderColor));

  __super::Update();
}

void PrimitiveFrame::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  D3DCULL cull;
  m_Device->GetRenderState(D3DRS_CULLMODE, (DWORD*)&cull);
  m_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
  {
    m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+4, 1);
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+5, 1);
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+6, 1);
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+7, 1);
  }
  m_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
  m_Device->SetRenderState(D3DRS_CULLMODE, cull);

  args->m_LineCount += 4;
}

bool PrimitiveFrame::Pick( PickVisitor* pick, const bool* solid ) const
{
  return false;
}