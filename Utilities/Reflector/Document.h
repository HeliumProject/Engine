#pragma once

#include "Reflect/Element.h"

struct FileArgs
{
  std::string m_File;

  FileArgs()
  {

  }
};
typedef Nocturnal::Signature<void, FileArgs&> FileSignature;

struct TitleArgs
{
  std::string m_Title;

  TitleArgs( const std::string& title )
    : m_Title( title )
  {

  }
};
typedef Nocturnal::Signature<void, const TitleArgs&> TitleSignature;

struct ElementArgs
{
  Reflect::Element* m_Element;

  ElementArgs( Reflect::Element* element )
    : m_Element( element )
  {

  }
};
typedef Nocturnal::Signature<void, const ElementArgs&> ElementSignature;

struct SelectionArgs
{
  const Reflect::V_Element& m_Selection;

  SelectionArgs( const Reflect::V_Element& selection )
    : m_Selection( selection )
  {

  }
};
typedef Nocturnal::Signature<void, const SelectionArgs&> SelectionSignature;

class Document : public Nocturnal::RefCountBase<Document>
{
public:
  Document();

  bool Open();
  bool Open(const std::string& file);
  bool Revert();
  bool Save();
  bool SaveAs();
  bool SaveAs(const std::string& files);

  const std::string& GetFile()
  {
    return m_File;
  }

  const std::string& GetTitle()
  {
    return m_Title;
  }

  const Reflect::V_Element& GetElements()
  {
    return m_Elements;
  }

  const Reflect::V_Element& GetSelection()
  {
    return m_Selection;
  }

  void SetSelection( const Reflect::V_Element& selection, const SelectionSignature::Delegate& emitter = SelectionSignature::Delegate () );

  bool IsModified()
  {
    return m_IsModified;
  }

  TitleSignature::Event& GetTitleChangedEvent()
  {
    return m_TitleChanged;
  }

  ElementSignature::Event& GetElementAddedEvent()
  {
    return m_ElementAdded;
  }

  ElementSignature::Event& GetElementRemovedEvent()
  {
    return m_ElementRemoved;
  }

  SelectionSignature::Event& GetSelectionChangedEvent()
  {
    return m_SelectionChanged;
  }

  static FileSignature::Event& GetRequestOpenFileEvent()
  {
    return s_RequestOpenFile;
  }

  static FileSignature::Event& GetRequestSaveFileEvent()
  {
    return s_RequestSaveFile;
  }

private:
  // document data
    Nocturnal::Path                 m_File;
  std::string                 m_Title;
  Reflect::V_Element          m_Elements;
  Reflect::V_Element          m_Selection;
  bool                        m_IsModified;

  // events
  TitleSignature::Event       m_TitleChanged;
  ElementSignature::Event     m_ElementAdded;
  ElementSignature::Event     m_ElementRemoved;
  SelectionSignature::Event   m_SelectionChanged;
  static FileSignature::Event s_RequestOpenFile;
  static FileSignature::Event s_RequestSaveFile;
};

typedef Nocturnal::SmartPtr<Document> DocumentPtr;