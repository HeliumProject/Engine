#pragma once

#include "WizardPanels.h"

#include "Common/Types.h"
#include "Common/Compiler.h"
#include "Common/Memory/SmartPtr.h"

//Forwards
namespace Attribute
{
  class AttributeBase;
  typedef Nocturnal::SmartPtr< AttributeBase > AttributePtr;
}

namespace AssetManager
{
  ///////////////////////////////////////////////////////////////////////////////
  // Class TextureMapPanel

  class TextureMapFieldPanel : public TextureMapPanel 
  {
  public:
    TextureMapFieldPanel( wxWindow* parent, Attribute::AttributeBase* attribute, bool isRequired = false );
    virtual ~TextureMapFieldPanel()
    {
    }

    const std::string& GetFilePath() const { return m_FilePath; }
    void SetFilePath( const std::string& filePath );

    bool IsRequired() const { return m_IsRequired; }
    void IsRequired( bool isRequired );

    bool ValidateFilePath( std::string& error ) const;

  protected:
    // Virtual event handlers, overide them in your derived class
    virtual void OnFieldTextEnter( wxCommandEvent& event ) NOC_OVERRIDE;
    virtual void OnFileBrowserButtonClick( wxCommandEvent& event )  NOC_OVERRIDE;
    virtual void OnExplorerButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;

  private:
    void SetFieldLabel();

  private:
    Attribute::AttributePtr m_Attribute;
    std::string             m_FilePath;
    bool                    m_IsRequired;
  };
} // namespace AssetManager