#include "LitesqlMDIChild.h"
#include "litesql-gen-xml.hpp"

using namespace litesql;

LitesqlDocument::LitesqlDocument(LPCTSTR pszFilename)
{
  Load(pszFilename);
}

void LitesqlDocument::setModified(bool bModified) 
{ m_bModified = bModified; }

bool LitesqlDocument::isModified() 
{ return m_bModified; }

const LiteSQL_String& LitesqlDocument::getFilename()
{ return m_filename; }

void LitesqlDocument::setFilename(const LiteSQL_String& filename)
{ m_filename = filename; }

ObjectModel& LitesqlDocument::getModel()
{
  return m_model;
}

bool LitesqlDocument::Load(LPCTSTR pszFilename) 
{
    if (pszFilename!=NULL)
    {
      m_filename= pszFilename;
    }
    else
    {
      m_filename =  LiteSQL_L( "Unbekannt.lsl" );
    }
    setModified(false);
    return m_model.loadFromFile(m_filename);
  }  
  
void LitesqlDocument::Save() 
{  
  XmlGenerator xml_generator;
  
  xml_generator.setOutputFilename(m_filename.c_str());

  xml_generator.generateCode(&m_model);
  
  setModified(false);   
}

void LitesqlDocument::SaveAs(LPCTSTR pszFilename) 
{ 
  setFilename(pszFilename);
  setModified();
  Save(); 
}