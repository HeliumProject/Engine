#include "stdafx.h"

#include "MayaShaderManager.h"
#include "ShaderBrowser.h"

#include "Asset/Exceptions.h"
#include "Asset/StandardColorMapAttribute.h"
#include "Asset/StandardExpensiveMapAttribute.h"
#include "Asset/StandardNormalMapAttribute.h"
#include "Asset/StandardShaderAsset.h"
#include "Attribute/AttributeHandle.h"
#include "File/File.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Finder/Finder.h"
#include "Finder/ShaderSpecs.h"
#include "MayaUtils/Utils.h"
#include "RCS/rcs.h"
#include "Texture/Texture.h"
#include "TUID/TUID.h"


using namespace Asset;
using namespace Attribute;

using namespace Maya;

//
// Command flags
//
const char* BrowseFlag = "-b";
const char* BrowseFlagLong = "-browse";

const char* CleanupShadersFlag = "-cs";
const char* CleanupShadersFlagLong = "-cleanupShaders";

const char* ColorMapFlag = "-cm";
const char* ColorMapFlagLong = "-colorMap";

const char* CreateShaderAssetFlag = "-csa";
const char* CreateShaderAssetFlagLong = "-createShaderAsset";

const char* ExpensiveMapFlag = "-em";
const char* ExpensiveMapFlagLong = "-expensiveMap";

const char* GetShaderNamesFlag = "-sn";
const char* GetShaderNamesFlagLong = "-shaderNames";

const char* MayaSurfaceShadersFlag = "-mss";
const char* MayaSurfaceShadersFlagLong = "-mayaSurfaceShaders";

const char* UniqueShadersFlag = "-us";
const char* UniqueShadersFlagLong = "-uniqueShaders";

const char* LoadBlinnShaderFlag = "-lbs";
const char* LoadBlinnShaderFlagLong = "-loadBlinnShader";

const char* LoadLambertShaderFlag = "-lls";
const char* LoadLambertShaderFlagLong = "-loadLambertShader";

const char* NormalMapFlag = "-nm";
const char* NormalMapFlagLong = "-normalMap";

const char* QueryFlag = "-q";
const char* QueryFlagLong = "-query";

const char* ShaderFilenameFlag = "-sf";
const char* ShaderFilenameFlagLong = "-shaderFilename";

const char* SyncShadersFlag = "-ss";
const char* SyncShadersFlagLong = "-syncShaders";

const char* UpdateFlag = "-up";
const char* UpdateFlagLong = "-update";


//
// Usage Message
//
const static char *g_UsageMessage =
"\n \
Usage: shaderManager [[-lls|-loadLambertShader]|[-lbs|-loadBlinnShader] <path>] \n \
[-q] [-si|shaderId <node>] [-sn|shaderNames [pattern]] [-up <node>] [-browse]\n \
[-csa|createShaderAsset <shader name> -cm|colorMap <color map> [-nm|-normalMap <normal map>] [-em|-expensiveMap <expensive map>]] \n \
\n \
Flags: \n \
\t -lls, loadLambertShader  -- Given a path to a shader's .rb file, load it into maya as \n \
\t                             a lambert shader. \n \
\t -lbs, loadBlinnShader    -- Given a path to a shader's .rb file, load it into maya as \n \
\t                             a blinn shader. \n \
\n \
\t -si, shaderId            -- Given a lambert node, get the shader id it's based on. \n \
\t -sn, shaderNames         -- Get a list of available shaders, if pattern is specified, \n \
\t                             filter the list by that pattern where * is a wildcard. \n \
\t -uid,uniqueIDs           -- Get the list of all unique shaders IDs. \n \
\t -mss,mayaSurfaceShaders  -- Get the list of all shaders with the give a shader asset ID. \n \
\n \
\t -csa, createShaderAsset  -- Creates a new shader asset, returning its asset filename, \n \
\t                             takes a shader name and needs the color map argument. \n \
\t -csw, createShaderWizard -- Opens the wizard UI for creating a new shader. \n \
\t -cm, colorMap            -- Specify the color map filename. \n \
\t -cs, cleanupShaders      -- Remove duplicate and unused shaders. \n \
\t -nm, normalMap           -- Specify the normal map filename. \n \
\t -em, expensiveMap        -- Specify the expensive map filename. \n \
\n \
\t -q, query                -- Query information about a given shader. \n \
\t -b, browse               -- Bring up the shader browser to import shaders. \n \
\n \
\t -up, update              -- Update shader texture paths. \n \
\t -sync,syncShaders        -- Sync and reload all shaders and textures. \n \
";


//
// Node and attribute names
//

static const char* s_TextureFileAttrName = "fileTextureName";
static const char* s_SurfaceShaderAttrName = "surfaceShader";

static const char* s_ShaderAssetIdAttrName = "shaderAssetId";
static const char* s_ShaderFilenameAttrName = "shaderFilename";
static const char* s_TopShaderAssetIdAttrName = "topShaderAssetId";

// Remove legacy deconstructed TUID support
static const char* s_LegacyShaderIdAttrName = "shaderId";
static const char* s_LegacyShaderNameAttrName = "shaderName";
static const char* s_LegacyTopShaderIdAttrName = "topShaderId";

//
// Globals
//

std::map< u64, bool > g_HasAlphaCache;
std::map< u64, u64 > g_CacheUpdateTimes;

ShaderBrowserPtr g_ShaderBrowserDlg = NULL;

ShaderManager::ShaderManager()
{
}

void *ShaderManager::creator()
{
    return new ShaderManager();
}

MSyntax ShaderManager::commandSyntax()
{
    MSyntax syntax;

    syntax.addFlag( LoadLambertShaderFlag, LoadLambertShaderFlagLong, MSyntax::kString );
    syntax.addFlag( LoadBlinnShaderFlag, LoadBlinnShaderFlagLong, MSyntax::kString );
    syntax.addFlag( ShaderFilenameFlag, ShaderFilenameFlagLong, MSyntax::kString );
    syntax.addFlag( GetShaderNamesFlag, GetShaderNamesFlagLong, MSyntax::kString );
    syntax.addFlag( MayaSurfaceShadersFlag, MayaSurfaceShadersFlagLong, MSyntax::kString );
    syntax.addFlag( UniqueShadersFlag, UniqueShadersFlagLong );
    syntax.addFlag( CleanupShadersFlag, CleanupShadersFlagLong );
    syntax.addFlag( ColorMapFlag, ColorMapFlagLong, MSyntax::kString );
    syntax.addFlag( NormalMapFlag, NormalMapFlagLong, MSyntax::kString );
    syntax.addFlag( ExpensiveMapFlag, ExpensiveMapFlagLong, MSyntax::kString );
    syntax.addFlag( QueryFlag, QueryFlagLong );
    syntax.addFlag( UpdateFlag, UpdateFlagLong );
    syntax.addFlag( SyncShadersFlag, SyncShadersFlagLong );
    syntax.addFlag( BrowseFlag, BrowseFlagLong );

    syntax.setObjectType(MSyntax::kSelectionList, 0, 1);
    /// if no objects are specified, then tell the command to use the selected objects.
    //syntax.useSelectionAsDefault(true);


    return syntax;
}

void ShaderManager::DisplayUsage(void) const
{
    MPxCommand::displayError( MString( g_UsageMessage ) );
}

MStatus ShaderManager::doIt( const MArgList &args )
{
    MStatus status = MS::kSuccess;

    // Parse the arguments
    MArgDatabase argDatabase( syntax(), args, &status );

    if ( !status )
    {
        DisplayUsage();
        return status;
    }

    //---------------------------------------------------------------------------
    if ( argDatabase.isFlagSet( LoadLambertShaderFlag ) && argDatabase.isFlagSet( LoadBlinnShaderFlag ) )
    {
        MPxCommand::displayError( MString( "Cannot create both lambert and blinn shaders simultaneously!" ) );
        return MStatus::kFailure;
    }

    //---------------------------------------------------------------------------
    if ( argDatabase.isFlagSet( LoadLambertShaderFlag ) || argDatabase.isFlagSet( LoadBlinnShaderFlag ) )
    {
        MString shaderPathMayaString;
        if ( argDatabase.isFlagSet( LoadLambertShaderFlag ) )
        {
            argDatabase.getFlagArgument( LoadLambertShaderFlag, 0, shaderPathMayaString );
        }
        else if ( argDatabase.isFlagSet( LoadBlinnShaderFlag ) )
        {
            argDatabase.getFlagArgument( LoadBlinnShaderFlag, 0, shaderPathMayaString );
        }

        std::string shaderPath = shaderPathMayaString.asChar();
        File::Reference shaderRef( shaderPath );
        shaderRef.Resolve();

        ShaderType shaderType;
        if ( argDatabase.isFlagSet( LoadLambertShaderFlag ) )
        {
            shaderType = ShaderTypes::Lambert;
        }
        else if ( argDatabase.isFlagSet( LoadBlinnShaderFlag ) )
        {
            shaderType = ShaderTypes::Blinn;
        }
        else
        {
            throw Nocturnal::Exception( "Unknown shader type, this shouldn't be possible." );
        }

        LoadShader( shaderType, shaderRef.GetPath() );
    }

    //---------------------------------------------------------------------------
    if ( argDatabase.isFlagSet( ShaderFilenameFlag ) )
    {
        MString lambertNodeName;
        argDatabase.getFlagArgument( ShaderFilenameFlag, 0, lambertNodeName );

        MSelectionList selectionList;
        MGlobal::getSelectionListByName( lambertNodeName, selectionList );

        if ( selectionList.length() != 1 )
        {
            MPxCommand::displayError( MString( "Could not locate the given lambert node!" ) );
            return MStatus::kFailure;
        }

        MObject object;
        selectionList.getDependNode( 0, object );

        clearResult();
        MStringArray result;
        std::string shaderFilename = GetShader( object );
        result.append( shaderFilename.c_str() );
        setResult( result );
    }

    //---------------------------------------------------------------------------
    if ( !argDatabase.isFlagSet( QueryFlag ) && argDatabase.isFlagSet( GetShaderNamesFlag ) )
    {
        MString mayaPatternString;
        argDatabase.getFlagArgument( GetShaderNamesFlag, 0, mayaPatternString );

        V_string shaderNames;
        GetShaders( mayaPatternString.asChar(), shaderNames );

        clearResult();
        MStringArray result;

        for( V_string::iterator it = shaderNames.begin(); it != shaderNames.end(); ++it )
        {
            result.append( (*it).c_str() );
        }

        setResult( result );
    }

    //---------------------------------------------------------------------------
    if ( !argDatabase.isFlagSet( QueryFlag ) && argDatabase.isFlagSet( MayaSurfaceShadersFlag ) )
    {
        MString shaderIDMString;
        argDatabase.getFlagArgument( MayaSurfaceShadersFlag, 0, shaderIDMString );

        tuid shaderID = TUID::Null;

        std::string shaderIDStr = shaderIDMString.asChar();

        std::istringstream idStream ( shaderIDStr );
        idStream >> std::hex >> shaderID;

        if ( shaderID != TUID::Null )
        {
            MStringArray shaderNames;
            GetMayaSurfaceShaderNames( shaderID, shaderNames );

            setResult( shaderNames );
        }
    }

    //---------------------------------------------------------------------------
    if ( !argDatabase.isFlagSet( QueryFlag ) && argDatabase.isFlagSet( UniqueShadersFlag ) )
    {
        MStringArray shaders;

        GetUniqueShaders( shaders );

        setResult( shaders );
    }

    //---------------------------------------------------------------------------
    if ( argDatabase.isFlagSet( CleanupShadersFlag ) )
    {
        CleanupShaders();
    }

    //---------------------------------------------------------------------------
    if ( argDatabase.isFlagSet( CreateShaderAssetFlag ) )
    {
        MString mayaShaderName = "";
        MString mayaColorMapFilename = "";
        MString mayaNormalMapFilename = "";
        MString mayaExpensiveMapFilename = "";

        argDatabase.getFlagArgument( CreateShaderAssetFlag, 0, mayaShaderName );

        std::string shaderFilename = "";

        // we fail if they didn't set this, returning an empty string
        if ( argDatabase.isFlagSet( ColorMapFlag ) )
        {
            argDatabase.getFlagArgument( ColorMapFlag, 0, mayaColorMapFilename );
            if ( argDatabase.isFlagSet( NormalMapFlag ) )
            {
                argDatabase.getFlagArgument( NormalMapFlag, 0, mayaNormalMapFilename );
            }

            if ( argDatabase.isFlagSet( ExpensiveMapFlag ) )
            {
                argDatabase.getFlagArgument( ExpensiveMapFlag, 0, mayaExpensiveMapFilename );
            }

            std::string colorMap = mayaColorMapFilename.asChar();
            std::string normalMap = mayaNormalMapFilename.asChar();
            std::string expensiveMap = mayaExpensiveMapFilename.asChar();

            try
            {
                FileSystem::CleanName( colorMap );
                if ( !normalMap.empty() )
                {
                    FileSystem::CleanName( normalMap );
                }
                if ( !expensiveMap.empty() )
                {
                    FileSystem::CleanName( expensiveMap );
                }

                CreateShader( shaderFilename, mayaShaderName.asChar(), mayaColorMapFilename.asChar(), mayaNormalMapFilename.asChar(), mayaExpensiveMapFilename.asChar() );
            }
            catch( Nocturnal::Exception &e )
            {
                if ( MGlobal::mayaState() == MGlobal::kInteractive )
                {
                    ::MessageBox( M3dView::applicationShell(), e.what(), "Error Creating Shader", MB_ICONERROR );
                }
                else
                {
                    std::cerr << e.what() << std::endl;
                }
                shaderFilename = "";
            }
        }

        setResult( shaderFilename.c_str() );
    }

    //---------------------------------------------------------------------------
    // WARNING: hax below -- to support thumbnail of currently selected shader in the shader browser.
    if( argDatabase.isFlagSet( QueryFlag ) && argDatabase.isFlagSet( GetShaderNamesFlag ) )
    {
        MString shaderName;
        argDatabase.getFlagArgument( GetShaderNamesFlag, 0, shaderName );

        if( argDatabase.isFlagSet( ColorMapFlag ) )
        {
            try
            {
                File::Reference shaderRef( shaderName.asChar() );
                shaderRef.Resolve();

                ShaderAssetPtr shader = Asset::AssetClass::LoadAssetClass<ShaderAsset>( shaderRef.GetPath() );

                AttributeViewer< StandardColorMapAttribute > colorMap( shader );
                if( colorMap.Valid() )
                {
                    Nocturnal::File colorMapFile = colorMap->GetFileReference().GetFile();

                    if( !colorMapFile.Exists() )
                    {
                        RCS::File rcsFile( colorMapFile.GetPath().Get() );
                        rcsFile.GetInfo();
                        if( rcsFile.ExistsInDepot() && !rcsFile.HeadDeleted() )
                        {
                            rcsFile.Sync();
                        }
                    }

                    // now we should be synced if we didn't have it before
                    if ( colorMapFile.Exists() )
                    {
                        MImage image;
                        image.readFromFile( colorMapFile.GetPath().c_str() );
                        image.resize( 100, 100, false );

                        static std::string thumbnailPath = Finder::ProjectRoot() + "thumbnail.iff";

                        image.writeToFile( thumbnailPath.c_str() );
                        MString result( thumbnailPath.c_str() );
                        setResult( result );
                    }
                    return MS::kSuccess;
                }
            }
            catch ( Nocturnal::Exception& e)
            {
                MGlobal::displayError( MString("Unable to show thumbnail, ") + e.what() );
            }
        }
    }

    //---------------------------------------------------------------------------
    if ( argDatabase.isFlagSet( SyncShadersFlag ) )
    {
        UpdateShaders();

        return MS::kSuccess;
    }

    //---------------------------------------------------------------------------
    if( argDatabase.isFlagSet( UpdateFlag ) )
    {
        MSelectionList list;
        argDatabase.getObjects( list );

        if ( list.length() != 1 )
        {
            MPxCommand::displayError( MString( "No shaders passed in to command!" ) );
            return MStatus::kFailure;
        }

        MObject shader;
        list.getDependNode( 0, shader );
        UpdateTexturePaths( shader );

        return MS::kSuccess;
    }

    //---------------------------------------------------------------------------
    if( argDatabase.isFlagSet( BrowseFlag ) )
    {
        return ShowShaderBrowser();
    }

    return status;
}

void Maya::CreateShader( std::string& shaderFilename, const std::string& shaderName, const std::string& colorMapPath, const std::string& normalMapPath, const std::string& expensiveMapPath )
{
    shaderFilename = FinderSpecs::Shader::DATA_FOLDER.GetFolder() + shaderName + "/" + shaderName;
    FinderSpecs::Asset::SHADER_DECORATION.Modify( shaderFilename );

    File::Reference shaderRef( shaderFilename );
    shaderRef.Resolve();

    // fail if this shader already exists
    if ( shaderRef.GetFile().Exists() )
    {
        if ( MGlobal::mayaState() == MGlobal::kInteractive )
        {
            ::MessageBox( M3dView::applicationShell(), ( std::string( "A shader named \"" ) + shaderName + "\" already exists!" ).c_str(), "Error Creating Shader", MB_ICONERROR );
        }
        else
        {
            std::cerr << "A shader named \"" << shaderName << "\" already exists!" << std::endl;
        }
        shaderFilename = "";
        return;
    }

    // fail if they don't specify a color map
    if ( colorMapPath.empty() )
    {
        if ( MGlobal::mayaState() == MGlobal::kInteractive )
        {
            ::MessageBox( M3dView::applicationShell(), "Cannot create a shader without a color map!", "Error Creating Shader", MB_ICONERROR );
        }
        else
        {
            std::cerr << "Cannot create a shader without a color map!" << std::endl;
        }

        shaderFilename = "";
        return;
    }

    FileSystem::MakePath( shaderFilename, true );

    StandardShaderAssetPtr shader = new StandardShaderAsset();

    AttributeEditor< StandardColorMapAttribute > colorMap( shader );
    File::Reference colorMapRef( colorMapPath );
    colorMapRef.Resolve();
    colorMap->SetFileReference( colorMapRef );

    std::string newNormalMapPath = normalMapPath;
    size_t typeOffset = colorMapPath.find( "_c." ); // FinderSpecs::Shader::NORMAL_MAP_SUFFIX.Modify( standardNormalMapPath );
    if ( typeOffset != std::string::npos )
    {
        std::string standardNormalMapPath;
        standardNormalMapPath = colorMapPath;
        standardNormalMapPath.replace( typeOffset, 3, "_n." ); // FinderSpecs::Shader::NORMAL_MAP_SUFFIX.Modify( standardNormalMapPath );
        if ( FileSystem::Exists( standardNormalMapPath ) && newNormalMapPath.empty() )
        {
            newNormalMapPath = standardNormalMapPath;
        }
    }

    if ( !newNormalMapPath.empty() )
    {
        AttributeEditor< StandardNormalMapAttribute > normalMap( shader );
        File::Reference normalMapRef( normalMapPath );
        normalMapRef.Resolve();
        normalMap->SetFileReference( normalMapPath );
    }

    std::string newExpensiveMapPath = expensiveMapPath;
    if ( typeOffset != std::string::npos )
    {
        std::string standardExpensiveMapPath;
        standardExpensiveMapPath = colorMapPath;
        standardExpensiveMapPath.replace( typeOffset, 3, "_e." ); // FinderSpecs::Shader::EXPENSIVE_MAP_SUFFIX.Modify( standardExpensiveMapPath );
        if ( FileSystem::Exists( standardExpensiveMapPath ) && newExpensiveMapPath.empty() )
        {
            newExpensiveMapPath = standardExpensiveMapPath;
        }
    }

    if ( !newExpensiveMapPath.empty() )
    {
        AttributeEditor< StandardExpensiveMapAttribute > expensiveMap( shader );
        File::Reference newExpensiveMapRef( newExpensiveMapPath );
        newExpensiveMapRef.Resolve();
        expensiveMap->SetFileReference( newExpensiveMapRef );
    }

    shader->SetAssetFileRef( shaderRef );
    shader->Serialize();
}

void Maya::LoadShader( const ShaderType shaderType, const std::string& shaderFile )
{
    MStatus status;
    MPlug plug;

    // get our shader
    ShaderAssetPtr shader = Asset::AssetClass::LoadAssetClass<ShaderAsset>( shaderFile );
    NOC_ASSERT( shader.ReferencesObject() );

    // create the 'shading group'
    std::string shadingEngineSetName = ( ( shaderType == ShaderTypes::Lambert ) ? std::string( "lambert" ) : std::string( "blinn" ) ) + std::string( "ShadingEngine_" ) + shader->GetShortName();
    MSelectionList emptyList;

    MFnSet setFn;
    MObject shadingEngineSet = setFn.create( emptyList, MFnSet::kRenderableOnly, &status );
    setFn.setName( shadingEngineSetName.c_str() );

    // FIXME only create this node if we don't already have a node that
    // refers to this file
    // create the texture node
    std::string fileNodeName = shader->GetShortName() + FinderSpecs::Shader::TEXTURE_NODE_SUFFIX.GetSuffix(); //"_tex";

    MFnDependencyNode nodeFn;
    MObject colorMapNode = nodeFn.create( "file", fileNodeName.c_str() );

    // create the shader node
    MObject shaderNode;
    if ( shaderType == ShaderTypes::Blinn )
    {
        std::string blinnNodeName = shader->GetShortName() + FinderSpecs::Shader::BLINN_SUFFIX.GetSuffix(); //"_bli";

        MFnBlinnShader blinnFn;
        shaderNode = blinnFn.create( );
        blinnFn.setName( blinnNodeName.c_str() );
    }
    else if ( shaderType == ShaderTypes::Lambert )
    {
        std::string lambertNodeName = shader->GetShortName() + FinderSpecs::Shader::LAMBERT_SUFFIX.GetSuffix(); //"_lam";

        MFnLambertShader lambertFn;
        shaderNode = lambertFn.create();
        lambertFn.setName( lambertNodeName.c_str() );
    }

    // find and disconnect any stupid shading connections maya automatically makes
    MFnDependencyNode shadingEngineSetFn( shadingEngineSet );
    MPlug shadingSetPlug( shadingEngineSetFn.findPlug( shadingEngineSetFn.attribute( s_SurfaceShaderAttrName ) ) );
    MPlugArray shadingSetConnections;
    shadingSetPlug.connectedTo( shadingSetConnections, true, false );

    MDGModifier dgModifier;
    for( u32 i = 0; i < shadingSetConnections.length(); ++i )
    {
        dgModifier.disconnect( shadingSetConnections[i], shadingSetPlug );
    }

    // make our real shading set connection
    MFnDependencyNode shaderNodeFn( shaderNode );
    status = dgModifier.connect( shaderNode, shaderNodeFn.attribute( "outColor" ), shadingEngineSet, shadingEngineSetFn.attribute( s_SurfaceShaderAttrName ) );

    SetShader( shaderNode, shaderFile );

    // hook the texture node up to the lambert node
    MObject textureFilenameAttr;
    MFnTypedAttribute tAttr;

    MFnDependencyNode colorMapNodeFn( colorMapNode );
    status = dgModifier.connect( colorMapNode, colorMapNodeFn.attribute( "outColor" ), shaderNode, shaderNodeFn.attribute( "color" ) );

    // hook the texture node up with its backing file
    textureFilenameAttr = tAttr.create( s_TextureFileAttrName, s_TextureFileAttrName, MFnData::kString );
    plug = colorMapNodeFn.findPlug( s_TextureFileAttrName );

    AttributeViewer< StandardColorMapAttribute > colorMap( shader );
    plug.setValue( colorMap->GetFileReference().GetPath().c_str() );

    // if the texture has alpha, hook that up
    if ( shader->m_AlphaMode != AlphaTypes::ALPHA_OPAQUE && HasAlpha( shaderFile ) )
    {
        dgModifier.connect( colorMapNode, colorMapNodeFn.attribute( "outTransparency" ), shaderNode, shaderNodeFn.attribute( "transparency" ) );
    }

    // hook our stuff up to the 2d texture view
    MSelectionList selList;
    MGlobal::getSelectionListByName( "defaultTextureList1", selList );

    if ( selList.length() != 1 )
    {
        MGlobal::displayError( "Could not locate default texture list to display shader in 2d texture view!" );
    }
    else
    {
        MObject textureListObject;
        selList.getDependNode( 0, textureListObject );
        MFnDependencyNode textureListNode( textureListObject );
        dgModifier.connect( colorMapNode, colorMapNodeFn.attribute( "message" ), textureListObject, textureListNode.attribute( "textures" ) );
    }

    // commit our dg changes
    dgModifier.doIt();

    // if we have a normal map
    AttributeViewer< StandardNormalMapAttribute > normalMap( shader );

    if ( shaderType == ShaderTypes::Blinn && ( normalMap.Valid() && !normalMap->GetFileReference().GetPath().empty() ) )
    {
        std::string normalMapNodeName = shader->GetShortName() + FinderSpecs::Shader::NORMAL_MAP_NODE_SUFFIX.GetSuffix(); //"_nrml";
        MObject normalMapNode = nodeFn.create( "file", normalMapNodeName.c_str() );
        MFnDependencyNode normalMapNodeFn( normalMapNode );

        textureFilenameAttr = tAttr.create( s_TextureFileAttrName, s_TextureFileAttrName, MFnData::kString );
        plug = normalMapNodeFn.findPlug( s_TextureFileAttrName );
        plug.setValue( normalMap->GetFileReference().GetPath().c_str() );

        dgModifier.connect( normalMapNode, normalMapNodeFn.attribute( "outColor" ), shaderNode, shaderNodeFn.attribute( "normalCamera" ) );
        dgModifier.doIt();

        Setup2dTextureNode( normalMapNode );
    }

    // set the shadow attenuation... ?
    plug = shaderNodeFn.findPlug( "shadowAttenuation" );
    plug.setValue( 0 );

    Setup2dTextureNode( colorMapNode );

    return;
}



void Maya::GetUniqueShaders( MStringArray& shaders )
{
    MStatus status = MS::kSuccess;

    S_string shaderFilenames;

    // FIXME: should we be using IGSHADERS::getShaderObjects()?
    Maya::S_MObject shaderObjects;
    Maya::findNodesOfType( shaderObjects, MFn::kLambert );
    Maya::findNodesOfType( shaderObjects, MFn::kBlinn );

    Maya::S_MObject::iterator itor = shaderObjects.begin();
    Maya::S_MObject::iterator end = shaderObjects.end();
    for( ; itor != end; ++itor )
    {
        MObject& shaderNode = (*itor);

        if ( shaderNode == MObject::kNullObj )
            continue;

        MFnDependencyNode shaderNodeFn( shaderNode, &status );
        MFn::Type apiType = shaderNodeFn.object().apiType();

        switch (apiType)
        {
        case MFn::kLambert:
            break;
        case MFn::kBlinn:
            break;
            //case MFn::kPhong:
            //case MFn::kPhongExplorer:
            //case MFn::kLayeredShader:
        default:
            continue;
            break;
        }

        std::string shaderFilename = Maya::GetShader( shaderNode );
        if( !shaderFilename.empty() )
        {
            Nocturnal::Insert<S_string>::Result inserted = shaderFilenames.insert( shaderFilename );
            if ( inserted.second )
            {
                shaders.append( MString( shaderFilename.c_str() ) );
            }

            //Nocturnal::Insert<M_TypeToShaderIDs>::Result insertedType = typeToShaderIDs.insert( M_TypeToShaderIDs::value_type( apiType, M_ShaderIDToObject() ) );
            //M_ShaderIDToObject& typeToShaderID = insertedType.first->second;
            //Nocturnal::Insert<M_ShaderIDToObject>::Result inserted = typeToShaderID.insert( M_ShaderIDToObject::value_type( shaderId, shaderNode ) );
            //if ( !inserted.second )
            //{
            //  MObject& existingShaderNode = inserted.first->second;
            //  MFnSet existingShaderSet( existingShaderNode, &status );

            //  MFnSet shaderSet( shaderNode, &status );

            //  // see what elements the potential source shader applies to
            //  MSelectionList shaderMembers;
            //  status = shaderSet.getMembers( shaderMembers, false );

            //  int memberCount = shaderMembers.length();
            //  for ( int index = 0; index < memberCount; ++index )
            //  {
            //    // inspect the j'th member of the shader
            //    MObject memberNode;
            //    status = shaderMembers.getDependNode( index, memberNode );
            //    if ( status == MS::kSuccess )
            //    {
            //      // the source geometry is a member of the shader, so add the destination
            //      status = existingShaderSet.addMember( memberNode );

            //      status = shaderSet.removeMember( memberNode );

            //      if ( status != MS::kSuccess )
            //      {
            //        //MGlobal::displayWarning( destinShader.name()+" shading engine would not accept a surface.");
            //        continue;
            //      }
            //    }
            //  }
            //}
        }
    }
}

void Maya::CleanupShaders()
{
    MStatus status = MS::kSuccess;

    // fix texture paths
    try
    {
        Console::Print( "Updating texture paths (RenameAllTexturePaths)...\n" );

        MString command( "RenameAllTexturePaths( \"" );
        command += "[xX]:/[a-zA-Z0-9]+/assets/[a-zA-Z0-9]+/textures/";
        command += "\", \"";
        command += Finder::ProjectAssets().c_str();
        command += "textures/\")";

        status = MGlobal::executeCommand( command, false, false );
    }
    catch( ... )
    {
        // do nothing
    }

    try
    {
        Console::Print( "Deleting unused shader nodes (MLdeleteUnused)...\n" );

        status = MGlobal::executeCommand( "MLdeleteUnused", false, false );
    }
    catch( ... )
    {
        // do nothing
    }

    Maya::UpdateShaders();
}

// This will replace global proc int igLibReloadTextures(int $quiet) in maya/scripts/igLib.mel
//
void Maya::UpdateShaders()
{
    MStatus status;

    // FIXME: should we be using IGSHADERS::getShaderObjects()?
    Maya::S_MObject shaderObjects;
    Maya::findNodesOfType( shaderObjects, MFn::kLambert );
    Maya::findNodesOfType( shaderObjects, MFn::kBlinn );

    Maya::S_MObject::iterator itor = shaderObjects.begin();
    Maya::S_MObject::iterator end = shaderObjects.end();
    for( ; itor != end; ++itor )
    {
        MObject& shaderNode = (*itor);
        if ( shaderNode != MObject::kNullObj )
        {
            UpdateTexturePaths( shaderNode );
        }
    }
}


void Maya::UpdateTexturePaths( MObject &shaderNode )
{
    std::string shaderFilename = GetShader( shaderNode );
    if( shaderFilename.empty() )
    {
        return;
    }

    // get our shader class
    AssetClassPtr shader = NULL;
    try
    {
        shader = Asset::AssetClass::LoadAssetClass( shaderFilename );
        if( !shader.ReferencesObject() )
        {
            return;
        }
    }
    catch( const Asset::UnableToLocateAssetClassException& )
    {
        // skip this one
        return;
    }

    AttributeViewer< StandardColorMapAttribute > colorMap( shader, true );
    AttributeViewer< StandardNormalMapAttribute > normalMap( shader, true );

    UpdateTexturePaths( shaderNode, colorMap->GetFileReference().GetPath(), normalMap->GetFileReference().GetPath() );
}

void Maya::UpdateTexturePaths( MObject& shaderNode, const std::string& colorMapFilePath, const std::string& normalMapFilePath )
{
    // find the colormap file texture node
    MFnDependencyNode nodeFn( shaderNode );
    MPlug plug = nodeFn.findPlug( "color" );

    MPlugArray connectedTo;
    plug.connectedTo( connectedTo, true, false );

    u32 len = connectedTo.length();
    if( len != 1 )
        return;

    MObject fileNode = connectedTo[0].node();
    nodeFn.setObject( fileNode );

    // set the texture path
    plug = nodeFn.findPlug( s_TextureFileAttrName );
    plug.setValue( colorMapFilePath.c_str() );

    // if there is a normalmap
    if( !normalMapFilePath.empty() )
    {
        // find the normalmap file texture node
        nodeFn.setObject( shaderNode );

        if( !nodeFn.hasAttribute( "normalCamera" ) )
        {
            return;
        }

        plug = nodeFn.findPlug( "normalCamera" );

        MPlugArray connectedTo;
        plug.connectedTo( connectedTo, true, false );

        if( connectedTo.length() != 1 )
        {
            return;
        }

        fileNode = connectedTo[0].node();
        nodeFn.setObject( fileNode );

        // set the texture path
        plug = nodeFn.findPlug( s_TextureFileAttrName );
        plug.setValue( normalMapFilePath.c_str() );
    }

}


void Maya::GetTexturePaths( MObject& shaderNode, std::string& colorMapFilePath, std::string& normalMapFilePath )
{
    // find the colormap file texture node
    MFnDependencyNode nodeFn( shaderNode );
    MPlug plug = nodeFn.findPlug( "color" );

    MPlugArray connectedTo;
    plug.connectedTo( connectedTo, true, false );

    if( connectedTo.length() != 1 )
        return;

    MObject fileNode = connectedTo[0].node();
    nodeFn.setObject( fileNode );

    // set the texture path
    plug = nodeFn.findPlug( s_TextureFileAttrName );

    MString textureNameStr;
    plug.getValue( textureNameStr );
    colorMapFilePath = textureNameStr.asChar();


    // see if it has a normalmap file texture node
    nodeFn.setObject( shaderNode );
    if( !nodeFn.hasAttribute( "normalCamera" ) )
        return;

    plug = nodeFn.findPlug( "normalCamera" );

    connectedTo.clear();
    plug.connectedTo( connectedTo, true, false );

    if( connectedTo.length() != 1 )
        return;

    fileNode = connectedTo[0].node();
    nodeFn.setObject( fileNode );

    // set the texture path
    plug = nodeFn.findPlug( s_TextureFileAttrName );
    plug.getValue( textureNameStr );

    normalMapFilePath = textureNameStr.asChar();
}

void Maya::Setup2dTextureNode( MObject &textureMapNode )
{
    // create the place2dtexturenode
    MFnDependencyNode nodeFn;
    MFnDependencyNode textureMapNodeFn( textureMapNode );

    MObject place2dTextureNode = nodeFn.create( "place2dTexture" );
    MFnDependencyNode place2dTextureNodeFn( place2dTextureNode );

    MDGModifier dgModifier;

    // hook all its junk up to the texture node
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "coverage" ), textureMapNode, textureMapNodeFn.attribute( "coverage" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "translateFrame" ), textureMapNode, textureMapNodeFn.attribute( "translateFrame" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "rotateFrame" ), textureMapNode, textureMapNodeFn.attribute( "rotateFrame" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "mirrorU" ), textureMapNode, textureMapNodeFn.attribute( "mirrorU" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "mirrorV" ), textureMapNode, textureMapNodeFn.attribute( "mirrorV" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "stagger" ), textureMapNode, textureMapNodeFn.attribute( "stagger" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "wrapU" ), textureMapNode, textureMapNodeFn.attribute( "wrapU" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "wrapV" ), textureMapNode, textureMapNodeFn.attribute( "wrapV" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "repeatUV" ), textureMapNode, textureMapNodeFn.attribute( "repeatUV" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "offset" ), textureMapNode, textureMapNodeFn.attribute( "offset" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "rotateUV" ), textureMapNode, textureMapNodeFn.attribute( "rotateUV" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "noiseUV" ), textureMapNode, textureMapNodeFn.attribute( "noiseUV" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "vertexUvOne" ), textureMapNode, textureMapNodeFn.attribute( "vertexUvOne" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "vertexUvTwo" ), textureMapNode, textureMapNodeFn.attribute( "vertexUvTwo" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "vertexUvThree" ), textureMapNode, textureMapNodeFn.attribute( "vertexUvThree" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "vertexCameraOne" ), textureMapNode, textureMapNodeFn.attribute( "vertexCameraOne" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "outUV" ), textureMapNode, textureMapNodeFn.attribute( "outUV" ) );
    dgModifier.connect( place2dTextureNode, place2dTextureNodeFn.attribute( "outUvFilterSize" ), textureMapNode, textureMapNodeFn.attribute( "outUvFilterSize" ) );

    dgModifier.doIt();
}


void Maya::SetShader( MObject &object, const std::string& shaderFilename )
{
    Maya::SetStringAttribute( object, s_ShaderFilenameAttrName, shaderFilename );
}


std::string Maya::GetShader( const MObject &object )
{
    const char* shaderAttrName( s_ShaderFilenameAttrName );

    std::string shaderFilename;
    Maya::GetStringAttribute( object, shaderAttrName, shaderFilename );

    return shaderFilename;
}


bool Maya::HasAlpha( const std::string& shaderPath )
{
    ShaderAssetPtr shader = Asset::AssetClass::LoadAssetClass<ShaderAsset>( shaderPath );

    if ( !shader.ReferencesObject() )
    {
        return false;
    }

    AttributeViewer< StandardColorMapAttribute > colorMap( shader );

    __timeb64 now;
    _ftime64_s( &now );

    if ( ( g_CacheUpdateTimes.find( colorMap->GetFileReference().GetHash() ) == g_CacheUpdateTimes.end() ) || colorMap->GetFileReference().GetFile().HasChangedSince( g_CacheUpdateTimes[ colorMap->GetFileReference().GetHash() ] ) )
    {
        bool result = false;

        IG::Texture* textureFile = 0;
        textureFile = IG::Texture::LoadFile( colorMap->GetFileReference().GetPath().c_str(), false, NULL );

        if ( textureFile )
        {
            f32* alpha_data = textureFile ->GetFacePtr( 0, IG::Texture::A );
            u32 pixelCount = textureFile->m_Width * textureFile->m_Height;
            for( u32 i = 0; i < pixelCount; ++i )
            {
                if ( *alpha_data < IG::COLOR_MAP_ALPHA_THRESHOLD )
                {
                    result = true;
                    break;
                }
                ++alpha_data;
            }

            delete textureFile;
        }

        g_CacheUpdateTimes[ colorMap->GetFileReference().GetHash() ] = now.time;
        g_HasAlphaCache[ colorMap->GetFileReference().GetHash() ] = result;
    }

    return g_HasAlphaCache[ colorMap->GetFileReference().GetHash() ];
}

void Maya::GetShaders( const std::string& nameFilter, V_string& shaderNames )
{
    shaderNames.clear();

    std::string lowercasePattern = nameFilter;
    std::transform( lowercasePattern.begin(), lowercasePattern.end(), lowercasePattern.begin(), tolower );

    std::string fileSpec = "*";

    if ( lowercasePattern.length() )
    {
        fileSpec += lowercasePattern;
        fileSpec += "*";
    }

    // this assumes that the user hasn't added a .sh or .shader suffix to their seach query
    FinderSpecs::Asset::SHADER_DECORATION.Modify( fileSpec );

    File::S_Reference shaderRefs;
    File::GlobalResolver().Find( fileSpec, shaderRefs );

    for( File::S_Reference::iterator itr = shaderRefs.begin(), end = shaderRefs.end(); itr != end; ++itr )
    {
        File::ReferencePtr shaderRef = (*itr);
        shaderRef->Resolve();
        shaderNames.push_back( shaderRef->GetPath() );
    }

    std::sort( shaderNames.begin(), shaderNames.end() );
}

void Maya::GetMayaSurfaceShaderNames( tuid shaderID, MStringArray& shaderNames )
{
    MStatus status;

    MFnDependencyNode surfaceFn;
    tuid currentShaderId = TUID::Null;

    // shaders derrive from kDependencyNode
    MItDependencyNodes iter( MFn::kDependencyNode, &status );
    if ( status == MS::kSuccess )
    {
        for ( ; !iter.isDone(); iter.next() )
        {
            MObject surfaceObject = iter.item(&status);
            if ( status == MS::kSuccess )
            {
                // see if this surfaceObject is indeed a maya shader
                MFn::Type surfaceType = surfaceObject.apiType();
                if ( surfaceType == MFn::kLambert
                    || surfaceType == MFn::kBlinn )
                    //|| surfaceType == MFn::kPhong
                    //|| surfaceType == MFn::kPhongExplorer
                    //|| surfaceType == MFn::kLayeredShader
                    //|| surfaceType == MFn::kReflect )
                {
                    currentShaderId = Maya::GetTUIDAttribute( surfaceObject, s_ShaderAssetIdAttrName, &status );

                    if ( status == MS::kSuccess
                        && currentShaderId == shaderID )
                    {
                        // found one
                        surfaceFn.setObject( surfaceObject );
                        shaderNames.append( surfaceFn.name() );
                    }
                }
            }
        }
    }
}

MStatus Maya::ShowShaderBrowser()
{
    if ( !g_ShaderBrowserDlg )
    {
        g_ShaderBrowserDlg = new ShaderBrowser();
    }
    g_ShaderBrowserDlg->Show();

    return MS::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when maya s existing to make sure that the ShaderBrowser
// window is hidden
//
void Maya::HideShaderBrowserCallBack( void* clientData )
{
    if ( g_ShaderBrowserDlg )
    {
        g_ShaderBrowserDlg->Hide();
    }

    g_ShaderBrowserDlg = NULL;
}

