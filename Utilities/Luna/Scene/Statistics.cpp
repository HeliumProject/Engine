#include "Precompile.h"
#include "Statistics.h"

#include "Foundation/Profile.h"

using namespace Luna;

Statistics::Statistics(IDirect3DDevice9* device)
: m_Device (device)
, m_Font (NULL)
, m_FrameNumber (0)
, m_Previous (0)
{
  Create();
  Reset();
}

Statistics::~Statistics()
{
  if (m_Font != NULL)
  {
    m_Font->Release();
  }
}

void Statistics::Reset()
{
  m_FrameCount = 0;

  m_RenderTime = 0;
  m_RenderWalkTime = 0;
  m_RenderSortTime = 0;
  m_RenderCompareTime = 0;
  m_RenderDrawTime = 0;

  m_EntryCount = 0;
  m_TriangleCount = 0;
  m_LineCount = 0;

  m_EvaluateTime = 0;
  m_NodeCount = 0;
}

void Statistics::Update()
{
  // update every second
  u64 ticks = Platform::TimerGetClock();

  float elapsed = Platform::CyclesToMillis(ticks - m_Previous);

  if (m_Previous == 0 || elapsed >= 1000)
  {
    if (m_FrameCount == 0)
    {
      return;
    }

    m_FramesPerSecondResult = m_FrameCount;
    m_FrameRateResult = 1.0f / m_RenderTimeResult * 1000.f;

    m_RenderTimeResult = m_RenderTime / (float)(m_FrameCount);
    m_RenderWalkTimeResult = m_RenderWalkTime / (float)(m_FrameCount);
    m_RenderSortTimeResult = m_RenderSortTime / (float)(m_FrameCount);
    m_RenderCompareTimeResult = m_RenderCompareTime / (float)(m_FrameCount);
    m_RenderDrawTimeResult = m_RenderDrawTime / (float)(m_FrameCount);

    m_EntryCountResult = m_EntryCount / m_FrameCount;
    m_TriangleCountResult = m_TriangleCount / m_FrameCount;
    m_LineCountResult = m_LineCount / m_FrameCount;

    m_EvaluateTimeResult = m_EvaluateTime / (float)(m_FrameCount);
    m_NodeCountResult = m_NodeCount / m_FrameCount;

    Reset();

    m_Previous = ticks;
  }
}

void Statistics::Create()
{
  if (m_Font == NULL)
  {
    HRESULT result = D3DXCreateFont( m_Device,
                                     14,
                                     0,
                                     700,
                                     1,
                                     FALSE,
                                     DEFAULT_CHARSET,
                                     OUT_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                     DEFAULT_PITCH | FF_DONTCARE,
                                     TXT( "Arial" ),
                                     &m_Font );

    NOC_ASSERT(SUCCEEDED(result));
  }
  else
  {
    m_Font->OnResetDevice();
  }
}

void Statistics::Delete()
{
  m_Font->OnLostDevice();
}

void Statistics::Draw( DrawArgs* args )
{
  int result = 0, space = 15;
  DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);

  //
  // Rendering
  //

  RECT rect;
  rect.top = -10;
  rect.left = 5;
  rect.right = 0;
  rect.bottom = 0;
  char buf[256];

  rect.top += space;
  sprintf(buf, "Frame Number: %d", m_FrameNumber);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "Frames Per Second: %d", m_FramesPerSecondResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "Frame Rate: %d", m_FrameRateResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);


  rect.top += space;

  rect.top += space;
  sprintf(buf, "Render Time: %f", m_RenderTimeResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Render Walk Time: %f", m_RenderWalkTimeResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Render Sort Time: %f", m_RenderSortTimeResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "    Compare Time: %f", m_RenderCompareTimeResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Render Draw Time: %f", m_RenderDrawTimeResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Entry Count: %d", m_EntryCountResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Triangle Count: %d", m_TriangleCountResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Line Count: %d", m_LineCountResult);
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);


  //
  // Evaluation
  //

  rect.top += space;

  rect.top += space;
  sprintf(buf, "Evaluate Time: %f", m_EvaluateTimeResult );
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);

  rect.top += space;
  sprintf(buf, "  Nodes Evaluated: %d", m_NodeCountResult );
  result = m_Font->DrawTextA(NULL, buf, -1, &rect, DT_NOCLIP, color);
}
