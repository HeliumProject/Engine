#include "Utils.h"

#include "Platform/Assert.h"
#include "Platform/Windows/Windows.h"

using namespace Debug;

const char* Debug::StructuredException::what() const
{
  if (m_StringInfo.empty())
  {
    m_StringInfo = Debug::GetExceptionInfo(m_Info);
    m_StringInfo.resize( m_StringInfo.size() - 1 );  // eat newline
  }

  return m_StringInfo.c_str();
}

void Debug::TranslateException(UINT code, LPEXCEPTION_POINTERS info)
{
  // we don't include the excption code in the C++ exception object because its already in the 
  //  exception record structure, just santity check that its really the case
  NOC_ASSERT(code == info->ExceptionRecord->ExceptionCode);

  throw Debug::StructuredException(info);
}

bool TryExceptCommand::Attempt()
{
  __try
  {
    Execute();
  }
  __except( Debug::ProcessException( GetExceptionInformation(), Debug::ExecuteHandler ) )
  {
    return false;
  }

  return true;
}
