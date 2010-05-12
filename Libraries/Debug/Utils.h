#pragma once

#include <exception>

#include "API.h"
#include "Symbols.h"
#include "Exception.h"

namespace Debug
{
  //
  // Helper for handling SEH Exceptions in C++ catch statements
  //

  class StructuredException : public std::exception
  {
  private:
    LPEXCEPTION_POINTERS m_Info;
    mutable std::string m_StringInfo;

  public:
    StructuredException(LPEXCEPTION_POINTERS info)
      : m_Info (info)
    {

    }

    virtual const char* what() const NOC_OVERRIDE;
  };

  //
  // Stack based enabler to ensure SEH translator setup/restoration
  //  this technically doesn't HAVE to be a template, but if it wasn't it would generate a compiler warning in code files
  //  that are compiled without /EHa, even IF they don't actually instantiate it.  Since the code for templates is generated
  //  only upon instantiation this delays that compiler warning only to code files that use this /EHa-dependent features of Debug
  //

  template<_se_translator_function translator>
  class EnableTranslator
  {
  private:
    _se_translator_function m_Previous;

  public:
    EnableTranslator()
    {
      m_Previous = _set_se_translator(&translator);
    }

    ~EnableTranslator()
    {
      _set_se_translator(m_Previous);
    }
  };

  // translates an SEH exception into a StructuredException when compiling with /EHa
  DEBUG_API void TranslateException(UINT code, LPEXCEPTION_POINTERS info);

  //
  // Alternatively, subclassing this command class will better allow mixing of SEH and C++ objects w/ constructor/destructor functions
  //  to use it just initialize references to stack variables in the derived constructor
  //  and use them in your Execute function implementation
  //

  class DEBUG_API TryExceptCommand
  {
  public:
    bool Attempt();

  protected:
    virtual void Execute() = 0;
  };
}