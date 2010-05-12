#include "Precompile.h" 
#include "CharacterDocument.h" 
#include "CharacterInit.h"


namespace Luna
{
  LUNA_DEFINE_TYPE(CharacterDocument); 

  void CharacterDocument::InitializeType()
  {
    Luna::LunaCharacter::g_RegisteredTypes.Push( Reflect::RegisterClass<CharacterDocument>( "CharacterDocument" ) ); 
  }

  void CharacterDocument::CleanupType()
  {
  }

  // implementation.... 
    
  CharacterDocument::CharacterDocument(const std::string& file)
    : Document(file)
  {


  }

  CharacterDocument::~CharacterDocument()
  {


  }


}
