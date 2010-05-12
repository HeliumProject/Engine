#include "Precompile.h"
#include "CharacterLooseAttachWizard.h" 
#include "CharacterEditor.h"
#include "CharacterManager.h"

#include "UIToolKit/ImageManager.h"

namespace Luna
{
  //------------------------------------------------------------
  // 
  // single-ended loose attachment chain starting panel
  //

  CharacterLooseAttachWizardSingle::CharacterLooseAttachWizardSingle(CharacterEditor* editor)
    : LooseAttachWizardSingle(editor)
    , m_Editor(editor)
    , m_Chain( new Content::LooseAttachChain() )
  {
    m_ButtonPickStart->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "select_16.png" ) ); 
    m_Chain->SetChainType( Content::LooseAttachChain::SingleEnded ); 

  }

  CharacterLooseAttachWizardSingle::~CharacterLooseAttachWizardSingle()
  {

  }

  void CharacterLooseAttachWizardSingle::OnClose( wxCloseEvent& event )
  {
    m_Editor->CloseWizard(this); 
  }

  void CharacterLooseAttachWizardSingle::OnButtonPickStart( wxCommandEvent& event )
  {
    Content::JointTransformPtr selected = m_Editor->GetManager()->GetSelectedJoint(); 

    m_Chain->SetJoint( Content::LooseAttachChain::JointStart, selected ); 

    if(selected)
    {
      m_StartJointText->SetLabel( selected->GetName() ); 
      if(m_NameText->GetLabelText() == "")
      {
        m_NameText->SetLabel(selected->GetName()); 
      }

    }
    else
    {
      m_StartJointText->SetLabel( "<no joint selected>" ); 
    }
  }

  void CharacterLooseAttachWizardSingle::OnButtonCancel( wxCommandEvent& event )
  {
    m_Editor->CloseWizard(this);        
  }

  void CharacterLooseAttachWizardSingle::OnButtonFinish( wxCommandEvent& event )
  {
    m_Chain->m_Name = m_NameText->GetLabelText(); 

    bool ok = m_Editor->GetManager()->AddLooseAttachChain( m_Chain ); 
    if(ok)
    {
      m_Editor->CloseWizard(this); 
    }


  }


  //------------------------------------------------------------
  // 
  // double-ended loose attachment chain starting panel
  //

  CharacterLooseAttachWizardDouble::CharacterLooseAttachWizardDouble(CharacterEditor* editor)
    : LooseAttachWizardDouble(editor)
    , m_Editor(editor)
    , m_Chain( new Content::LooseAttachChain() )
  {
    m_ButtonPickStart->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "select_16.png" ) ); 
    m_ButtonPickMiddle->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "select_16.png" ) ); 
    m_ButtonPickEnd->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "select_16.png" ) ); 

    m_Chain->SetChainType( Content::LooseAttachChain::DoubleEnded ); 

  }

  CharacterLooseAttachWizardDouble::~CharacterLooseAttachWizardDouble()
  {

  }

  void CharacterLooseAttachWizardDouble::OnClose( wxCloseEvent& event )
  {
    m_Editor->CloseWizard(this); 
  }

	void CharacterLooseAttachWizardDouble::OnButtonCancel( wxCommandEvent& event )
  {
    m_Editor->CloseWizard(this); 
  }

  void CharacterLooseAttachWizardDouble::OnButtonPickStart( wxCommandEvent& event )
  {
    Content::JointTransformPtr selected = m_Editor->GetManager()->GetSelectedJoint(); 

    m_Chain->SetJoint( Content::LooseAttachChain::JointStart, selected ); 

    if(selected)
    {
      m_StartJointText->SetLabel( selected->GetName() ); 
      if(m_NameText->GetLabelText() == "")
      {
        m_NameText->SetLabel(selected->GetName()); 
      }
    }
    else
    {
      m_StartJointText->SetLabel( "<no joint selected>" ); 
    }
  }

  void CharacterLooseAttachWizardDouble::OnButtonPickMiddle( wxCommandEvent& event )
  {
    Content::JointTransformPtr selected = m_Editor->GetManager()->GetSelectedJoint(); 

    m_Chain->SetJoint( Content::LooseAttachChain::JointMiddle, selected ); 

    if(selected)
    {
      m_MiddleJointText->SetLabel( selected->GetName() ); 
    }
    else
    {
      m_MiddleJointText->SetLabel( "<no joint selected>" ); 
    }
  }

	void CharacterLooseAttachWizardDouble::OnButtonPickEnd( wxCommandEvent& event )
  {
    Content::JointTransformPtr selected = m_Editor->GetManager()->GetSelectedJoint(); 

    m_Chain->SetJoint( Content::LooseAttachChain::JointEnd, selected ); 

    if(selected)
    {
      m_EndJointText->SetLabel( selected->GetName() ); 
    }
    else
    {
      m_EndJointText->SetLabel( "<no joint selected>" ); 
    }
  }

	void CharacterLooseAttachWizardDouble::OnButtonFinish( wxCommandEvent& event )
  {
    m_Chain->m_Name = m_NameText->GetLabelText(); 

    bool ok = m_Editor->GetManager()->AddLooseAttachChain( m_Chain ); 
    if(ok)
    {
      m_Editor->CloseWizard(this); 
    }

  }


}
