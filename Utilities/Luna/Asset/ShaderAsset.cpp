#include "Precompile.h"

#include "ShaderAsset.h"

#include "AssetDocument.h"
#include "AssetEditor.h"
#include "AssetManager.h"
#include "AssetReferenceNode.h"
#include "PersistentDataFactory.h"

#include "Pipeline/Asset/Classes/StandardShaderAsset.h"
#include "Pipeline/Asset/Components/TextureMapComponent.h"

#include "Foundation/String/Natural.h"
#include "Editor/Editor.h"
#include "Application/Inspect/Widgets/Button Controls/InspectButton.h"
#include "Application/Inspect/Widgets/Container.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::ShaderAsset );

///////////////////////////////////////////////////////////////////////////////
// Helper class used to pass information through the context menu callback so 
// that we know what kind of shader to change into.
// 
class ShaderConvertHelper : public Luna::Object
{
private:
    const Reflect::Class* m_ShaderType;
    Luna::ShaderAsset* m_Shader;
    Luna::AssetManager* m_AssetManager;

public:
    LUNA_DECLARE_TYPE( ShaderConvertHelper, Luna::Object );
    ShaderConvertHelper( const Reflect::Class* shaderType, Luna::ShaderAsset* shader, Luna::AssetManager* manager ) 
        : m_ShaderType( shaderType )
        , m_Shader( shader )
        , m_AssetManager( manager )
    {
    }

    virtual ~ShaderConvertHelper() 
    {
    }

    const Reflect::Class* GetShaderType() const 
    { 
        return m_ShaderType; 
    }

    Luna::ShaderAssetPtr GetShader() const
    {
        return m_Shader;
    }

    Luna::AssetManager* GetAssetManager() const
    {
        return m_AssetManager;
    }
};
LUNA_DEFINE_TYPE( ShaderConvertHelper );


///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AssetClass types.
// 
void ShaderAsset::InitializeType()
{
    Reflect::RegisterClass<Luna::ShaderAsset>( TXT( "Luna::ShaderAsset"  ) );
    Reflect::RegisterClass<ShaderConvertHelper>( TXT( "ShaderConvertHelper" ) );

    PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::ShaderAsset >(), &ShaderAsset::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AssetClass types.
// 
void ShaderAsset::CleanupType()
{
    Reflect::UnregisterClass<Luna::ShaderAsset>();
    Reflect::UnregisterClass<ShaderConvertHelper>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function for shader classes.
// 
Luna::PersistentDataPtr ShaderAsset::Create( Reflect::Element* shaderClass, Luna::AssetManager* manager )
{
    return new Luna::ShaderAsset( Reflect::AssertCast< Asset::ShaderAsset >( shaderClass ), manager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ShaderAsset::ShaderAsset( Asset::ShaderAsset* shaderClass, Luna::AssetManager* manager )
: Luna::AssetClass( shaderClass, manager )
{
    InitializeContextMenu();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShaderAsset::~ShaderAsset()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the icon to use based upon what type of shader this asset is.
// 
tstring ShaderAsset::GetIcon() const
{
    const i32 typeID = GetPackage< Asset::ShaderAsset >()->GetType();

    tstring icon( TXT( "enginetype_shader.png" ) );
    if ( typeID != Reflect::GetType< Asset::StandardShaderAsset >() )
    {
        icon = TXT( "enginetype_custom_shader.png" );
    }

    return icon;
}

///////////////////////////////////////////////////////////////////////////////
// Adds shader conversion sub-menu.
// 
void ShaderAsset::PopulateContextMenu( ContextMenuItemSet& menu )
{
    __super::PopulateContextMenu( menu );

    menu.AppendSeparator();
    SubMenuPtr subMenu = new SubMenu( TXT( "Convert Shader" ), TXT( "Convert this shader to a different type." ) );
    D_ContextMenuItemSmartPtr::const_iterator itr = m_CustomShaders.GetItems().begin();
    D_ContextMenuItemSmartPtr::const_iterator end = m_CustomShaders.GetItems().end();
    for ( ; itr != end; ++itr )
    {
        subMenu->AppendItem( *itr );
    }
    menu.AppendItem( subMenu );

    // Append option to reload all textures
    menu.AppendSeparator();
    ContextMenuItemPtr menuItem = new ContextMenuItem( TXT( "Reload all textures" ) );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &ShaderAsset::ReloadAllTextures ) );

    menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Stores a cloned copy of the original persistent data so that changes can
// be compared during real-time updates.
// 
void ShaderAsset::Unpack()
{
    __super::Unpack();

    m_OldShaderClass = Reflect::ObjectCast< Asset::ShaderAsset >( GetPackage< Asset::ShaderAsset >()->Clone() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a control in the property panel has changed.  Handles
// real-time update of the shader.  
// 
void ShaderAsset::Changed( Inspect::Control* control )
{
    __super::Changed( control );

    Asset::ShaderAssetPtr shaderClass = GetPackage< Asset::ShaderAsset >();

    Nocturnal::Path texturePath;

    if ( control && control->HasType( Reflect::GetType<Inspect::Button>() ) )
    {
        Inspect::StringData* data = Inspect::CastData< Inspect::StringData, Inspect::DataTypes::String >( control->GetData() );

        tstring str;
        data->Get( str );

        texturePath.Set( str );
        shaderClass->SetTextureDirty( texturePath, true );
    }

    CheckShaderChanged( texturePath );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to check if a shader changed and signal the concerned parties
// 
void ShaderAsset::CheckShaderChanged( Nocturnal::Path texturePath )
{
    Asset::ShaderAssetPtr shaderClass = GetPackage< Asset::ShaderAsset >();

    if ( m_OldShaderClass.ReferencesObject() )
    {
        ShaderChangedArgs args;
        args.m_ShaderClass = shaderClass;
        args.m_OldShaderClass = m_OldShaderClass;
        args.m_TexturePath = texturePath;
        m_ShaderChanged.Raise( args );

        m_OldShaderClass = Reflect::ObjectCast< Asset::ShaderAsset >( shaderClass->Clone() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to prepare the context menu items for converting shaders.
// 
void ShaderAsset::InitializeContextMenu()
{
    // Build a list of context menu items for each type of shader that exists in
    // the system.
    typedef std::map< tstring, ContextMenuItemPtr, CaseInsensitiveNatStrCmp > M_Ordered;
    M_Ordered items;
    ContextMenuItemPtr addStandardShader;
    const Reflect::Class* classInfo = Reflect::GetClass< Asset::ShaderAsset >();
    const std::set<tstring>& typeSet = classInfo->m_Derived;
    std::set<tstring>::const_iterator typeItr = typeSet.begin();
    std::set<tstring>::const_iterator typeEnd = typeSet.end();
    for ( ; typeItr != typeEnd; ++typeItr )
    {
        const Reflect::Class* derived = Reflect::Registry::GetInstance()->GetClass( *typeItr );
        ContextMenuItemPtr menuItem = new ContextMenuItem( derived->m_UIName );
        menuItem->AddCallback( ContextMenuSignature::Delegate ( &ShaderAsset::ConvertShader ), new ShaderConvertHelper( derived, this, m_AssetManager ) );

        // If this derived class is not the same type as us, add it to the list.
        if ( derived->m_TypeID != GetPackage< Asset::ShaderAsset >()->GetType() )
        {
            if ( derived->m_TypeID == Reflect::GetType< Asset::StandardShaderAsset >() )
            {
                addStandardShader = menuItem;
            }
            else
            {
                items.insert( M_Ordered::value_type( derived->m_UIName, menuItem ) );
            }
        }
    }

    // Now that we have the menu items in the proper order, build the context menu
    if ( addStandardShader.ReferencesObject() )
    {
        m_CustomShaders.AppendItem( addStandardShader );
        if ( items.size() > 0 )
        {
            m_CustomShaders.AppendSeparator();
        }
    }
    M_Ordered::const_iterator itemItr = items.begin();
    M_Ordered::const_iterator itemEnd = items.end();
    for ( ; itemItr != itemEnd; ++itemItr )
    {
        m_CustomShaders.AppendItem( itemItr->second );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for converting this shader to a different type.
// 
void ShaderAsset::ConvertShader( const ContextMenuArgsPtr& args )
{
    ShaderConvertHelper* changeArgs = Reflect::ObjectCast< ShaderConvertHelper >( args->GetClientData() );
    if ( changeArgs )
    {
        Luna::ShaderAsset* changeShader = changeArgs->GetShader();
        Luna::AssetManager* assetManager = changeShader->GetAssetManager();

        if ( assetManager->IsEditable( changeShader ) )
        {
            AssetDocument* doc = assetManager->FindAssetDocument( changeShader );
            if ( !assetManager->IsCheckedOut( doc ) )
            {
                tstring msg = TXT( "You must check out '" ) + doc->GetFileName() + TXT( "' in order to convert it to another shader type." );
                wxMessageBox( msg.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, assetManager->GetAssetEditor() );
                return;
            }

            // Clear the selection since we are about to close the selected shader.
            assetManager->GetSelection().Clear();

            const Reflect::Class* convertTo = changeArgs->GetShaderType();

            changeShader->Pack();
            Asset::ShaderAsset* pkg = changeShader->GetPackage< Asset::ShaderAsset >();

            if ( convertTo->m_TypeID != pkg->GetType() )
            {
                Asset::ShaderAssetPtr newPkg = Reflect::ObjectCast< Asset::ShaderAsset >( Reflect::Registry::GetInstance()->CreateInstance( convertTo ) );
                if ( newPkg.ReferencesObject() )
                {
                    // Copy the persistent data
                    pkg->CopyTo( newPkg );
                    newPkg->SetPath( pkg->GetPath() );
                    tstring filePath = pkg->GetFilePath();

                    try
                    {
                        newPkg->Serialize();
                    }
                    catch ( const Nocturnal::Exception& e )
                    {
                        tstring msg = TXT( "Failed to convert shader: " ) + e.Get();
                        wxMessageBox( msg.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, assetManager->GetAssetEditor() );
                        return;
                    }

                    newPkg = NULL;

                    // Close the old shader
                    tstring error;
                    assetManager->CloseDocument( doc, false );

                    if ( !assetManager->Open( filePath, error, true ) )
                    {
                        wxMessageBox( error.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, assetManager->GetAssetEditor() );
                        return;
                    }
                }
                else
                {
                    // You are not allowed to convert to an object that is not derived from ShaderAsset.
                    NOC_BREAK(); 
                }
            }
        }
    }
    else
    {
        // This callback did not have all the required information (should never happen!)
        NOC_BREAK();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for reloading all of the textures in a shader
// 
void ShaderAsset::ReloadAllTextures( const ContextMenuArgsPtr& args )
{
    Asset::ShaderAssetPtr shaderClass = GetPackage< Asset::ShaderAsset >();
    Component::M_Component::const_iterator itr = shaderClass->GetComponents().begin();
    Component::M_Component::const_iterator end = shaderClass->GetComponents().end(); 

    std::set< Nocturnal::Path > textureFiles;

    while( itr != end )
    {
        // try to cast this item to a texture map attribute
        Asset::TextureMapComponent *textureAttr = Reflect::ObjectCast< Asset::TextureMapComponent >( (*itr).second );

        if( textureAttr )
        {
            textureFiles.insert( textureAttr->GetPath() );
        }    
        itr++;
    }

    for( std::set< Nocturnal::Path >::const_iterator itr = textureFiles.begin(), end = textureFiles.end(); itr != end; ++itr )
    {
        shaderClass->SetTextureDirty( *itr, true );
        CheckShaderChanged( (*itr) );
    }

    return;
}