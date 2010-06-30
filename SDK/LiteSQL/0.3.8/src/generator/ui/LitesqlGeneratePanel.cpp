#include <wx/msgdlg.h>

#include "litesql.hpp"
#include "generator.hpp"
#include "litesql-gen-cpp.hpp"
#include "litesql-gen-graphviz.hpp"
#include "litesql-gen-ruby-activerecord.hpp"

#include "LitesqlGeneratePanel.h"

using namespace std;
using namespace litesql;

LitesqlGeneratePanel::LitesqlGeneratePanel( wxWindow* parent,litesql::ObjectModel* pModel )
:
GeneratePanel( parent ),
m_pModel(pModel)
{

}


void LitesqlGeneratePanel::OnRunClick( wxCommandEvent& event )
{
  m_gaugeRunProgress->SetLabel(_("Running"));

struct options_t {
  string output_dir;
  string output_sources;
  string output_includes;
  bool refresh;
  vector<string> targets;
};

  options_t options = {"","","",true};
  options.targets.push_back("c++");

  CompositeGenerator generator;
    
  generator.setOutputDirectory(options.output_dir);
  
  
  for (vector<string>::const_iterator target= options.targets.begin(); target!=options.targets.end();target++)
  {

    if (*target == "c++") 
    {
      CppGenerator* pCppGen = new CppGenerator();
      pCppGen->setOutputSourcesDirectory(options.output_sources);
      pCppGen->setOutputIncludesDirectory(options.output_includes);

      generator.add(pCppGen);
    }    
    else if (*target == "graphviz") 
    {
      generator.add(new GraphvizGenerator());
    }
    else if (*target == "ruby-activerecord") 
    {
      generator.add(new RubyActiveRecordGenerator());
    }
    else 
    {
      throw litesql::Except("unsupported target: " + *target);
    }
  }

  wxString s (generator.generateCode(  m_pModel) ? _("Success") : _("Fail") );
  wxMessageBox(s);

}

