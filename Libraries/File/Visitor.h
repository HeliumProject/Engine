#pragma once

#ifndef FILE_EXPORTS
#error These declarations are internal to the File API
#endif

#include "TUID/TUID.h"
#include "Reflect/Archive.h"

namespace File
{
  void Serialize( Reflect::SerializeArgs& args );
  void Deserialize( Reflect::DeserializeArgs& args );

  class FileVisitor : public Reflect::ArchiveVisitor
  {
  private:
    std::set<tuid> m_Found;

  public:
    virtual void VisitField(Reflect::Element* element, const Reflect::Field* field) NOC_OVERRIDE;
    virtual void CreateAppendElements(Reflect::V_Element& append) NOC_OVERRIDE;
    virtual void ProcessAppendElements(Reflect::V_Element& append) NOC_OVERRIDE;
  };
}