#pragma once

#include "Foundation/Automation/Event.h"

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

class Document : public Nocturnal::RefCountBase<Document>
{
public:
  Document();

  bool Open();
  bool Open(const std::string& file);
  bool Revert();

  const std::string& GetFile()
  {
    return m_File;
  }

  const std::string& GetTitle()
  {
    return m_Title;
  }

  TitleSignature::Event& GetTitleChangedEvent()
  {
    return m_TitleChanged;
  }

  static FileSignature::Event& GetRequestOpenFileEvent()
  {
    return s_RequestOpenFile;
  }

private:
  // document data
  std::string                 m_File;
  std::string                 m_Title;

  // events
  TitleSignature::Event       m_TitleChanged;
  static FileSignature::Event s_RequestOpenFile;
};

typedef Nocturnal::SmartPtr<Document> DocumentPtr;