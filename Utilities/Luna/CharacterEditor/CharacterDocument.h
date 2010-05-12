#pragma once

#include "Editor/Document.h" 
#include "Asset/AssetClass.h" 

namespace Luna
{
  class CharacterDocument : public Document
  {
  private: 
    LUNA_DECLARE_TYPE(CharacterDocument, Document); 
    static void InitializeType(); 
    static void CleanupType(); 

  public: 
    CharacterDocument(const std::string& file); 
    virtual ~CharacterDocument(); 

    std::string m_EntityPath; 
    std::string m_SkeletonPath; 
    std::string m_CollisionPath;

    std::string m_LocalModelPath; 
    std::string m_MasterModelPath; 

    std::string m_LocalRiggedPath; 
    std::string m_MasterRiggedPath; 

  }; 

  typedef Nocturnal::SmartPtr<CharacterDocument> CharacterDocumentPtr; 
}
