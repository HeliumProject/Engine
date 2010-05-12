#include "Environment.h"

#include "misc.h"

igDXRender::Environment::Environment(const char* fname)
{
  m_load_count = 0;
  m_filename = fname;
  m_crc = StringHashDJB2(fname);
  m_timestamp = (u64)-1L;
  m_env_texture = 0;
  m_clearcolor = 0;
  m_env_bias = 0.0f;
  m_env_scale = 1.0f;
}


igDXRender::Environment::~Environment()
{
  if (m_env_texture)
  {
    m_env_texture->Release();
    m_env_texture=0;
  }
}
