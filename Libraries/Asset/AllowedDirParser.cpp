#include "AllowedDirParser.h"

#include "Common/Boost/Regex.h" 
#include "Console/Console.h"

#include <tinyxml.h>

// Using
using namespace Asset;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AllowedDirParser::AllowedDirParser()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AllowedDirParser::~AllowedDirParser()
{
}

bool AllowedDirParser::Load( const std::string& file )
{
  //open the config file
  TiXmlDocument doc;
  if (!doc.LoadFile( file.c_str() ))
  {
    Console::Warning("Unable to load allowed directories from %s\n", file.c_str());
    return false;
  }

  //start cycling the elements in the xml config file
  TiXmlElement* dummyElement = doc.FirstChildElement();
  for ( TiXmlElement* templateElement = dummyElement->FirstChildElement(); templateElement != NULL; templateElement = templateElement->NextSiblingElement() )
  {
    //if we've got a Template node
    if ( std::string( templateElement->Value() ) == "Template" )
    {
      AllowedDirSettingsPtr allowedDirSettings = new AllowedDirSettings();
      std::string name = "";
      V_string allowedDirs;
      
      for ( TiXmlElement* currentTemplate = templateElement->FirstChildElement(); currentTemplate != NULL; currentTemplate = currentTemplate->NextSiblingElement() )
      {
        //Grab the class type name
        if ( std::string( currentTemplate->Value() ) == "Name" )
        {
          name = currentTemplate->GetText();
        }

        //Grab the regex for the allowed directory locations of that class type
        else if ( std::string( currentTemplate->Value() ) == "AllowedDir" )
        {
          std::string dir = currentTemplate->GetText();

          //check if the regex has curly braces, which represents a reference to another type
          std::string pattern = "^.*\\{(.*)\\}.*$";
          const boost::regex matchSubDir( pattern, boost::match_single_line );    
          boost::smatch matchResult;
          if ( boost::regex_match( dir, matchResult, matchSubDir ) )
          {
            std::string typeName = ResultAsString( matchResult, 1 );

            //if we found curly braces, do a scan through the classes we've already found, and pull out
            //their allowed dirs and use them in place here
            M_AllowedDirs::iterator foundIt = m_AllowedDirSettings.find( typeName );
            if ( foundIt != m_AllowedDirSettings.end() )
            {
              const V_string& allowedDirsTemp = foundIt->second->m_AllowedDirs;
              V_string::const_iterator foundDirIt = allowedDirsTemp.begin();
              V_string::const_iterator foundDirEnd = allowedDirsTemp.end();
              for ( ; foundDirIt != foundDirEnd ; ++foundDirIt )
              {
                char replace_string[128] = "";
                std::string currDir = dir;
                sprintf( replace_string, "\\{%s\\}", typeName.c_str() ); 
                boost::regex replace_text( replace_string, boost::regex::icase ); 
                currDir = boost::regex_replace( dir, replace_text, (*foundDirIt) ); 
                allowedDirSettings->m_AllowedDirs.push_back( currDir );
              }
              continue;
            }
          }
          allowedDirSettings->m_AllowedDirs.push_back( currentTemplate->GetText() );
        }
        else if ( std::string( currentTemplate->Value() ) == "AboutDirSettings" )
        {
          allowedDirSettings->m_AboutDirSettings = currentTemplate->GetText();
        }
      }
      m_AllowedDirSettings.insert( M_AllowedDirs::value_type( name, allowedDirSettings ) );
    }
  }
  return true;
}

const V_string& AllowedDirParser::GetPatterns( const std::string& typeName ) const
{
  static V_string empty;
  M_AllowedDirs::const_iterator foundIt = m_AllowedDirSettings.find( typeName );
  if ( foundIt != m_AllowedDirSettings.end() )
  {
    return foundIt->second->m_AllowedDirs;
  }
  return empty;
}

const std::string& AllowedDirParser::GetAboutDirSettings( const std::string& typeName ) const
{
  static std::string empty = "";
  M_AllowedDirs::const_iterator foundIt = m_AllowedDirSettings.find( typeName );
  if ( foundIt != m_AllowedDirSettings.end() )
  {
    return foundIt->second->m_AboutDirSettings;
  }
  return empty;
}