#include "stdafx.h"
#include "Script.h"
#include "Container.h"
#include "Label.h"
#include "Value.h"
#include "CheckBox.h"
#include "ColorPicker.h"
#include "Choice.h"
#include "List.h"
#include "Slider.h"

#include "Console/Console.h"

#include <boost/regex.hpp> 

using namespace Inspect;

// #define INSPECT_DEBUG_SCRIPT_COMPILE

// L strings
#define LS_REGEX_DELIM_BEGIN "UI\\[\\.\\["
#define LS_REGEX_DELIM_END "\\]\\.\\]"
#define LS_WHITESPACE " \t\n"

// L chars
#define LC_COMMENT "#"


//
// Control Registry
//

typedef std::map<std::string, int> M_ControlType;

M_ControlType g_ControlTypeMap;


//
// Script Parsing
//

void Script::Initialize()
{
  if (g_ControlTypeMap.size() > 0)
  {
    return;
  }

  struct ControlEntry
  {
    const char* name;
    int type;
  };

  const ControlEntry controls[] =
  {
    {"label",       Reflect::GetType<Label>()},
    {"value",       Reflect::GetType<Value>()},
    {"slider",      Reflect::GetType<Slider>()},
    {"check",       Reflect::GetType<CheckBox>()},
    {"color",       Reflect::GetType<ColorPicker>()},
    {"choice",      Reflect::GetType<Choice>()},
    {"combo",       Reflect::GetType<Choice>()},
    {"list",        Reflect::GetType<List>()},
  };

  int size = sizeof(controls)/sizeof(controls[0]);
  for (int i=0; i<size; i++)
  {
    g_ControlTypeMap[ controls[i].name ] = controls[i].type;
  }
}

void Script::Cleanup()
{
  g_ControlTypeMap.clear();
}

bool Script::PreProcess(std::string& script)
{
  INSPECT_SCOPE_TIMER( ("Attributes Script Pre-Processing") );

  //
  // Debug
  //

#ifdef INSPECT_DEBUG_SCRIPT_COMPILE
  Console::Debug("Pre-preprocessed:\n\n%s\n\n", script.c_str());
#endif

  //
  // Check for L code
  //

  const boost::regex cfPattern ( ".*"LS_REGEX_DELIM_BEGIN".*"LS_REGEX_DELIM_END".*" ); 

  boost::smatch matchResult; 
  if(!boost::regex_search(script, matchResult, cfPattern))
  {
    return false; 
  }

  //
  // Cull outside of our begin/end descriptors
  //

  // the .* at the end of this secretly culls the rest of the string for you
  // including comments and additional UI[.[ (.*) ].] 
  // 
  const boost::regex cfStartEndPattern ( ".*"LS_REGEX_DELIM_BEGIN"(.*)"LS_REGEX_DELIM_END".*"); 
  script = boost::regex_replace(script, cfStartEndPattern, "$1"); 

  //
  // Cull comments
  //
  
  const boost::regex cfCommentPattern ( LC_COMMENT".*\n" ); 
  script = boost::regex_replace(script, cfCommentPattern, "\n"); 

  //
  // Debug
  //

#ifdef INSPECT_DEBUG_SCRIPT_COMPILE
  Console::Debug("Post-preprocessed:\n\n%s\n\n\n\n", script.c_str());
#endif

  return true;
}

void Script::ParseAttributes(std::string& attributes, Control* control)
{


  INSPECT_SCOPE_TIMER( ("Attributes Script Attribute Processing") );
  
  size_t pos = 0;
  size_t end = std::string::npos;

  while (pos < attributes.length() && pos != std::string::npos)
  {
    // eat ws
    pos = attributes.find_first_not_of(LS_WHITESPACE, pos);

    // the rest is WS, abort
    if (pos == std::string::npos)
      break;

    // search for end of keyword
    end = attributes.find_first_of(LS_WHITESPACE"=", pos);

    // we have no symbol term, just abort
    if (end == std::string::npos)
      break;

    // copy just our symbol into a string
    std::string key (attributes.data() + pos, end - pos);

    // next char
    pos = end+1;

    // eat ws
    pos = attributes.find_first_not_of(LS_WHITESPACE, pos);

    // the rest is WS, abort
    if (pos == std::string::npos)
      break;

    // see if the value is directly quoted
    size_t startQuote = attributes.find_first_of("\"", pos);
    size_t endQuote = attributes.find_first_of("\"", startQuote+1);

    // search for end of keyword
    end = attributes.find_first_of(";", pos);

    // if the semi is in the quote
    if (startQuote != std::string::npos && endQuote != std::string::npos && startQuote < end && end < endQuote)
    {
      // search for end of value not quoted
      end = attributes.find_first_of(";", endQuote);
    }

    // we have no symbol term, just abort
    if (end == std::string::npos)
    {
      end = attributes.length();
    }

    // copy just our symbol into a string
    std::string value (attributes.data() + pos, end - pos);

    // next char
    pos = end+1;

    // trim quoted values
    {
      size_t start = value.find_first_of('\"');
      size_t finish = value.find_last_of('\"');

      if (start != std::string::npos)
      {
        if (start == finish)
        {
          value.erase(start, 1);
        }
        else if (start < finish)
        {
          value = value.substr(start + 1, finish - start - 1);
        }
      }
    }

    // insert
    control->Process(key, value);
  }
}

bool Script::Parse(const std::string& script, Interpreter* interpreter, Canvas* canvas, Container* output, u32 fieldFlags)
{
  INSPECT_SCOPE_TIMER( ("Attributes Script Parsing") );

  // make working copy
  std::string str = script;

  // remove delimiters and exterior string data
  if (!PreProcess(str))
    return false;

  size_t pos = 0;
  size_t end = std::string::npos;

  while (pos < str.length() && pos != std::string::npos)
  {
    // eat ws
    pos = str.find_first_not_of(LS_WHITESPACE, pos);

    // the rest is WS, abort
    if (pos == std::string::npos)
      break;

    // search for end of keyword
    end = str.find_first_of(LS_WHITESPACE"{", pos);

    // we have no symbol term, just abort
    if (end == std::string::npos)
      break;

    // this shouldn't happen
    if (pos > end)
      break;
    
    // copy just our symbol into a string
    std::string symbol (str.data() + pos, end - pos);


    //
    // Lookup type
    //

    M_ControlType::iterator i = g_ControlTypeMap.find(symbol);

    if (i == g_ControlTypeMap.end())
    {
      Console::Warning("Undefined script symbol \"%s\"\n", symbol.c_str());
      return false;
    }


    //
    // Create control
    //

    ControlPtr control = Reflect::ObjectCast<Inspect::Control> ( Reflect::Registry::GetInstance()->CreateInstance( i->second ) );

    control->SetInterpreter( interpreter );

    control->SetCanvas( canvas );

    if (control == NULL)
    {
      Console::Warning("Unable to construct control \"%s\"\n", symbol.c_str());
      return false;
    }

    bool readOnly = ( fieldFlags & Reflect::FieldFlags::ReadOnly ) == Reflect::FieldFlags::ReadOnly;
    control->SetReadOnly( readOnly );


    //
    // Process attributes
    //

    pos = str.find_first_of('{', pos);
    end = str.find_first_of('}', pos);


    if (pos != std::string::npos && 
        end != std::string::npos &&
        pos < end && pos+1 != end)
    {
      ParseAttributes(std::string (str.data() + pos + 1, end - pos - 1), control);
    }


    //
    // Add control
    //

    output->AddControl(control);

    // keep going
    pos = end+1;
  }

  return true;
}
