#pragma once

#include "WizardPage.h"
#include "CreateAssetWizard.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "Attribute/AttributeCollection.h"

// Forwards
namespace Asset
{
  class AssetTemplate;
}

namespace Reflect
{
  class Class;
}

namespace AssetManager
{

  // Forwards
  class CreateAssetWizard;
  class FinishPage;
  class AttribsPanel;

  ///////////////////////////////////////////////////////////////////////////
  // Helper class for sorting a set of attributes alphabetically, with the 
  // required attributes appearing before the optional ones.
  // 
  class AttributeSort
  {
  private:
    const Asset::AssetTemplate* m_CurrentTemplate;

  public:
    AttributeSort( const Asset::AssetTemplate* assetTemplate );
    bool operator()( Attribute::AttributeBase* first, Attribute::AttributeBase* second ) const;
  };

  typedef std::set< Attribute::AttributeBase*, AttributeSort > S_SortedAttribute;

  ///////////////////////////////////////////////////////////////////////////
  // This page of the wizard displays a list of attributes that the user can
  // toggle on and off.  Some attributes are required and cannot be toggled 
  // off.  A description is displayed for each attribute as the mouse passes
  // over it.  This page also sets up all the custom wizard pages that may
  // follow.
  // 
  class AttributesPage : public WizardPage< AttribsPanel >
  {
  protected:
    AttributesPage( CreateAssetWizard* wizard );

  public:
    virtual ~AttributesPage();

    static UIToolKit::WizardPage* Create( CreateAssetWizard* wizard );

    const S_SortedAttribute& GetRequiredAttributes() { return m_RequiredAttributes; }

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

    virtual void Finished() NOC_OVERRIDE;

    // Returns properly typed wizard object.
    CreateAssetWizard* GetCreateAssetWizard() const
    {
      return wxStaticCast( __super::GetWizard(), CreateAssetWizard );
    }

  protected:
    void ConnectListeners();
    void DisconnectListeners();

  private:
    // UI callbacks
    void OnMouseMove( wxMouseEvent& args );

  private:
    void SortAttributes( const Attribute::M_Attribute& original, S_SortedAttribute& sorted );
    void AddCheckboxes( const S_SortedAttribute& attribs, wxWindow* parent, bool enabled );
    void SetAttribDescription( const Reflect::Class* attribClass );
    void ChainPages();

  private:
    typedef std::map< wxCheckBox*, Attribute::AttributeBase* > M_CheckboxAttribute;

    M_CheckboxAttribute       m_CheckboxToAttrib;
    S_SortedAttribute         m_RequiredAttributes;
    i32                       m_CurrentDescription;
    UIToolKit::WizardPage*    m_NextPage;
  };
}
