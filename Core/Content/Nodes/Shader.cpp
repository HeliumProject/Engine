#include "Core/Content/Nodes/Shader.h"
#include "Core/Content/ContentVisitor.h"

using namespace Helium;
using namespace Helium::Content;
using namespace Helium::Component;

REFLECT_DEFINE_CLASS(Shader)

void Shader::EnumerateClass( Reflect::Compositor<Shader>& comp )
{
  comp.AddField( &Shader::m_WrapU, "m_WrapU" );
  comp.AddField( &Shader::m_WrapV, "m_WrapV" );
  comp.AddField( &Shader::m_RepeatU, "m_RepeatU" );
  comp.AddField( &Shader::m_RepeatV, "m_RepeatV" );
  comp.AddField( &Shader::m_BaseColor, "m_BaseColor" );
  comp.AddField( &Shader::m_AssetPath, "m_AssetPath" );
}

void Shader::Host(ContentVisitor* visitor)
{
  visitor->VisitShader(this); 
}
