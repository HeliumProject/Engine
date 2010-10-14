#include <wx/msgdlg.h>

#include "litesql.hpp"
#include "generator.hpp"
#include "litesql-gen-cpp.hpp"
#include "litesql-gen-graphviz.hpp"
#include "litesql-gen-ruby-activerecord.hpp"

#include "LitesqlGeneratePanel.h"

using namespace litesql;

LitesqlGeneratePanel::LitesqlGeneratePanel( wxWindow* parent,litesql::ObjectModel* pModel )
:
GeneratePanel( parent ),
m_pModel(pModel)
{

}


void LitesqlGeneratePanel::OnRunClick( wxCommandEvent& event )
{
  m_gaugeRunProgress->SetLabel(_( LITESQL_L( "Running" )));

struct options_t {
  LITESQL_String output_dir;
  LITESQL_String output_sources;
  LITESQL_String output_includes;
  bool refresh;
  std::vector<LITESQL_String> targets;
};

  options_t options = { LITESQL_L( "" ), LITESQL_L( "" ), LITESQL_L( "" ),true};
  options.targets.push_back( LITESQL_L( "c++" ));

  CompositeGenerator generator;
    
  generator.setOutputDirectory(options.output_dir);
  
  
  for (std::vector<LITESQL_String>::const_iterator target= options.targets.begin(); target!=options.targets.end();target++)
  {

    if (*target ==  LITESQL_L( "c++" )) 
    {
      CppGenerator* pCppGen = new CppGenerator();
      pCppGen->setOutputSourcesDirectory(options.output_sources);
      pCppGen->setOutputIncludesDirectory(options.output_includes);

      generator.add(pCppGen);
    }    
    else if (*target ==  LITESQL_L( "graphviz" )) 
    {
      generator.add(new GraphvizGenerator());
    }
    else if (*target ==  LITESQL_L( "ruby-activerecord" )) 
    {
      generator.add(new RubyActiveRecordGenerator());
    }
    else 
    {
      throw litesql::Except( LITESQL_L( "unsupported target: " ) + *target);
    }
  }

  wxString s (generator.generateCode(  m_pModel) ? _( LITESQL_L( "Success" )) : _( LITESQL_L( "Fail" )) );
  wxMessageBox(s);

}
