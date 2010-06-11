#include "Shader.h"
#include "ContentVisitor.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "Finder/ShaderSpecs.h"
#include "Finder/AssetSpecs.h"
#include "UID/TUID.h"
#include "Attribute/AttributeHandle.h"
#include "Foundation/Log.h"
#include "rcs/rcs.h"

using namespace Reflect;
using namespace Content;
using namespace Attribute; 

REFLECT_DEFINE_CLASS(Shader)

void Shader::EnumerateClass( Reflect::Compositor<Shader>& comp )
{
  Reflect::Field* fieldWrapU = comp.AddField( &Shader::m_WrapU, "m_WrapU" );
  Reflect::Field* fieldWrapV = comp.AddField( &Shader::m_WrapV, "m_WrapV" );
  Reflect::Field* fieldRepeatU = comp.AddField( &Shader::m_RepeatU, "m_RepeatU" );
  Reflect::Field* fieldRepeatV = comp.AddField( &Shader::m_RepeatV, "m_RepeatV" );
  Reflect::Field* fieldBaseColor = comp.AddField( &Shader::m_BaseColor, "m_BaseColor" );
}

void Shader::Host(ContentVisitor* visitor)
{
  visitor->VisitShader(this); 
}
