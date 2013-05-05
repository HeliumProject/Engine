#include "GraphicsPch.h"
#include "Graphics/Material.h"

#include "Rendering/RConstantBuffer.h"
#include "Rendering/Renderer.h"
#include "Graphics/Texture.h"

#include "Reflect/DataDeduction.h"

HELIUM_IMPLEMENT_ASSET( Helium::Material, Graphics, AssetType::FLAG_ABSTRACT | AssetType::FLAG_NO_TEMPLATE );
REFLECT_DEFINE_BASE_STRUCTURE( Helium::Material::Float1Parameter );
REFLECT_DEFINE_BASE_STRUCTURE( Helium::Material::Float2Parameter );
REFLECT_DEFINE_BASE_STRUCTURE( Helium::Material::Float3Parameter );
REFLECT_DEFINE_BASE_STRUCTURE( Helium::Material::Float4Parameter );
REFLECT_DEFINE_BASE_STRUCTURE( Helium::Material::TextureParameter );
REFLECT_DEFINE_OBJECT( Helium::Material::PersistentResourceData );

using namespace Helium;

/// Constructor.
Material::Material()
{
    MemoryZero( m_persistentResourceData.m_shaderVariantIndices, sizeof( m_persistentResourceData.m_shaderVariantIndices ) );

    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_persistentResourceData.m_shaderVariantIndices ); ++shaderTypeIndex )
    {
        SetInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] );
        SetInvalid( m_constantBufferLoadIds[ shaderTypeIndex ] );
    }

#if HELIUM_TOOLS
    m_bLoadedOptions = false;
#endif
}

/// Destructor.
Material::~Material()
{
}


#if HELIUM_TOOLS
void Helium::Material::PreSerialize( const Reflect::Field* field )
{
    m_userOptions.Resize(0);

    Shader* pShader = m_spShader;
    if( pShader )
    {
        const Shader::Options& rUserOptions = pShader->GetUserOptions();

        DynamicArray< Name > enabledToggles;
        rUserOptions.GetOptionSetFromIndex(
            RShader::TYPE_FIRST,
            m_persistentResourceData.m_shaderVariantIndices[ 0 ],
            enabledToggles,
            m_userOptions );

        size_t enabledToggleCount = enabledToggles.GetSize();

        Shader::SelectPair optionPair;

        Name enabledChoice( TXT( "1" ) );
        Name disabledChoice( TXT( "0" ) );

        const DynamicArray< Shader::Toggle >& rUserToggles = rUserOptions.GetToggles();
        size_t userToggleCount = rUserToggles.GetSize();
        for( size_t userToggleIndex = 0; userToggleIndex < userToggleCount; ++userToggleIndex )
        {
            optionPair.name = rUserToggles[ userToggleIndex ].name;

            size_t enabledToggleIndex;
            for( enabledToggleIndex = 0; enabledToggleIndex < enabledToggleCount; ++enabledToggleIndex )
            {
                if( enabledToggles[ enabledToggleIndex ] == optionPair.name )
                {
                    break;
                }
            }

            optionPair.choice =
                ( enabledToggleIndex < enabledToggleCount ? enabledChoice : disabledChoice );

            m_userOptions.Push( optionPair );
        }
    }
}

void Helium::Material::PostDeserialize( const Reflect::Field* field )
{
    m_bLoadedOptions = true;
//     m_spShader = Reflect::SafeCast<ShaderPtr>(m_spShaderAsAsset);
// 
//     if (m_spShaderAsAsset && !m_spShader)
//     {
//         HELIUM_TRACE(
//             TraceLevels::Warning,
//             TXT( "Material::PostDeserialize(): Shader object \"%s\" is not of the type Shader.\n" ),
//             m_spShaderAsAsset->GetPath().ToString() );
//     }
}
#endif



void Material::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Material::m_spShader,           TXT( "m_spShader" ) );
    comp.AddField( &Material::m_textureParameters,  TXT( "m_textureParameters" ) );

#if HELIUM_TOOLS
    comp.AddField( &Material::m_userOptions,        TXT( "m_userOptions" ) );
    comp.AddField( &Material::m_float1Parameters,   TXT( "m_float1Parameters" ) );
    comp.AddField( &Material::m_float2Parameters,   TXT( "m_float2Parameters" ) );
    comp.AddField( &Material::m_float3Parameters,   TXT( "m_float3Parameters" ) );
    comp.AddField( &Material::m_float4Parameters,   TXT( "m_float4Parameters" ) );
#endif
}

//
///// @copydoc Asset::Serialize()
//void Material::Serialize( Serializer& s )
//{
//    HELIUM_SERIALIZE_BASE( s );
//
//    s << HELIUM_TAGGED( m_spShader );
//
//#if HELIUM_TOOLS
//    if( s.CanResolveTags() )
//    {
//        m_userOptions.Resize( 0 );
//
//        Serializer::EMode serializerMode = s.GetMode();
//        bool bSaving = ( serializerMode == Serializer::MODE_SAVE );
//        bool bLoading = ( serializerMode == Serializer::MODE_LOAD );
//
//        if( bSaving )
//        {
//            // Aggregate all user options from the resource information into the single "m_userOptions" array.
//            Shader* pShader = m_spShader;
//            if( pShader )
//            {
//                const Shader::Options& rUserOptions = pShader->GetUserOptions();
//
//                DynamicArray< Name > enabledToggles;
//                rUserOptions.GetOptionSetFromIndex(
//                    RShader::TYPE_FIRST,
//                    m_shaderVariantIndices[ 0 ],
//                    enabledToggles,
//                    m_userOptions );
//
//                size_t enabledToggleCount = enabledToggles.GetSize();
//
//                Shader::SelectPair optionPair;
//
//                Name enabledChoice( TXT( "1" ) );
//                Name disabledChoice( TXT( "0" ) );
//
//                const DynamicArray< Shader::Toggle >& rUserToggles = rUserOptions.GetToggles();
//                size_t userToggleCount = rUserToggles.GetSize();
//                for( size_t userToggleIndex = 0; userToggleIndex < userToggleCount; ++userToggleIndex )
//                {
//                    optionPair.name = rUserToggles[ userToggleIndex ].name;
//
//                    size_t enabledToggleIndex;
//                    for( enabledToggleIndex = 0; enabledToggleIndex < enabledToggleCount; ++enabledToggleIndex )
//                    {
//                        if( enabledToggles[ enabledToggleIndex ] == optionPair.name )
//                        {
//                            break;
//                        }
//                    }
//
//                    optionPair.choice =
//                        ( enabledToggleIndex < enabledToggleCount ? enabledChoice : disabledChoice );
//
//                    m_userOptions.Push( optionPair );
//                }
//            }
//        }
//        else if( bLoading )
//        {
//            DynamicArray< String > propertyTagNames;
//            s.GetPropertyTagNames( propertyTagNames );
//
//            Shader::SelectPair optionPair;
//            optionPair.choice.Clear();
//
//            size_t propertyTagCount = propertyTagNames.GetSize();
//            for( size_t tagIndex = 0; tagIndex < propertyTagCount; ++tagIndex )
//            {
//                const String& rTagName = propertyTagNames[ tagIndex ];
//                if( !rTagName.Contains( TXT( '.' ) ) )
//                {
//                    optionPair.name.Set( rTagName );
//                    m_userOptions.Push( optionPair );
//                }
//            }
//        }
//
//        s.PushPropertyFlags( Serializer::FLAG_EDITOR_ONLY );
//
//        size_t userOptionCount = m_userOptions.GetSize();
//        for( size_t optionIndex = 0; optionIndex < userOptionCount; ++optionIndex )
//        {
//            Shader::SelectPair& rOptionPair = m_userOptions[ optionIndex ];
//            s << Serializer::Tag( *rOptionPair.name ) << rOptionPair.choice;
//        }
//
//        // XXX TMC TODO: Replace with flexible name resolution support (a la m_userOptions above).
//        s << HELIUM_TAGGED_STRUCT_DYNARRAY( m_float1Parameters );
//        s << HELIUM_TAGGED_STRUCT_DYNARRAY( m_float2Parameters );
//        s << HELIUM_TAGGED_STRUCT_DYNARRAY( m_float3Parameters );
//        s << HELIUM_TAGGED_STRUCT_DYNARRAY( m_float4Parameters );
//
//        s.PopPropertyFlags();
//
//        if( bSaving )
//        {
//            m_userOptions.Clear();
//        }
//        else if( bLoading )
//        {
//            m_bLoadedOptions = true;
//        }
//    }
//#endif
//
//    // XXX TMC TODO: Replace with flexible name resolution support (a la m_userOptions above).
//    s << HELIUM_TAGGED_STRUCT_DYNARRAY( m_textureParameters );
//}

/// @copydoc Asset::NeedsPrecacheResourceData()
bool Material::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc Asset::BeginPrecacheResourceData()
bool Material::BeginPrecacheResourceData()
{
#if HELIUM_TOOLS
    // Convert shader options to variant indices if we just loaded a set of options.
    if( m_bLoadedOptions )
    {
        MemoryZero( m_persistentResourceData.m_shaderVariantIndices, sizeof( m_persistentResourceData.m_shaderVariantIndices ) );

        Shader* pShader = m_spShader;
        if( pShader )
        {
            const Shader::Options& rUserOptions = pShader->GetUserOptions();

            for( size_t shaderTypeIndex = 0;
                shaderTypeIndex < HELIUM_ARRAY_COUNT( m_persistentResourceData.m_shaderVariantIndices );
                ++shaderTypeIndex )
            {
                m_persistentResourceData.m_shaderVariantIndices[ shaderTypeIndex ] = static_cast< uint32_t >( rUserOptions.GetOptionSetIndex(
                    static_cast< RShader::EType >( shaderTypeIndex ),
                    m_userOptions.GetData(),
                    m_userOptions.GetSize() ) );
            }
        }

        m_userOptions.Clear();

        m_bLoadedOptions = false;
    }
#endif

    // Preload shader variant resources.
    Shader* pShader = m_spShader;
    if( !pShader )
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
        {
            HELIUM_ASSERT( IsInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] ) );
            m_shaderVariants[ shaderTypeIndex ].Release();
        }
    }
    else
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
        {
            HELIUM_ASSERT( IsInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] ) );
            m_shaderVariantLoadIds[ shaderTypeIndex ] = pShader->BeginLoadVariant(
                static_cast< RShader::EType >( shaderTypeIndex ),
                m_persistentResourceData.m_shaderVariantIndices[ shaderTypeIndex ] );
        }
    }

    // Preload the constant buffers for shader parameters.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( pRenderer )
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_constantBuffers ); ++shaderTypeIndex )
        {
            HELIUM_ASSERT( !m_constantBuffers[ shaderTypeIndex ] );
            HELIUM_ASSERT( IsInvalid( m_constantBufferLoadIds[ shaderTypeIndex ] ) );

            size_t bufferSize = GetSubDataSize( static_cast< uint32_t >( shaderTypeIndex ) );
            HELIUM_ASSERT( IsValid(bufferSize) );
            if( bufferSize == 0 )
            {
                continue;
            }

            RConstantBufferPtr spBuffer = pRenderer->CreateConstantBuffer(
                bufferSize,
                RENDERER_BUFFER_USAGE_STATIC );
            if( !spBuffer )
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    ( TXT( "Material::BeginPrecacheResourceData(): Failed to allocate constant buffer of %" )
                    TPRIuSZ TXT( " bytes for parameter usage for material \"%s\".\n" ) ),
                    bufferSize,
                    *GetPath().ToString() );

                continue;
            }

            void* pBufferData = spBuffer->Map();
            if( !pBufferData )
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    ( TXT( "Material::BeginPrecacheResourceData(): Failed to map constant buffer data for " )
                    TXT( "material \"%s\".\n" ) ),
                    *GetPath().ToString() );

                continue;
            }

            size_t loadId = BeginLoadSubData( pBufferData, static_cast< uint32_t >( shaderTypeIndex ) );
            if( IsInvalid( loadId ) )
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    ( TXT( "Material::BeginPrecacheResourceData(): Failed to begin loading resource sub-data %" )
                    TPRIuSZ TXT( " for material \"%s\".\n" ) ),
                    shaderTypeIndex,
                    *GetPath().ToString() );

                spBuffer->Unmap();

                continue;
            }

            m_constantBuffers[ shaderTypeIndex ] = spBuffer;
            m_constantBufferLoadIds[ shaderTypeIndex ] = loadId;
        }
    }

    return true;
}

/// @copydoc Asset::TryFinishPrecacheResourceData()
bool Material::TryFinishPrecacheResourceData()
{
    Shader* pShader = m_spShader;
    if( pShader )
    {
        for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
        {
            size_t loadId = m_shaderVariantLoadIds[ shaderTypeIndex ];
            if( IsValid( loadId ) &&
                !pShader->TryFinishLoadVariant( loadId, m_shaderVariants[ shaderTypeIndex ] ) )
            {
                return false;
            }

            SetInvalid( m_shaderVariantLoadIds[ shaderTypeIndex ] );
        }
    }

    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_constantBuffers ); ++shaderTypeIndex )
    {
        size_t loadId = m_constantBufferLoadIds[ shaderTypeIndex ];
        if( IsInvalid( loadId ) )
        {
            continue;
        }

        if( !TryFinishLoadSubData( loadId ) )
        {
            return false;
        }

        RConstantBuffer* pBuffer = m_constantBuffers[ shaderTypeIndex ];
        HELIUM_ASSERT( pBuffer );
        pBuffer->Unmap();

        SetInvalid( m_constantBufferLoadIds[ shaderTypeIndex ] );
    }

#if HELIUM_TOOLS
    // Synchronize shader constant parameters with those exposed by the shader variant resources.
    SynchronizeShaderParameters();
#endif

    return true;
}

/// @copydoc Resource::SerializePersistentResourceData()
// void Material::SerializePersistentResourceData( Serializer& s )
// {
//     s << Serializer::WrapArray( m_persistentResourceData.m_shaderVariantIndices );
// }

bool Helium::Material::LoadPersistentResourceObject( Reflect::ObjectPtr &_object )
{
    HELIUM_ASSERT(_object.ReferencesObject());
    if (!_object.ReferencesObject())
    {
        return false;
    }

    _object->CopyTo(&m_persistentResourceData);

    return true;
}

/// @copydoc Resource::GetCacheName()
Name Material::GetCacheName() const
{
    static Name cacheName( TXT( "Material" ) );

    return cacheName;
}

/// Get the name of the constant buffer used to set material parameters.
///
/// @return  Material parameter constant buffer name.
Name Material::GetParameterConstantBufferName()
{
    static Name parameterConstantBufferName( TXT( "MaterialParameters" ) );

    return parameterConstantBufferName;
}

#if HELIUM_TOOLS
/// Synchronize the shader parameter list with those provided by the selected shader variant.
///
/// @see SynchronizeFloatVectorParameters(), SynchronizeTextureParameters()
void Material::SynchronizeShaderParameters()
{
    Shader* pShader = m_spShader;
    if( !pShader )
    {
        m_float1Parameters.Clear();
        m_float2Parameters.Clear();
        m_float3Parameters.Clear();
        m_float4Parameters.Clear();
        m_textureParameters.Clear();
    }

    // Synchronize floating-point constant parameters.
    Name parameterConstantBufferName = GetParameterConstantBufferName();

    size_t existingFloat1Count = m_float1Parameters.GetSize();
    size_t existingFloat2Count = m_float2Parameters.GetSize();
    size_t existingFloat3Count = m_float3Parameters.GetSize();
    size_t existingFloat4Count = m_float4Parameters.GetSize();

    DynamicArray< Float1Parameter > newFloat1Parameters;
    DynamicArray< Float2Parameter > newFloat2Parameters;
    DynamicArray< Float3Parameter > newFloat3Parameters;
    DynamicArray< Float4Parameter > newFloat4Parameters;
    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
    {
        ShaderVariant* pShaderVariant = m_shaderVariants[ shaderTypeIndex ];
        if( !pShaderVariant )
        {
            continue;
        }

        const ShaderConstantBufferInfoSet* pBufferSet = pShaderVariant->GetConstantBufferInfoSet( 0 );
        if( !pBufferSet )
        {
            continue;
        }

        bool bCheckDuplicates =
            ( !newFloat1Parameters.IsEmpty() || !newFloat2Parameters.IsEmpty() || !newFloat3Parameters.IsEmpty() ||
            !newFloat4Parameters.IsEmpty() );

        const DynamicArray< ShaderConstantBufferInfo >& rBuffers = pBufferSet->buffers;
        size_t bufferCount = rBuffers.GetSize();
        for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
        {
            const ShaderConstantBufferInfo& rBufferInfo = rBuffers[ bufferIndex ];
            if( rBufferInfo.name != parameterConstantBufferName )
            {
                continue;
            }

            const DynamicArray< ShaderConstantInfo >& rConstants = rBufferInfo.constants;
            size_t constantCount = rConstants.GetSize();
            for( size_t constantIndex = 0; constantIndex < constantCount; ++constantIndex )
            {
                const ShaderConstantInfo& rConstantInfo = rConstants[ constantIndex ];

                // Constants must be between 1 and 4 floating-point values.
                uint16_t constantSize = rConstantInfo.usedSize;
                if( constantSize < sizeof( float32_t ) || constantSize > sizeof( float32_t ) * 4 )
                {
                    continue;
                }

                Name constantName = rConstantInfo.name;

                size_t parameterIndex;
                if( bCheckDuplicates )
                {
                    size_t parameterCount = newFloat1Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat1Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }

                    parameterCount = newFloat2Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat2Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }

                    parameterCount = newFloat3Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat3Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }

                    parameterCount = newFloat4Parameters.GetSize();
                    for( parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex )
                    {
                        if( newFloat4Parameters[ parameterIndex ].name == constantName )
                        {
                            break;
                        }
                    }

                    if( parameterIndex < parameterCount )
                    {
                        continue;
                    }
                }

                Simd::Vector4 newValue( 0.0f );
                for( parameterIndex = 0; parameterIndex < existingFloat1Count; ++parameterIndex )
                {
                    const Float1Parameter& rExistingParameter = m_float1Parameters[ parameterIndex ];
                    if( rExistingParameter.name == constantName )
                    {
                        newValue.SetElement( 0, rExistingParameter.value );

                        break;
                    }
                }

                if( parameterIndex >= existingFloat1Count )
                {
                    for( parameterIndex = 0; parameterIndex < existingFloat2Count; ++parameterIndex )
                    {
                        const Float2Parameter& rExistingParameter = m_float2Parameters[ parameterIndex ];
                        if( rExistingParameter.name == constantName )
                        {
                            newValue.SetElement( 0, rExistingParameter.value.GetX() );
                            newValue.SetElement( 1, rExistingParameter.value.GetY() );

                            break;
                        }
                    }

                    if( parameterIndex >= existingFloat2Count )
                    {
                        for( parameterIndex = 0; parameterIndex < existingFloat3Count; ++parameterIndex )
                        {
                            const Float3Parameter& rExistingParameter = m_float3Parameters[ parameterIndex ];
                            if( rExistingParameter.name == constantName )
                            {
                                newValue.SetElement( 0, rExistingParameter.value.GetElement( 0 ) );
                                newValue.SetElement( 1, rExistingParameter.value.GetElement( 1 ) );
                                newValue.SetElement( 2, rExistingParameter.value.GetElement( 2 ) );

                                break;
                            }
                        }

                        if( parameterIndex >= existingFloat3Count )
                        {
                            for( parameterIndex = 0; parameterIndex < existingFloat4Count; ++parameterIndex )
                            {
                                const Float4Parameter& rExistingParameter =
                                    m_float4Parameters[ parameterIndex ];
                                if( rExistingParameter.name == constantName )
                                {
                                    newValue = rExistingParameter.value;

                                    break;
                                }
                            }
                        }
                    }
                }

                if( constantSize < sizeof( float32_t ) * 2 )
                {
                    Float1Parameter* pParameter = newFloat1Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value = newValue.GetElement( 0 );
                }
                else if( constantSize < sizeof( float32_t ) * 3 )
                {
                    Float2Parameter* pParameter = newFloat2Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value = Simd::Vector2( newValue.GetElement( 0 ), newValue.GetElement( 1 ) );
                }
                else if( constantSize < sizeof( float32_t ) * 4 )
                {
                    Float3Parameter* pParameter = newFloat3Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value =
                        Simd::Vector3( newValue.GetElement( 0 ), newValue.GetElement( 1 ), newValue.GetElement( 2 ) );
                }
                else
                {
                    Float4Parameter* pParameter = newFloat4Parameters.New();
                    HELIUM_ASSERT( pParameter );
                    pParameter->name = constantName;
                    pParameter->value = newValue;
                }
            }
        }
    }

    newFloat1Parameters.Trim();
    newFloat2Parameters.Trim();
    newFloat3Parameters.Trim();
    newFloat4Parameters.Trim();
    m_float1Parameters.Swap( newFloat1Parameters );
    m_float2Parameters.Swap( newFloat2Parameters );
    m_float3Parameters.Swap( newFloat3Parameters );
    m_float4Parameters.Swap( newFloat4Parameters );
    newFloat1Parameters.Clear();
    newFloat2Parameters.Clear();
    newFloat3Parameters.Clear();
    newFloat4Parameters.Clear();

    // Synchronize texture parameters.
    size_t existingTextureCount = m_textureParameters.GetSize();

    DynamicArray< TextureParameter > newTextureParameters;
    for( size_t shaderTypeIndex = 0; shaderTypeIndex < HELIUM_ARRAY_COUNT( m_shaderVariants ); ++shaderTypeIndex )
    {
        ShaderVariant* pShaderVariant = m_shaderVariants[ shaderTypeIndex ];
        if( !pShaderVariant )
        {
            continue;
        }

        const ShaderTextureInfoSet* pTextureSet = pShaderVariant->GetTextureInfoSet( 0 );
        if( !pTextureSet )
        {
            continue;
        }

        bool bCheckDuplicates = !newTextureParameters.IsEmpty();

        const DynamicArray< ShaderTextureInfo >& rTextureInputs = pTextureSet->inputs;
        size_t textureInputCount = rTextureInputs.GetSize();
        for( size_t textureIndex = 0; textureIndex < textureInputCount; ++textureIndex )
        {
            const ShaderTextureInfo& rTextureInfo = rTextureInputs[ textureIndex ];

            // Ignore textures prefixed with an underscore, as they are reserved for system use.
            Name textureInputName = rTextureInfo.name;
            if( !textureInputName.IsEmpty() && ( *textureInputName )[ 0 ] == TXT( '_' ) )
            {
                continue;
            }

            size_t parameterIndex;
            if( bCheckDuplicates )
            {
                size_t textureParameterCount = newTextureParameters.GetSize();
                for( parameterIndex = 0; parameterIndex < textureParameterCount; ++parameterIndex )
                {
                    if( newTextureParameters[ parameterIndex ].name == textureInputName )
                    {
                        break;
                    }
                }

                if( parameterIndex < textureParameterCount )
                {
                    continue;
                }
            }

            TextureParameter* pParameter = newTextureParameters.New();
            HELIUM_ASSERT( pParameter );
            pParameter->name = textureInputName;

            for( parameterIndex = 0; parameterIndex < existingTextureCount; ++parameterIndex )
            {
                const TextureParameter& rTextureParameter = m_textureParameters[ parameterIndex ];
                if( rTextureParameter.name == textureInputName )
                {
                    pParameter->value = rTextureParameter.value;

                    break;
                }
            }
        }
    }

    newTextureParameters.Trim();
    m_textureParameters.Swap( newTextureParameters );
    newTextureParameters.Clear();
}
#endif  // HELIUM_TOOLS

void Material::Float1Parameter::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Material::Float1Parameter::name,           TXT( "name" ) );
    comp.AddField( &Material::Float1Parameter::value,          TXT( "value" ) );
}

void Material::Float2Parameter::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Material::Float2Parameter::name,           TXT( "name" ) );
    comp.AddField( &Material::Float2Parameter::value,          TXT( "value" ) );
}

void Material::Float3Parameter::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Material::Float3Parameter::name,           TXT( "name" ) );
    comp.AddField( &Material::Float3Parameter::value,          TXT( "value" ) );
}

void Material::Float4Parameter::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Material::Float4Parameter::name,           TXT( "name" ) );
    comp.AddField( &Material::Float4Parameter::value,          TXT( "value" ) );
}

void Material::TextureParameter::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &Material::TextureParameter::name,          TXT( "name" ) );
    comp.AddField( &Material::TextureParameter::value,         TXT( "value" ) );
}

void Material::PersistentResourceData::PopulateStructure( Reflect::Structure& comp )
{
    // If these trip, then this struct needs to be updated. Having to do this hack because reflect does not support serializing
    // c style arrays
#pragma TODO("Support static arrays in reflect")
    HELIUM_COMPILE_ASSERT(RShader::TYPE_MAX == 2);
    HELIUM_COMPILE_ASSERT(RShader::TYPE_VERTEX == 0);
    HELIUM_COMPILE_ASSERT(RShader::TYPE_PIXEL == 1);

    comp.AddField( &Material::PersistentResourceData::m_shaderVariantIndexVertex,        TXT( "m_shaderVariantIndexVertex" ) );
    comp.AddField( &Material::PersistentResourceData::m_shaderVariantIndexPixel,         TXT( "m_shaderVariantIndexPixel" ) );
}
