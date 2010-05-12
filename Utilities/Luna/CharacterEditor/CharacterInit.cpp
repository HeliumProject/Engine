#include "Precompile.h"
#include "CharacterInit.h" 
#include "CharacterEditor.h" 
#include "CharacterDocument.h" 
#include "CharacterPreferences.h"

#include "Common/InitializerStack.h"
#include "Editor/SessionManager.h" 
#include "Asset/AssetInit.h"
#include "Reflect/Registry.h"

#include "Reflect/Field.h"
#include "InspectReflect/InspectReflectInit.h"
#include "InspectReflect/ReflectInterpreter.h"
#include "AttachmentCollisionInterpreter.h"

namespace Luna
{
  namespace LunaCharacter
  {
    Nocturnal::InitializerStack g_RegisteredTypes;
    Nocturnal::InitializerStack g_InitializerStack; 
    i32                         g_InitCount = 0; 


    void InitializeModule()
    {
      if(++g_InitCount == 1)
      {
        g_InitializerStack.Push(Reflect::Initialize, Reflect::Cleanup); 
        g_InitializerStack.Push(Asset::Initialize, Asset::Cleanup); 
        g_InitializerStack.Push(CharacterDocument::InitializeType, 
                                CharacterDocument::CleanupType); 
        g_InitializerStack.Push(CharacterEditor::InitializeEditor, 
                                CharacterEditor::CleanupEditor);
        g_InitializerStack.Push(CharacterPreferences::InitializeType, 
                                CharacterPreferences::CleanupType);
        g_InitializerStack.Push(InspectReflect::Initialize,
                                InspectReflect::Cleanup ); 

        Inspect::ReflectFieldInterpreterFactory::Register<LAttachmentCollisionInterpreter>( Reflect::GetType<Reflect::U64ArraySerializer>(), Reflect::FieldFlags::NodeID );

      }
    }

    void CleanupModule()
    {
      if(--g_InitCount == 0)
      {
        g_RegisteredTypes.Cleanup();
        g_InitializerStack.Cleanup(); 
      }
    }
  }
}