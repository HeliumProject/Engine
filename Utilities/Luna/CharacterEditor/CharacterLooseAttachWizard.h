#pragma once

#include "CharacterForms.h" 
#include "CharacterManager.h"
#include "Content/LooseAttachChain.h"

namespace Luna
{
  class CharacterEditor; 

  class CharacterLooseAttachWizardSingle : public LooseAttachWizardSingle
  {
  public: 
    CharacterLooseAttachWizardSingle(CharacterEditor* editor); 
    virtual ~CharacterLooseAttachWizardSingle(); 

		virtual void OnClose( wxCloseEvent& event ); 
		virtual void OnButtonPickStart( wxCommandEvent& event ); 
		virtual void OnButtonCancel( wxCommandEvent& event ); 
		virtual void OnButtonFinish( wxCommandEvent& event ); 

  private: 
    CharacterEditor*            m_Editor; 
    Content::LooseAttachChainPtr m_Chain; 

  }; 

  class CharacterLooseAttachWizardDouble : public LooseAttachWizardDouble
  {
  public: 
    CharacterLooseAttachWizardDouble(CharacterEditor* editor); 
    virtual ~CharacterLooseAttachWizardDouble(); 

    virtual void OnClose( wxCloseEvent& event ); 
 		virtual void OnButtonPickStart( wxCommandEvent& event );
		virtual void OnButtonPickMiddle( wxCommandEvent& event );
		virtual void OnButtonPickEnd( wxCommandEvent& event );
		virtual void OnButtonCancel( wxCommandEvent& event );
		virtual void OnButtonFinish( wxCommandEvent& event );

  private: 

    CharacterEditor*            m_Editor; 
    Content::LooseAttachChainPtr m_Chain; 

  }; 


}
