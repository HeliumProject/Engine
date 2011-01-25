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
  for (CodeGenerator::FactoryMap::iterator it  = CodeGenerator::getFactoryMap().begin();
                                           it != CodeGenerator::getFactoryMap().end();
                                           it++ )
  {
    m_checkListGenerators->Append(wxString::FromUTF8(it->first.c_str()));
  }
}


void LitesqlGeneratePanel::OnRunClick( wxCommandEvent& event )
{
  m_gaugeRunProgress->SetLabel(_(LITESQL_L("Running")));

struct options_t {
  LITESQL_String output_dir;
  LITESQL_String output_sources;
  LITESQL_String output_includes;
  bool refresh;
  std::vector<LITESQL_String> targets;
};

  options_t options = {LITESQL_L(""),LITESQL_L(""),LITESQL_L(""),true};
  options.targets.push_back(LITESQL_L("c++"));
  options.output_includes = options.output_sources 
                          = options.output_dir 
                          = m_dirPickerOutputDir->GetPath().ToUTF8();

  CompositeGenerator generator;
    
  generator.setOutputDirectory(options.output_dir);
  
  for (size_t index=0; index < m_checkListGenerators->GetCount();index++)
  {
    if (m_checkListGenerators->IsChecked(index))
    {
      LITESQL_String key(m_checkListGenerators->GetString(index).ToUTF8()); 
      CodeGenerator::create(key.c_str());
      CodeGenerator::FactoryMap::iterator it = CodeGenerator::getFactoryMap().find(key);
      if (it != CodeGenerator::getFactoryMap().end() &&  it->second!=NULL)
      {
        generator.add(it->second->create());
      }

    }
  }

  wxString s (generator.generateCode(  m_pModel) ? _(LITESQL_L("Success")) : _(LITESQL_L("Fail")) );
  wxMessageBox(s);
}
