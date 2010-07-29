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
  m_gaugeRunProgress->SetLabel(_( LiteSQL_L( "Running" )));

struct options_t {
  LiteSQL_String output_dir;
  LiteSQL_String output_sources;
  LiteSQL_String output_includes;
  bool refresh;
  std::vector<LiteSQL_String> targets;
};

  options_t options = { LiteSQL_L( "" ), LiteSQL_L( "" ), LiteSQL_L( "" ),true};
  options.targets.push_back( LiteSQL_L( "c++" ));

  CompositeGenerator generator;
    
  generator.setOutputDirectory(options.output_dir);
  
  
  for (std::vector<LiteSQL_String>::const_iterator target= options.targets.begin(); target!=options.targets.end();target++)
  {

    if (*target ==  LiteSQL_L( "c++" )) 
    {
      CppGenerator* pCppGen = new CppGenerator();
      pCppGen->setOutputSourcesDirectory(options.output_sources);
      pCppGen->setOutputIncludesDirectory(options.output_includes);

      generator.add(pCppGen);
    }    
    else if (*target ==  LiteSQL_L( "graphviz" )) 
    {
      generator.add(new GraphvizGenerator());
    }
    else if (*target ==  LiteSQL_L( "ruby-activerecord" )) 
    {
      generator.add(new RubyActiveRecordGenerator());
    }
    else 
    {
      throw litesql::Except( LiteSQL_L( "unsupported target: " ) + *target);
    }
  }

  wxString s (generator.generateCode(  m_pModel) ? _( LiteSQL_L( "Success" )) : _( LiteSQL_L( "Fail" )) );
  wxMessageBox(s);

}
