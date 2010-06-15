#include "Precompiled.h"
#include "Document.h"

FileSignature::Event Document::s_RequestOpenFile;
FileSignature::Event Document::s_RequestSaveFile;

Document::Document()
: m_IsModified (true)
{
  if (!s_RequestOpenFile.Count() || !s_RequestSaveFile.Count())
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

  m_File.Set( file );

  try
  {
    m_Title = m_File.Filename();

    success = m_File.Exists();

    if ( success )
    {
      Reflect::Archive::FromFile( m_File, m_Elements );
    }
  }
  catch ( Nocturnal::Exception& ex )
  {
    wxMessageBox( ex.what(), "Error", wxCENTER | wxICON_ERROR | wxOK );
    success = false;
  }

  if ( success )
  {
    m_IsModified = false;
  }

  return success;
}

bool Document::Revert()
{
  bool success = true;

  if ( m_IsModified )
  {
    if ( wxNO == wxMessageBox( "This document has been modified, do you still wish to revert it?", "Warning", wxCENTER | wxICON_WARNING | wxYES_NO | wxNO_DEFAULT ) )
    {
      return false;
    }
  }

  Reflect::V_Element objects;

  try
  {
    Reflect::Archive::FromFile( m_File, objects );
  }
  catch ( Nocturnal::Exception& ex )
  {
    wxMessageBox( ex.what(), "Error", wxCENTER | wxICON_ERROR | wxOK );
    success = false;
  }

  if ( success )
  {
    m_Elements = objects;
    m_IsModified = false;
  }

  return success;
}

bool Document::Save()
{
  bool success = true;

  if ( m_File.empty() )
  {
    return SaveAs();
  }

  if ( m_IsModified )
  {
    try
    {
      Reflect::Archive::ToFile( m_Elements, m_File );
    }
    catch ( Nocturnal::Exception& ex )
    {
      wxMessageBox( ex.what(), "Error", wxCENTER | wxICON_ERROR | wxOK );
      success = false;
    }
  }

  return success;
}

bool Document::SaveAs()
{
  FileArgs args;
  s_RequestSaveFile.Raise( args );

  if ( !args.m_File.empty() )
  {
    return SaveAs( args.m_File );
  }
  else
  {
    return false;
  }
}

bool Document::SaveAs(const std::string& file)
{
  bool success = true;

  m_File.Set( file );
  m_Title = m_File.Filename();
  m_TitleChanged.Raise( TitleArgs (m_Title) );

  try
  {
    Reflect::Archive::ToFile( m_Elements, m_File );
  }
  catch ( Nocturnal::Exception& ex )
  {
    wxMessageBox( ex.what(), "Error", wxCENTER | wxICON_ERROR | wxOK );
    success = false;
  }

  return success;
}

void Document::SetSelection(const Reflect::V_Element& selection, const SelectionSignature::Delegate& emitter)
{
  if ( m_Selection != selection )
  {
    m_Selection = selection;
    m_SelectionChanged.Raise( m_Selection, emitter );
  }
}