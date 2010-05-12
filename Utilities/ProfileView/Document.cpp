#include "Precompiled.h"
#include "Document.h"

#include "FileSystem/FileSystem.h"

FileSignature::Event Document::s_RequestOpenFile;

Document::Document()
{
  if (!s_RequestOpenFile.Count())
  {
    throw Nocturnal::Exception("File event delegates missing");
  }

  m_Title = "New Document";
}

bool Document::Open()
{
  FileArgs args;
  s_RequestOpenFile.Raise( args );

  if ( !args.m_File.empty() )
  {
    return Open( args.m_File );
  }
  else
  {
    return false;
  }
}

bool Document::Open(const std::string& file)
{
  bool success = true;

  m_File = file;

  return success;
}

bool Document::Revert()
{
  bool success = true;

  return success;
}