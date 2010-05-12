#include "Precompile.h"
#include "Enumerator.h"

#include "InspectReflect/ReflectInterpreter.h"
#include "InspectSymbol/SymbolInterpreter.h"

#include "Symbol/Symbols.h"
#include "Console/Console.h"
#include "Common/Container/Insert.h" 

#include "Inspect/Label.h"

using namespace Luna;

//#define LUNA_DEBUG_ENUMERATOR

namespace Luna
{
  M_PanelCreators s_PanelCreators;
}

static bool g_Initialized = false;

Enumerator::Enumerator(Inspect::Container* container)
: Inspect::Interpreter (container)
{
}

Enumerator::~Enumerator()
{
}

void Enumerator::Initialize()
{
  g_Initialized = true;
}

void Enumerator::Cleanup()
{
  s_PanelCreators.clear();
  g_Initialized = false;
}

bool Enumerator::InitializePanel(const std::string& name, const CreatePanelSignature::Delegate& creator)
{
  LUNA_CORE_SCOPE_TIMER( ("") );

  NOC_ASSERT(g_Initialized);

  Nocturnal::Insert<M_PanelCreators>::Result inserted = s_PanelCreators.insert(M_PanelCreators::value_type (name, creator));

  if (!inserted.second)
  {
    if (!creator.Equals( inserted.first->second ))
    {
      Console::Error("Panel '%s' initialized with a different creator\n", name.c_str());     
    }
    else
    {
      Console::Warning("Panel '%s' already initialized\n", name.c_str());
    }
  }

  return inserted.second;
}

void Enumerator::CleanupPanel(const std::string& name)
{
  NOC_ASSERT(g_Initialized);

  Console::Print("Cleaned up Panel '%s'\n", name.c_str());

  s_PanelCreators.erase(name);
}

void Enumerator::Reset()
{
  LUNA_CORE_SCOPE_TIMER( ("") );

  m_Container->Clear();
}
