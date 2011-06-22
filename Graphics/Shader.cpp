//----------------------------------------------------------------------------------------------------------------------
// Shader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Shader.h"

#include "Platform/Thread.h"
#include "Engine/BinaryDeserializer.h"
#include "Engine/GameObjectLoader.h"
#include "Rendering/RPixelShader.h"
#include "Rendering/Renderer.h"
#include "Rendering/RVertexShader.h"

using namespace Helium;

L_IMPLEMENT_OBJECT( Shader, Graphics, GameObjectType::FLAG_NO_TEMPLATE );
L_IMPLEMENT_OBJECT( ShaderVariant, Graphics, GameObjectType::FLAG_NO_TEMPLATE );

Shader::BEGIN_LOAD_VARIANT_FUNC* Shader::sm_pBeginLoadVariantOverride = NULL;
Shader::TRY_FINISH_LOAD_VARIANT_FUNC* Shader::sm_pTryFinishLoadVariantOverride = NULL;
void* Shader::sm_pVariantLoadOverrideData = NULL;

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderConstantInfo::Serialize( Serializer& s )
{
    s << name;
    s << offset;
    s << size;
    s << usedSize;
}

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderConstantBufferInfo::Serialize( Serializer& s )
{
    s << name;
    s << Serializer::WrapStructDynArray( constants );
    s << index;
    s << size;
}

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderConstantBufferInfoSet::Serialize( Serializer& s )
{
    s << Serializer::WrapStructDynArray( buffers );
}

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderSamplerInfo::Serialize( Serializer& s )
{
    s << name;
    s << bindIndex;
}

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderSamplerInfoSet::Serialize( Serializer& s )
{
    s << Serializer::WrapStructDynArray( inputs );
}

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderTextureInfo::Serialize( Serializer& s )
{
    s << name;
    s << bindIndex;
}

/// Serialize this structure.
///
/// @param[in] s  Serializer with which to serialize.
void ShaderTextureInfoSet::Serialize( Serializer& s )
{
    s << Serializer::WrapStructDynArray( inputs );
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

/// @copydoc GameObject::Serialize()
void Shader::Serialize( Serializer& s )
{
    L_SERIALIZE_BASE( s );

    s << L_TAGGED( m_bPrecacheAllVariants );
}

/// @copydoc GameObject::FinalizeLoad()
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

#if L_EDITOR
/// @copydoc GameObject::PostSave()
void Shader::PostSave()
{
    Base::PostSave();

    // Precache all shader variants if flagged to do so and the load process has been overridden by the shader
    // variant resource handler.
    // XXX TMC TODO: Replace with a more robust method for checking whether we're running within the editor.
    if( !IsDefaultTemplate() && m_bPrecacheAllVariants && sm_pBeginLoadVariantOverride )
    {
        GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
        HELIUM_ASSERT( pObjectLoader );

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
#endif  // L_EDITOR

/// @copydoc Resource::SerializePersistentResourceData()
void Shader::SerializePersistentResourceData( Serializer& s )
{
    m_persistentResourceData.Serialize( s );
}

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
            TRACE_ERROR,
            ( TXT( "Shader::BeginLoadVariant(): Invalid user option index %" ) TPRIuSZ TXT( " specified for " )
            TXT( "variant of shader \"%s\" (only %" ) TPRIuSZ TXT ( " variants are available for shader type %" )
            TPRId32 TXT( ").\n" ) ),
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
    tchar_t shaderTypeCharacter;
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
    variantNameString.Format( TXT( "%c%" ) TPRIu32, shaderTypeCharacter, userOptionIndex );

    GameObjectPath variantPath;
    HELIUM_VERIFY( variantPath.Set( Name( variantNameString ), false, GetPath() ) );

    // Begin the load process.
    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
    HELIUM_ASSERT( pObjectLoader );

    size_t loadId = pObjectLoader->BeginLoadObject( variantPath );

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
    GameObjectLoader* pObjectLoader = GameObjectLoader::GetStaticInstance();
    HELIUM_ASSERT( pObjectLoader );

    GameObjectPtr spObject;
    bool bFinished = pObjectLoader->TryFinishLoad( loadId, spObject );
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

/// Serialize this shader preprocessor toggle information.
///
/// @param[in] s  Serializer with which to serialize.
void Shader::Toggle::Serialize( Serializer& s )
{
    s << name;
    s << shaderTypeFlags;
}

/// Serialize this shader preprocessor selection information.
///
/// @param[in] s  Serializer with which to serialize.
void Shader::Select::Serialize( Serializer& s )
{
    s << name;

    uint32_t typeFlags = shaderTypeFlags;
    s << typeFlags;
    shaderTypeFlags = typeFlags;

    bool bOptionalValue = bOptional;
    s << bOptionalValue;
    bOptional = ( bOptionalValue ? 1 : 0 );

    s << Serializer::WrapDynArray( choices );
}

/// Serialize this select option name/choice pair.
///
/// @param[in] s  Serializer with which to serialize.
void Shader::SelectPair::Serialize( Serializer& s )
{
    s << L_TAGGED( name );
    s << L_TAGGED( choice );
}

/// Serialize this set of shader preprocessor options.
///
/// @param[in] s  Serializer with which to serialize.
void Shader::Options::Serialize( Serializer& s )
{
    s << Serializer::WrapStructDynArray( m_toggles );
    s << Serializer::WrapStructDynArray( m_selects );
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
                const DynArray< Name >& rShaderSelectChoices = rShaderSelect.choices;
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
                const DynArray< Name >& rShaderSelectChoices = rShaderSelect.choices;
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
    DynArray< Name >& rToggleNames,
    DynArray< SelectPair >& rSelectPairs ) const
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

        const DynArray< Name >& rShaderSelectChoices = rShaderSelect.choices;
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

/// Serialize this set of persistent shader resource data.
///
/// @param[in] s  Serializer with which to serialize.
void Shader::PersistentResourceData::Serialize( Serializer& s )
{
    s << Serializer::WrapStruct( m_systemOptions );
    s << Serializer::WrapStruct( m_userOptions );
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

/// @copydoc GameObject::PreDestroy()
void ShaderVariant::PreDestroy()
{
    HELIUM_ASSERT( !m_pRenderResourceLoadBuffer );

    m_renderResources.Clear();

    Base::PreDestroy();
}

/// @copydoc GameObject::NeedsPrecacheResourceData()
bool ShaderVariant::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc GameObject::BeginPrecacheResourceData()
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
    tchar_t shaderTypeCharacter = ( *variantName )[ 0 ];
    if( shaderTypeCharacter != TXT( 'v' ) && shaderTypeCharacter != TXT( 'p' ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
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
                TRACE_ERROR,
                ( TXT( "ShaderVariant::BeginPrecacheResourceData(): Could not find resource sub-data %" ) TPRIuSZ
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
                TRACE_ERROR,
                ( TXT( "ShaderVariant::BeginPrecacheResourceData(): Failed to begin asynchronous load of " )
                TXT( "resource sub-data %" ) TPRIuSZ TXT( " of shader variant \"%s\".\n" ) ),
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

/// @copydoc GameObject::TryFinishPrecacheResourceData()
bool ShaderVariant::TryFinishPrecacheResourceData()
{
    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    // Get the type of shader being loaded.
    Name variantName = GetName();
    tchar_t shaderTypeCharacter = ( *variantName )[ 0 ];

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

        // Serialize the constant buffer information.
        BinaryDeserializer deserializer;
        deserializer.Prepare( rLoadData.pData, rLoadData.size );
        deserializer.BeginSerialize();
        m_constantBufferSets[ loadRequestIndex ].Serialize( deserializer );
        m_samplerInputSets[ loadRequestIndex ].Serialize( deserializer );
        m_textureInputSets[ loadRequestIndex ].Serialize( deserializer );
        size_t dataOffset = deserializer.GetCurrentOffset();
        deserializer.EndSerialize();

        // Apply padding for shader code alignment.
        dataOffset = Min( Align( dataOffset, sizeof( uint32_t ) ), rLoadData.size );

        // Create the shader resource.
        size_t shaderSize = rLoadData.size - dataOffset;
        const void* pShaderData = static_cast< const uint8_t* >( rLoadData.pData ) + dataOffset;

        RShaderPtr spShaderBase;
        if( shaderType == RShader::TYPE_VERTEX )
        {
            spShaderBase = pRenderer->CreateVertexShader( shaderSize, pShaderData );
        }
        else
        {
            spShaderBase = pRenderer->CreatePixelShader( shaderSize, pShaderData );
        }

        if( !spShaderBase )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "ShaderVariant::TryFinishPrecacheResourceData(): Failed to create shader for sub-data %" )
                TPRIuSZ TXT( " of shader \"%s\".\n" ) ),
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

/// @copydoc Resource::SerializePersistentResourceData()
void ShaderVariant::SerializePersistentResourceData( Serializer& s )
{
    size_t existingRenderResourceCount = m_renderResources.GetSize();
    HELIUM_ASSERT( existingRenderResourceCount <= UINT32_MAX );

    uint32_t renderResourceCount = static_cast< uint32_t >( existingRenderResourceCount );
    s << renderResourceCount;

    if( s.GetMode() == Serializer::MODE_LOAD )
    {
        // Reserve space for the resource data that will be loaded later.
        for( size_t resourceIndex = 0; resourceIndex < existingRenderResourceCount; ++resourceIndex )
        {
            m_renderResources[ resourceIndex ].Release();
        }

        m_renderResources.Reserve( renderResourceCount );
        m_renderResources.Resize( renderResourceCount );
        m_renderResources.Trim();

        m_constantBufferSets.Reserve( renderResourceCount );
        m_constantBufferSets.Resize( renderResourceCount );
        m_constantBufferSets.Trim();

        m_samplerInputSets.Reserve( renderResourceCount );
        m_samplerInputSets.Resize( renderResourceCount );
        m_samplerInputSets.Trim();

        m_textureInputSets.Reserve( renderResourceCount );
        m_textureInputSets.Resize( renderResourceCount );
        m_textureInputSets.Trim();
    }
}

/// @copydoc Resource::GetCacheName()
Name ShaderVariant::GetCacheName() const
{
    static Name cacheName( TXT( "Shader" ) );

    return cacheName;
}
