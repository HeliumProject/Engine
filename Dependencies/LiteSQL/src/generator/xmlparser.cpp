#include "litesql_char.hpp"
#include <iostream>

#include <string.h>
#include <cstdio>

#include "xmlparser.hpp"
#include "logger.hpp"

using namespace litesql;
using namespace xml;

namespace xml {
void XMLParser_xmlSAX2StartElement		(void *ctx,
						 const XML_Char *fullname,
						 const XML_Char **atts)
{
   ((XmlParser*)ctx)->onStartElement(fullname,atts);
}

void XMLParser_xmlSAX2EndElement(void *ctx,const XML_Char *name)
{
   ((XmlParser*)ctx)->onEndElement(name);
}

}

XmlParser::~XmlParser()
{
   XML_ParserFree(saxHandler);
}

bool XmlParser::parseFile(const LITESQL_String& filename)
{
  saxHandler = XML_ParserCreate(LITESQL_L("UTF-8"));
  XML_SetUserData(saxHandler,this);
  XML_SetElementHandler(saxHandler,
    XMLParser_xmlSAX2StartElement,
    XMLParser_xmlSAX2EndElement);

  const size_t BUFF_SIZE = 255;
  FILE* docfd = _tfopen(filename.c_str(), LITESQL_L("r"));
  
  bool success = (docfd !=NULL);
  if (!success)
  {
    Logger::error(LITESQL_L("cant open %s"),filename.c_str());
  }
  else {
    for (;;) {
    int bytes_read;
    void *buff = XML_GetBuffer(saxHandler, BUFF_SIZE);
    /* handle error */
    if (buff == NULL) {
      success = false;
      break;
    }

    bytes_read = fread(buff,1, BUFF_SIZE,docfd);
    if (bytes_read < 0) {
      /* handle error */
      success = false;
      break;
    }

    if (! XML_ParseBuffer(saxHandler, bytes_read, bytes_read == 0)) {
      /* handle parse error */
      success = false;
      break;
    }

    if (bytes_read == 0)
      break;
    }
    fclose(docfd);
  }
  
  if (!success)
   {
      LITESQL_cerr <<  LITESQL_L("error parsing ") << filename.c_str() << std::endl;
   }
  return success;
}

const XML_Char* XmlParser::xmlGetAttrValue(const XML_Char** attrs,const XML_Char* key)
{
   if (attrs!=NULL)
   {      
      for (size_t i = 0; attrs[i]!=NULL;i+=2)
      {
         if (!_tcscmp(attrs[i],key))
         {
            return attrs[i+1];
         } 
      }
   }
   return NULL;
}