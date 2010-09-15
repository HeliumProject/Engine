#include "ContentCamera.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Camera)

void Camera::EnumerateClass( Reflect::Compositor<Camera>& comp )
{
  comp.AddField( &Camera::m_View, "m_View" );
  comp.AddField( &Camera::m_FOV, "m_FOV" );
  comp.AddField( &Camera::m_Width, "m_Width" );
  comp.AddField( &Camera::m_Height, "m_Height" );
}
