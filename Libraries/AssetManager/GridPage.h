#pragma once

#include "WizardPage.h"
#include "AssetManager.h"


// Forwards
class wxPropertyGridManager;
class wxPropertyGridEvent;
class wxPGProperty;

class DisplayManagedAssetVisitor;

namespace AssetManager
{
  // Forwards
  class GridPanel;
  class ManagedAssetFile;

  typedef std::map< int, wxPGProperty* > M_Property;

  //////////////////////////////////////
  class PropList
  {
  public:
    M_Property m_Property;

    PropList( int propertyCount )
    {
      for ( int propIndex = 0; propIndex < propertyCount; ++propIndex )
      {
        m_Property[propIndex] = NULL;
      }
    }
  };

  //////////////////////////////////////
  // this is a bad name and I'm sorry :( ~ rachel
  class ManagedAssetFilePathField
  {
  public:
    const std::string m_SuggestPath;
    bool m_NewPathEdited;
    bool m_ReferencePathEdited;

    ManagedAssetFilePathField( const std::string& suggestPath )
      : m_SuggestPath( suggestPath )
      , m_NewPathEdited( false )
      , m_ReferencePathEdited( false )
    {
    }
  };

  typedef std::multimap< ManagedAssetFile*, PropList > M_RenameToProp;
  typedef std::map< wxPGProperty*, ManagedAssetFile* > M_PropToRename;

  typedef std::map< ManagedAssetFile*, ManagedAssetFilePathField > M_ManagedAssetFilePathField;

  ///////////////////////////////////////////////////////////////////////////
  // First page of the wizard.  Contains information about the type of asset
  // being duplicated.  Allows the user to name the new asset and choose where
  // it will be saved.
  // 
  class GridPage : public WizardPage< GridPanel >
  {
  private:
    GridPage( Wizard* wizard );

  public:
    static UIToolKit::WizardPage* Create( Wizard* wizard );
    virtual ~GridPage();

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

    void SetPropertyLabels( const V_string& propertyLabels ){ m_PropertyLabels = propertyLabels; }

  protected:
    void OnPropertyGridChange( wxPropertyGridEvent& args );
    
    virtual void OnSelectAllButtonClick( wxCommandEvent& event );
    virtual void OnUnselectAllButtonClick( wxCommandEvent& event );

  private:
    void ConnectListeners();
    void DisconnectListeners();
    
    void SelectAllFiles( bool select = true );
    void UpdatePerformOperation( ManagedAssetFile* managedAssetFile, bool performOperation );

  public:
    friend class DisplayManagedAssetVisitor;

  private:

    ManagedAsset*                   m_ManagedAsset;

    wxPropertyGridManager*          m_PropGrid;

    M_PropToRename                  m_PropertyToManagedAssetFile;
    M_RenameToProp                  m_ManagedAssetFileToProperties;
    M_ManagedAssetFilePathField     m_ManagedAssetFilePathFields;

    std::string                     m_DlgTitle;

    V_string                        m_PropertyLabels;
  };
}
