#pragma once

#include "API.h"
#include "Wizard.h"

#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"

// Forwards
namespace Asset
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;
}

namespace AssetManager
{
  // Forwards
  class AttributesPage;
  class CreateAssetFileLocationPage;

  // Prototype for creator functions that generate a custom wizard page for an attribute
  typedef UIToolKit::WizardPage* (*CustomPageCreator)( Wizard* wizard, i32 attribTypeID );

  ///////////////////////////////////////////////////////////////////////////
  // Wizard for creating new Asset Classes in the Asset Editor.  Construct
  // an instance of this class with the type ID of the Asset Class that
  // you want to create.  Call RunWizard and pass in GetFirstPage to kick
  // things off.  If RunWizard returns true, you can call GetAssetClass to 
  // retrieve the asset that was made by the wizard.
  // NOTE: All pages of this wizard must extend UIToolKit::WizardPage.
  // 
  class ASSETMANAGER_API CreateAssetWizard : public Wizard
  {
  public:
    CreateAssetWizard( wxWindow *parent, i32 assetClassTypeID );
    virtual ~CreateAssetWizard();

    virtual void SetDefaultDirectory( const std::string& defaultDir );

    virtual const std::string&  GetNewFileLocation() const NOC_OVERRIDE;
    virtual void                SetNewFileLocation( const std::string& file ) NOC_OVERRIDE;

    AttributesPage*             GetAttributesPage() { return m_AttributesPage; }
    void                        SetAttributesPage( AttributesPage* attributesPage ) { m_AttributesPage = attributesPage; }

    const std::string&          GetSelectedAssetPath() const { return m_SelectedAssetPath; }
    void                        SetSelectedAssetPath( const std::string& selectedAssetPath ) { m_SelectedAssetPath = selectedAssetPath; }
    UIToolKit::WizardPage*      GetCustomAttributePage( i32 attributeTypeID );

    virtual void                SetCurrentTemplate( const Asset::AssetTemplate* assetTemplate ) NOC_OVERRIDE;
  private:
    void                        ResetAttributes();

  public:
    //
    // Wizard API that needs to be implemented by subclasses
    //

    virtual void                ProcessOperation() NOC_OVERRIDE;
    virtual void                PostProcessUpdate() NOC_OVERRIDE;

    virtual const std::string&  GetChangeDescription() NOC_OVERRIDE;

  private:
    typedef std::map< i32, UIToolKit::WizardPage* > M_Page;
    typedef std::map< i32, CustomPageCreator >      M_PageCreator;

  private:
    CreateAssetFileLocationPage* m_CreateAssetFileLocationPage;
    AttributesPage*           m_AttributesPage;
    std::string               m_SelectedAssetPath;

    M_Page                    m_CustomPages;
    M_PageCreator             m_CustomPageCreators;
  };

}