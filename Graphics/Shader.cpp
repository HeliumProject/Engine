#include "GraphicsPch.h"
#include "Graphics/Shader.h"

#include "Platform/Thread.h"
#include "Engine/AssetLoader.h"
#include "Rendering/RPixelShader.h"
#include "Rendering/Renderer.h"
#include "Rendering/RVertexShader.h"

#include "Reflect/TranslatorDeduction.h"

HELIUM_IMPLEMENT_ASSET( Helium::Shader, Graphics, AssetType::FLAG_NO_TEMPLATE );
HELIUM_IMPLEMENT_ASSET( Helium::ShaderVariant, Graphics, AssetType::FLAG_NO_TEMPLATE | AssetType::FLAG_GENERATED_FROM_OWNER );
HELIUM_DEFINE_CLASS( Helium::Shader::PersistentResourceData );
HELIUM_DEFINE_CLASS( Helium::ShaderVariant::PersistentResourceData );

using namespace Helium;

void Shader::PersistentResourceData::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField(&Shader::PersistentResourceData::m_systemOptions, TXT("systemOptions"));
    comp.AddField(&Shader::PersistentResourceData::m_userOptions, TXT("userOptions"));
}

void ShaderVariant::PersistentResourceData::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField(&ShaderVariant::PersistentResourceData::m_resourceCount, TXT("m_resourceCount"));
}

Shader::BEGIN_LOAD_VARIANT_FUNC* Shader::sm_pBeginLoadVariantOverride = NULL;
Shader::TRY_FINISH_LOAD_VARIANT_FUNC* Shader::sm_pTryFinishLoadVariantOverride = NULL;
void* Shader::sm_pVariantLoadOverrideData = NULL;


CompiledShaderData::CompiledShaderData()
{

}

CompiledShaderData::~CompiledShaderData()
{

}

HELIUM_DEFINE_CLASS( Helium::CompiledShaderData );

void CompiledShaderData::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &CompiledShaderData::compiledCodeBuffer, TXT( "compiledCodeBuffer" ) );
    comp.AddField( &CompiledShaderData::constantBuffers,    TXT( "constantBuffers" ) );
    comp.AddField( &CompiledShaderData::samplerInputs,      TXT( "samplerInputs" ) );
    comp.AddField( &CompiledShaderData::textureInputs,      TXT( "textureInputs" ) );
}


HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderConstantInfo );

void ShaderConstantInfo::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderConstantInfo::name,       TXT( "name" ) );
    comp.AddField( &ShaderConstantInfo::offset,     TXT( "offset" ) );
    comp.AddField( &ShaderConstantInfo::size,       TXT( "size" ) );
    comp.AddField( &ShaderConstantInfo::usedSize,   TXT( "usedSize" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderConstantBufferInfo );

void ShaderConstantBufferInfo::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderConstantBufferInfo::name,         TXT( "name" ) );
    comp.AddField( &ShaderConstantBufferInfo::constants,    TXT( "constants" ) );
    comp.AddField( &ShaderConstantBufferInfo::index,        TXT( "index" ) );
    comp.AddField( &ShaderConstantBufferInfo::size,         TXT( "size" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderConstantBufferInfoSet );

void ShaderConstantBufferInfoSet::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderConstantBufferInfoSet::buffers,  TXT( "buffers" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderSamplerInfo );

void ShaderSamplerInfo::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderSamplerInfo::name,       TXT( "name" ) );
    comp.AddField( &ShaderSamplerInfo::bindIndex,  TXT( "bindIndex" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderSamplerInfoSet );

void ShaderSamplerInfoSet::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderSamplerInfoSet::inputs,  TXT( "inputs" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderTextureInfo );

void ShaderTextureInfo::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderTextureInfo::name,       TXT( "name" ) );
    comp.AddField( &ShaderTextureInfo::bindIndex,  TXT( "bindIndex" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ShaderTextureInfoSet );

void ShaderTextureInfoSet::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ShaderTextureInfoSet::inputs,  TXT( "inputs" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::Shader::Toggle );

void Shader::Toggle::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Shader::Toggle::name,  TXT( "name" ) );
    comp.AddField( &Shader::Toggle::shaderTypeFlags,  TXT( "shaderTypeFlags" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::Shader::Select );

void Shader::Select::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Shader::Select::name,           TXT( "name" ) );
    comp.AddField( &Shader::Select::allFlags,       TXT( "allFlags" ) );
    comp.AddField( &Shader::Select::choices,        TXT( "choices" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::Shader::SelectPair );

void Shader::SelectPair::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Shader::SelectPair::name,   TXT( "name" ) );
    comp.AddField( &Shader::SelectPair::choice, TXT( "choice" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::Shader::Options );

void Shader::Options::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Shader::Options::m_toggles,  TXT( "m_toggles" ) );
    comp.AddField( &Shader::Options::m_selects,  TXT( "m_selects" ) );
}

/// Constructor.
Shader::Shader()
: m_bPrecacheAllVariants( false )
{
    MemoryZero( m_variantCounts, sizeof( m_variantCounts ) );
}

/// Destructor.
Shader::~Shader()
{
}

/// @copydoc Asset::FinalizeLoad()
void Shader::FinalizeLoad()
{
    Base::FinalizeLoad();

    // Cache the number of user variants for each shader type.  Note that we don't need to create variants for the
    // default type template.
    if( IsDefaultTemplate() )
    {
        MemoryZero( m_variantCounts, sizeof( m_variantCounts ) );
    }
    else
    {
        const Options& rUserOptions = m_persistentResourceData.GetUserOptions();

        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_variantCounts ); ++shaderTypeIndex )
        {
            size_t count = rUserOptions.ComputeOptionSetCount( static_cast< RShader::EType >( shaderTypeIndex ) );
            HELIUM_ASSERT( count <= UINT32_MAX );

            m_variantCounts[ shaderTypeIndex ] = static_cast< uint32_t >( count );
        }
    }
}

#if HELIUM_TOOLS
/// @copydoc Asset::PostSave()
void Shader::PostSave()
{
    Base::PostSave();

    // Precache all shader variants if flagged to do so and the load process has been overridden by the shader
    // variant resource handler.
    // XXX TMC TODO: Replace with a more robust method for checking whether we're running within the editor.
    if( !IsDefaultTemplate() && m_bPrecacheAllVariants && sm_pBeginLoadVariantOverride )
    {
        AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
        HELIUM_ASSERT( pAssetLoader );

        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_variantCounts ); ++shaderTypeIndex )
        {
            RShader::EType shaderType = static_cast< RShader::EType >( shaderTypeIndex );

            size_t variantCount = m_variantCounts[ shaderTypeIndex ];
            HELIUM_ASSERT( variantCount <= UINT32_MAX );
            for( size_t variantIndex = 0; variantIndex < variantCount; ++variantIndex )
            {
                size_t loadId = BeginLoadVariant( shaderType, static_cast< uint32_t >( variantIndex ) );
                if( IsValid( loadId ) )
                {
                    ShaderVariantPtr spVariant;
                    while( !TryFinishLoadVariant( loadId, spVariant ) )
                    {
                        Thread::Yield();
                    }
                }
            }
        }
    }
}
#endif  // HELIUM_TOOLS

/// @copydoc Resource::GetCacheName()
Name Shader::GetCacheName() const
{
    static Name cacheName( TXT( "Shader" ) );

    return cacheName;
}

/// Begin asynchronous loading of a shader variant.
///
/// @param[in] shaderType       Shader type.
/// @param[in] userOptionIndex  Index associated with the user option combination for the shader variant.
///
/// @return  ID associated with the load procedure, or an invalid index if the load could not be started.
///
/// @see TryFinishLoadVariant()
size_t Shader::BeginLoadVariant( RShader::EType shaderType, uint32_t userOptionIndex )
{
    HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

    // Make sure the user option index is valid.
    if( userOptionIndex >= m_variantCounts[ shaderType ] )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "Shader::BeginLoadVariant(): Invalid user option index %" ) PRIuSZ TXT( " specified for " )
            TXT( "variant of shader \"%s\" (only %" ) PRIuSZ TXT ( " variants are available for shader type %" )
            PRId32 TXT( ").\n" ) ),
            userOptionIndex,
            *GetPath().ToString(),
            m_variantCounts[ shaderType ],
            static_cast< int32_t >( shaderType ) );

        return Invalid< size_t >();
    }

    // Use the begin-load override if one is registered.
    if( sm_pBeginLoadVariantOverride )
    {
        size_t loadId = sm_pBeginLoadVariantOverride(
            sm_pVariantLoadOverrideData,
            this,
            shaderType,
            userOptionIndex );

        return loadId;
    }

    // Build the object path name.
    char shaderTypeCharacter;
    if( shaderType == RShader::TYPE_VERTEX )
    {
        shaderTypeCharacter = TXT( 'v' );
    }
    else
    {
        HELIUM_ASSERT( shaderType == RShader::TYPE_PIXEL );
        shaderTypeCharacter = TXT( 'p' );
    }

    String variantNameString;
    variantNameString.Format( TXT( "%c%" ) PRIu32, shaderTypeCharacter, userOptionIndex );

    AssetPath variantPath;
    HELIUM_VERIFY( variantPath.Set( Name( variantNameString ), false, GetPath() ) );

    // Begin the load process.
    AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pAssetLoader );

    size_t loadId = pAssetLoader->BeginLoadObject( variantPath );

    return loadId;
}

/// Perform a non-blocking attempt to sync with an asynchronous shader variant load request.
///
/// @param[in]  loadId      Load request ID.
/// @param[out] rspVariant  Smart pointer set to the variant reference if loading has completed.
///
/// @return  True if loading has completed, false if not.  Note that if this returns true, the load request ID will
///          no longer be valid for its current load request and should not be reused.
///
/// @see BeginLoadVariant()
bool Shader::TryFinishLoadVariant( size_t loadId, ShaderVariantPtr& rspVariant )
{
    HELIUM_ASSERT( IsValid( loadId ) );

    // Use the try-finish-load override if one is registered.
    if( sm_pTryFinishLoadVariantOverride )
    {
        bool bFinished = sm_pTryFinishLoadVariantOverride( sm_pVariantLoadOverrideData, loadId, rspVariant );

        return bFinished;
    }

    // Attempt to sync the object load request.
    AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pAssetLoader );

    AssetPtr spObject;
    bool bFinished = pAssetLoader->TryFinishLoad( loadId, spObject );
    if( bFinished )
    {
        rspVariant = Reflect::AssertCast< ShaderVariant >( spObject.Get() );
    }

    return bFinished;
}

/// Set override callbacks for loading shader variants.
///
/// @param[in] pBeginLoadVariantOverride      Override callback to initiate loading of a shader variant.
/// @param[in] pTryFinishLoadVariantOverride  Override callback for non-blocking testing and completion of the
///                                           shader variant load process.
/// @param[in] pVariantLoadOverrideData       Data passed in the first parameter to each callback.
///
/// @see GetBeginLoadVariantOverride(), GetTryFinishLoadVariantOverride(), GetVariantLoadOverrideData()
void Shader::SetVariantLoadOverride(
                                    BEGIN_LOAD_VARIANT_FUNC* pBeginLoadVariantOverride,
                                    TRY_FINISH_LOAD_VARIANT_FUNC* pTryFinishLoadVariantOverride,
                                    void* pVariantLoadOverrideData )
{
    sm_pBeginLoadVariantOverride = pBeginLoadVariantOverride;
    sm_pTryFinishLoadVariantOverride = pTryFinishLoadVariantOverride;
    sm_pVariantLoadOverrideData = pVariantLoadOverrideData;
}

/// Get a unique index associated with a specific set of shader preprocessor options.
///
/// Note that this will always attempt to match up a valid index, even if invalid options are specified or select
/// options are missing.
///
/// @param[in] shaderType       Type of shader.
/// @param[in] pToggleNames     List of enabled shader toggles.
/// @param[in] toggleNameCount  Number of names in the toggle name array.
/// @param[in] pSelectPairs     List of shader selection pair values.
/// @param[in] selectPairCount  Number of shader selection pair values in the given array.
///
/// @return  Index to use for the specified options.
size_t Shader::Options::GetOptionSetIndex(
    RShader::EType shaderType,
    const Name* pToggleNames,
    size_t toggleNameCount,
    const SelectPair* pSelectPairs,
    size_t selectPairCount ) const
{
    HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );
    HELIUM_ASSERT( pToggleNames || toggleNameCount == 0 );
    HELIUM_ASSERT( pSelectPairs || selectPairCount == 0 );

    uint32_t shaderTypeMask = ( 1 << shaderType );

    size_t optionIndex = 0;
    size_t optionIndexMultiplier = 1;

    size_t shaderToggleCount = m_toggles.GetSize();
    for( size_t shaderToggleIndex = 0; shaderToggleIndex < shaderToggleCount; ++shaderToggleIndex )
    {
        const Toggle& rShaderToggle = m_toggles[ shaderToggleIndex ];
        if( !( rShaderToggle.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        Name shaderToggleName = rShaderToggle.name;
        for( size_t enabledToggleIndex = 0; enabledToggleIndex < toggleNameCount; ++enabledToggleIndex )
        {
            if( pToggleNames[ enabledToggleIndex ] == shaderToggleName )
            {
                optionIndex |= optionIndexMultiplier;

                break;
            }
        }

        optionIndexMultiplier <<= 1;
    }

    size_t shaderSelectCount = m_selects.GetSize();
    for( size_t shaderSelectIndex = 0; shaderSelectIndex < shaderSelectCount; ++shaderSelectIndex )
    {
        const Select& rShaderSelect = m_selects[ shaderSelectIndex ];
        if( !( rShaderSelect.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        Name shaderSelectName = rShaderSelect.name;

        bool bSetSelectIndex = false;
        for( size_t selectPairIndex = 0; selectPairIndex < selectPairCount; ++selectPairIndex )
        {
            const SelectPair& rPair = pSelectPairs[ selectPairIndex ];
            if( rPair.name == shaderSelectName )
            {
                const DynamicArray< Name >& rShaderSelectChoices = rShaderSelect.choices;
                size_t shaderSelectChoiceCount = rShaderSelectChoices.GetSize();

                Name targetChoiceName = rPair.choice;
                if( !targetChoiceName.IsEmpty() )
                {
                    for( size_t shaderSelectChoiceIndex = 0;
                        shaderSelectChoiceIndex < shaderSelectChoiceCount;
                        ++shaderSelectChoiceIndex )
                    {
                        if( rShaderSelectChoices[ shaderSelectChoiceIndex ] == targetChoiceName )
                        {
                            optionIndex += shaderSelectChoiceIndex * optionIndexMultiplier;
                            bSetSelectIndex = true;

                            break;
                        }
                    }
                }

                break;
            }
        }

        if( !bSetSelectIndex )
        {
            optionIndex +=
                ( rShaderSelect.bOptional ? rShaderSelect.choices.GetSize() * optionIndexMultiplier : 0 );
        }

        optionIndexMultiplier *= rShaderSelect.choices.GetSize() + rShaderSelect.bOptional;
    }

    return optionIndex;
}

/// Get a unique index associated with a specific set of shader preprocessor options.
///
/// Note that this will always attempt to match up a valid index, even if invalid options are specified or select
/// options are missing.
///
/// @param[in] shaderType       Type of shader.
/// @param[in] pOptionPairs     Mixed list of shader toggle states and shader selection pair values.
/// @param[in] optionPairCount  Number of options in the option pair array.
///
/// @return  Index to use for the specified options.
size_t Shader::Options::GetOptionSetIndex(
    RShader::EType shaderType,
    const SelectPair* pOptionPairs,
    size_t optionPairCount ) const
{
    HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );
    HELIUM_ASSERT( pOptionPairs || optionPairCount == 0 );

    uint32_t shaderTypeMask = ( 1 << shaderType );

    size_t optionIndex = 0;
    size_t optionIndexMultiplier = 1;

    static const Name disabledToggleValues[] = { Name( NULL_NAME ), Name( TXT( "0" ) ), Name( TXT( "false" ) ) };

    size_t shaderToggleCount = m_toggles.GetSize();
    for( size_t shaderToggleIndex = 0; shaderToggleIndex < shaderToggleCount; ++shaderToggleIndex )
    {
        const Toggle& rShaderToggle = m_toggles[ shaderToggleIndex ];
        if( !( rShaderToggle.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        Name shaderToggleName = rShaderToggle.name;

        for( size_t optionPairIndex = 0; optionPairIndex < optionPairCount; ++optionPairIndex )
        {
            const SelectPair& rOptionPair = pOptionPairs[ optionPairIndex ];
            if( rOptionPair.name == shaderToggleName )
            {
                Name value = rOptionPair.choice;

                size_t disabledValueIndex;
                for( disabledValueIndex = 0;
                    disabledValueIndex < HELIUM_ARRAY_COUNT( disabledToggleValues );
                    ++disabledValueIndex )
                {
                    if( value == disabledToggleValues[ disabledValueIndex ] )
                    {
                        break;
                    }
                }

                if( disabledValueIndex >= HELIUM_ARRAY_COUNT( disabledToggleValues ) )
                {
                    optionIndex |= optionIndexMultiplier;
                }

                break;
            }
        }

        optionIndexMultiplier <<= 1;
    }

    size_t shaderSelectCount = m_selects.GetSize();
    for( size_t shaderSelectIndex = 0; shaderSelectIndex < shaderSelectCount; ++shaderSelectIndex )
    {
        const Select& rShaderSelect = m_selects[ shaderSelectIndex ];
        if( !( rShaderSelect.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        Name shaderSelectName = rShaderSelect.name;

        bool bSetSelectIndex = false;
        for( size_t optionPairIndex = 0; optionPairIndex < optionPairCount; ++optionPairIndex )
        {
            const SelectPair& rPair = pOptionPairs[ optionPairIndex ];
            if( rPair.name == shaderSelectName )
            {
                const DynamicArray< Name >& rShaderSelectChoices = rShaderSelect.choices;
                size_t shaderSelectChoiceCount = rShaderSelectChoices.GetSize();

                Name targetChoiceName = rPair.choice;
                if( !targetChoiceName.IsEmpty() )
                {
                    for( size_t shaderSelectChoiceIndex = 0;
                        shaderSelectChoiceIndex < shaderSelectChoiceCount;
                        ++shaderSelectChoiceIndex )
                    {
                        if( rShaderSelectChoices[ shaderSelectChoiceIndex ] == targetChoiceName )
                        {
                            optionIndex += shaderSelectChoiceIndex * optionIndexMultiplier;
                            bSetSelectIndex = true;

                            break;
                        }
                    }
                }

                break;
            }
        }

        if( !bSetSelectIndex )
        {
            optionIndex +=
                ( rShaderSelect.bOptional ? rShaderSelect.choices.GetSize() * optionIndexMultiplier : 0 );
        }

        optionIndexMultiplier *= rShaderSelect.choices.GetSize() + rShaderSelect.bOptional;
    }

    return optionIndex;
}

/// Resolve a set of shader preprocessor options from the associated index.
///
/// @param[in]  shaderType    Type of shader.
/// @param[in]  index         Option set index.
/// @param[out] rToggleNames  List of enabled shader toggles.
/// @param[out] rSelectPairs  List shader selection pair values.
void Shader::Options::GetOptionSetFromIndex(
    RShader::EType shaderType,
    size_t index,
    DynamicArray< Name >& rToggleNames,
    DynamicArray< SelectPair >& rSelectPairs ) const
{
    HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

    rToggleNames.Resize( 0 );
    rSelectPairs.Resize( 0 );

    uint32_t shaderTypeMask = ( 1 << shaderType );

    size_t shaderToggleCount = m_toggles.GetSize();
    for( size_t shaderToggleIndex = 0; shaderToggleIndex < shaderToggleCount; ++shaderToggleIndex )
    {
        const Toggle& rShaderToggle = m_toggles[ shaderToggleIndex ];
        if( !( rShaderToggle.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        if( index & 0x1 )
        {
            HELIUM_VERIFY( rToggleNames.New( rShaderToggle.name ) );
        }

        index >>= 1;
    }

    size_t shaderSelectCount = m_selects.GetSize();
    for( size_t shaderSelectIndex = 0; shaderSelectIndex < shaderSelectCount; ++shaderSelectIndex )
    {
        const Select& rShaderSelect = m_selects[ shaderSelectIndex ];
        if( !( rShaderSelect.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        const DynamicArray< Name >& rShaderSelectChoices = rShaderSelect.choices;
        size_t shaderSelectChoiceCount = rShaderSelectChoices.GetSize();

        size_t selectIndexMultiplier = shaderSelectChoiceCount + rShaderSelect.bOptional;

        size_t selectIndex = index % selectIndexMultiplier;
        index /= selectIndexMultiplier;

        if( !rShaderSelect.bOptional || selectIndex != shaderSelectChoiceCount )
        {
            SelectPair* pSelectPair = rSelectPairs.New();
            HELIUM_ASSERT( pSelectPair );
            pSelectPair->name = rShaderSelect.name;
            pSelectPair->choice = rShaderSelectChoices[ selectIndex ];
        }
    }
}

/// Compute the total number of shader preprocessor option sets based on this set of available options for a given
/// shader type.
///
/// @param[in] shaderType  Target shader type.
///
/// @return  Number of shader preprocessor option sets.
size_t Shader::Options::ComputeOptionSetCount( RShader::EType shaderType ) const
{
    HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

    uint32_t shaderTypeMask = ( 1 << shaderType );

    size_t optionSetCount = 1;

    size_t shaderToggleCount = m_toggles.GetSize();
    for( size_t shaderToggleIndex = 0; shaderToggleIndex < shaderToggleCount; ++shaderToggleIndex )
    {
        const Toggle& rShaderToggle = m_toggles[ shaderToggleIndex ];
        if( !( rShaderToggle.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        optionSetCount <<= 1;
    }

    size_t shaderSelectCount = m_selects.GetSize();
    for( size_t shaderSelectIndex = 0; shaderSelectIndex < shaderSelectCount; ++shaderSelectIndex )
    {
        const Select& rShaderSelect = m_selects[ shaderSelectIndex ];
        if( !( rShaderSelect.shaderTypeFlags & shaderTypeMask ) )
        {
            continue;
        }

        optionSetCount *= rShaderSelect.choices.GetSize() + rShaderSelect.bOptional;
    }

    return optionSetCount;
}

/// Constructor.
ShaderVariant::ShaderVariant()
: m_pRenderResourceLoadBuffer( NULL )
{
}

/// Destructor.
ShaderVariant::~ShaderVariant()
{
    HELIUM_ASSERT( !m_pRenderResourceLoadBuffer );
}

/// @copydoc Asset::PreDestroy()
void ShaderVariant::RefCountPreDestroy()
{
    HELIUM_ASSERT( !m_pRenderResourceLoadBuffer );

    m_renderResources.Clear();

    Base::RefCountPreDestroy();
}

/// @copydoc Asset::NeedsPrecacheResourceData()
bool ShaderVariant::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc Asset::BeginPrecacheResourceData()
bool ShaderVariant::BeginPrecacheResourceData()
{
    HELIUM_ASSERT( m_renderResourceLoads.IsEmpty() );
    HELIUM_ASSERT( !m_pRenderResourceLoadBuffer );

    // Don't load any resources if we have no renderer.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return true;
    }

    // Make sure the shader type is valid.
    Name variantName = GetName();
    char shaderTypeCharacter = ( *variantName )[ 0 ];
    if( shaderTypeCharacter != TXT( 'v' ) && shaderTypeCharacter != TXT( 'p' ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "ShaderVariant::BeginPrecacheResourceData(): Unable to determine shader type from variant " )
            TXT( "name \"%s\".\n" ) ),
            *GetPath().ToString() );

        return false;
    }

    // Allocate memory for load staging and begin loading the shader data.
    size_t renderResourceCount = m_renderResources.GetSize();
    HELIUM_ASSERT( renderResourceCount <= UINT32_MAX );

    m_renderResourceLoads.Reserve( renderResourceCount );
    m_renderResourceLoads.Resize( renderResourceCount );
    m_renderResourceLoads.Trim();

    size_t* pLoadSizes = static_cast< size_t* >( alloca( sizeof( size_t ) * renderResourceCount ) );
    HELIUM_ASSERT( pLoadSizes );
    size_t totalLoadSize = 0;

    for( size_t resourceIndex = 0; resourceIndex < renderResourceCount; ++resourceIndex )
    {
        HELIUM_ASSERT( !m_renderResources[ resourceIndex ] );

        size_t loadSize = GetSubDataSize( static_cast< uint32_t >( resourceIndex ) );
        pLoadSizes[ resourceIndex ] = loadSize;
        if( IsInvalid( loadSize ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "ShaderVariant::BeginPrecacheResourceData(): Could not find resource sub-data %" ) PRIuSZ
                TXT( " for shader variant \"%s\".\n" ) ),
                resourceIndex,
                *GetPath().ToString() );

            continue;
        }

        if( loadSize == 0 )
        {
            // No data has been cached for this sub-resource.
            m_renderResources[ resourceIndex ] = NULL;
            m_constantBufferSets[ resourceIndex ].buffers.Clear();
            m_samplerInputSets[ resourceIndex ].inputs.Clear();
            m_textureInputSets[ resourceIndex ].inputs.Clear();
        }
        else
        {
            totalLoadSize += loadSize;
        }
    }

    m_pRenderResourceLoadBuffer = DefaultAllocator().Allocate( totalLoadSize );
    HELIUM_ASSERT( m_pRenderResourceLoadBuffer );

    uint8_t* pTargetBuffer = static_cast< uint8_t* >( m_pRenderResourceLoadBuffer );

    for( size_t resourceIndex = 0; resourceIndex < renderResourceCount; ++resourceIndex )
    {
        LoadData& rLoadData = m_renderResourceLoads[ resourceIndex ];

        size_t loadSize = pLoadSizes[ resourceIndex ];
        if( IsInvalid( loadSize ) || loadSize == 0 )
        {
            SetInvalid( rLoadData.id );

            continue;
        }

        rLoadData.size = loadSize;

        rLoadData.pData = pTargetBuffer;
        rLoadData.id = BeginLoadSubData( pTargetBuffer, static_cast< uint32_t >( resourceIndex ) );
        if( IsInvalid( rLoadData.id ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "ShaderVariant::BeginPrecacheResourceData(): Failed to begin asynchronous load of " )
                TXT( "resource sub-data %" ) PRIuSZ TXT( " of shader variant \"%s\".\n" ) ),
                resourceIndex,
                *GetPath().ToString() );

            SetInvalid( rLoadData.id );
        }
        else
        {
            pTargetBuffer += loadSize;
        }
    }

    return true;
}

/// @copydoc Asset::TryFinishPrecacheResourceData()
bool ShaderVariant::TryFinishPrecacheResourceData()
{
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    // Get the type of shader being loaded.
    Name variantName = GetName();
    char shaderTypeCharacter = ( *variantName )[ 0 ];

    RShader::EType shaderType;
    if( shaderTypeCharacter == TXT( 'v' ) )
    {
        shaderType = RShader::TYPE_VERTEX;
    }
    else
    {
        HELIUM_ASSERT( shaderTypeCharacter == TXT( 'p' ) );
        shaderType = RShader::TYPE_PIXEL;
    }

    // Wait for all async load requests to complete.
    bool bHavePendingLoad = false;

    size_t loadRequestCount = m_renderResourceLoads.GetSize();
    for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestCount; ++loadRequestIndex )
    {
        LoadData& rLoadData = m_renderResourceLoads[ loadRequestIndex ];
        if( IsInvalid( rLoadData.id ) )
        {
            continue;
        }

        if( !TryFinishLoadSubData( rLoadData.id ) )
        {
            bHavePendingLoad = true;

            continue;
        }

        SetInvalid( rLoadData.id );

        CompiledShaderData *compiled_shader_data = NULL;
        Reflect::ObjectPtr object_ptr;

        object_ptr = Cache::ReadCacheObjectFromBuffer((uint8_t*) rLoadData.pData, 0, rLoadData.size);
        compiled_shader_data = Reflect::SafeCast<CompiledShaderData>(object_ptr.Get());
        
        RShaderPtr spShaderBase;

        HELIUM_ASSERT(compiled_shader_data);
        if (compiled_shader_data)
        {
            m_constantBufferSets[loadRequestIndex].buffers = compiled_shader_data->constantBuffers;
            m_samplerInputSets[loadRequestIndex].inputs = compiled_shader_data->samplerInputs;
            m_textureInputSets[loadRequestIndex].inputs = compiled_shader_data->textureInputs;

            HELIUM_ASSERT(!compiled_shader_data->compiledCodeBuffer.IsEmpty());
            if( shaderType == RShader::TYPE_VERTEX )
            {
                spShaderBase = pRenderer->CreateVertexShader( 
                    compiled_shader_data->compiledCodeBuffer.GetSize(), 
                    &compiled_shader_data->compiledCodeBuffer[0]);
            }
            else
            {
                spShaderBase = pRenderer->CreatePixelShader( 
                    compiled_shader_data->compiledCodeBuffer.GetSize(), 
                    &compiled_shader_data->compiledCodeBuffer[0] );
            }
        }
        
        if( !spShaderBase )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "ShaderVariant::TryFinishPrecacheResourceData(): Failed to create shader for sub-data %" )
                PRIuSZ TXT( " of shader \"%s\".\n" ) ),
                loadRequestIndex,
                *GetPath().ToString() );
        }

        m_renderResources[ loadRequestIndex ] = spShaderBase;
    }

    if( bHavePendingLoad )
    {
        return false;
    }

    // All load requests have completed, so free all memory allocated for resource staging.
    m_renderResourceLoads.Clear();

    DefaultAllocator().Free( m_pRenderResourceLoadBuffer );
    m_pRenderResourceLoadBuffer = NULL;

    return true;
}

bool Helium::Shader::LoadPersistentResourceObject( Reflect::ObjectPtr &_object )
{
    HELIUM_ASSERT(_object.ReferencesObject());
    if (!_object.ReferencesObject())
    {
        return false;
    }

    _object->CopyTo(&m_persistentResourceData);

    return true;
}

bool Helium::ShaderVariant::LoadPersistentResourceObject( Reflect::ObjectPtr &_object )
{
    HELIUM_ASSERT(_object.ReferencesObject());
    if (!_object.ReferencesObject())
    {
        return false;
    }

    _object->CopyTo(&m_persistentResourceData);

    // Reserve space for the resource data that will be loaded later.
    for( size_t resourceIndex = 0; resourceIndex < m_renderResources.GetSize(); ++resourceIndex )
    {
        m_renderResources[ resourceIndex ].Release();
    }

    m_renderResources.Reserve( m_persistentResourceData.m_resourceCount );
    m_renderResources.Resize( m_persistentResourceData.m_resourceCount );
    m_renderResources.Trim();

    m_constantBufferSets.Reserve( m_persistentResourceData.m_resourceCount );
    m_constantBufferSets.Resize( m_persistentResourceData.m_resourceCount );
    m_constantBufferSets.Trim();

    m_samplerInputSets.Reserve( m_persistentResourceData.m_resourceCount );
    m_samplerInputSets.Resize( m_persistentResourceData.m_resourceCount );
    m_samplerInputSets.Trim();

    m_textureInputSets.Reserve( m_persistentResourceData.m_resourceCount );
    m_textureInputSets.Resize( m_persistentResourceData.m_resourceCount );
    m_textureInputSets.Trim();

    return true;
}

/// @copydoc Resource::GetCacheName()
Name ShaderVariant::GetCacheName() const
{
    static Name cacheName( TXT( "Shader" ) );

    return cacheName;
}

Helium::ShaderVariant::PersistentResourceData::PersistentResourceData()
    : m_resourceCount(0)
{

}