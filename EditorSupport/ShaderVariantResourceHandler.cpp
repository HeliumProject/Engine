#include "EditorSupportPch.h"

#if HELIUM_TOOLS

#include "EditorSupport/ShaderVariantResourceHandler.h"

#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Foundation/FileStream.h"
#include "Foundation/StringConverter.h"
#include "Engine/CacheManager.h"
#include "Engine/AssetLoader.h"
#include "Engine/PackageLoader.h"
#include "Rendering/ShaderProfiles.h"
#include "PcSupport/AssetPreprocessor.h"

HELIUM_IMPLEMENT_ASSET( Helium::ShaderVariantResourceHandler, EditorSupport, 0 );

using namespace Helium;

/// Constructor.
ShaderVariantResourceHandler::ShaderVariantResourceHandler()
: m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
{
	// Objects of this type should only be constructed in the editor, and only the template should exist, so
	// register ourself to override the shader variant load process.
	HELIUM_ASSERT( !Shader::GetBeginLoadVariantOverride() );
	HELIUM_ASSERT( !Shader::GetTryFinishLoadVariantOverride() );
	HELIUM_ASSERT( !Shader::GetVariantLoadOverrideData() );

	Shader::SetVariantLoadOverride( BeginLoadVariantCallback, TryFinishLoadVariantCallback, this );
}

/// Destructor.
ShaderVariantResourceHandler::~ShaderVariantResourceHandler()
{
}

/// @copydoc ResourceHandler::GetResourceType()
const AssetType* ShaderVariantResourceHandler::GetResourceType() const
{
	return ShaderVariant::GetStaticType();
}

/// @copydoc ResourceHandler::CacheResource()
bool ShaderVariantResourceHandler::CacheResource(
	AssetPreprocessor* pAssetPreprocessor,
	Resource* pResource,
	const String& rSourceFilePath )
{
	HELIUM_ASSERT( pAssetPreprocessor );
	HELIUM_ASSERT( pResource );

	ShaderVariant* pVariant = Reflect::AssertCast< ShaderVariant >( pResource );

	// Parse the shader type and user option index from the variant name.
	Name variantName = pVariant->GetName();
	const tchar_t* pVariantNameString = *variantName;
	HELIUM_ASSERT( pVariantNameString );

	tchar_t shaderTypeCharacter = pVariantNameString[ 0 ];
	HELIUM_ASSERT( shaderTypeCharacter != TXT( '\0' ) );

	RShader::EType shaderType;
	switch( shaderTypeCharacter )
	{
	case TXT( 'v' ):
		{
			shaderType = RShader::TYPE_VERTEX;
			break;
		}

	case TXT( 'p' ):
		{
			shaderType = RShader::TYPE_PIXEL;
			break;
		}

	default:
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "ShaderVariantResourceHandler: Failed to determine shader type from the name of object " )
				TXT( "\"%s\".\n" ) ),
				*pVariant->GetPath().ToString() );

			return false;
		}
	}

	uint32_t userOptionIndex = 0;
	++pVariantNameString;
	int parseResult;
#if HELIUM_WCHAR_T
#if HELIUM_CC_CL
	parseResult = swscanf_s( pVariantNameString, TXT( "%" ) SCNu32, &userOptionIndex );
#else
	parseResult = swscanf( pVariantNameString, TXT( "%" ) SCNu32, &userOptionIndex );
#endif
#else
#if HELIUM_CC_CL
	parseResult = sscanf_s( pVariantNameString, TXT( "%" ) SCNu32, &userOptionIndex );
#else
	parseResult = sscanf( pVariantNameString, TXT( "%" ) SCNu32, &userOptionIndex );
#endif
#endif
	if( parseResult != 1 )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "ShaderVariantResourceHandler: Failed to parse user shader option set index from the name of " )
			TXT( "option \"%s\".\n" ) ),
			*pVariant->GetPath().ToString() );

		return false;
	}

	// Get the parent shader.
	Shader* pShader = Reflect::AssertCast< Shader >( pVariant->GetOwner() );
	HELIUM_ASSERT( pShader );
	HELIUM_ASSERT( pShader->GetAnyFlagSet( Asset::FLAG_PRECACHED ) );

	// Acquire the user preprocessor option set associated with the target shader type and user option set index.
	const Shader::Options& rUserOptions = pShader->GetUserOptions();

	DynamicArray< Name > toggleNames;
	DynamicArray< Shader::SelectPair > selectPairs;
	rUserOptions.GetOptionSetFromIndex( shaderType, userOptionIndex, toggleNames, selectPairs );

	DynamicArray< PlatformPreprocessor::ShaderToken > shaderTokens;

	size_t userToggleNameCount = toggleNames.GetSize();
	for( size_t toggleNameIndex = 0; toggleNameIndex < userToggleNameCount; ++toggleNameIndex )
	{
		PlatformPreprocessor::ShaderToken* pToken = shaderTokens.New();
		HELIUM_ASSERT( pToken );
		StringConverter< tchar_t, char >::Convert( pToken->name, *toggleNames[ toggleNameIndex ] );
		pToken->definition = "1";
	}

	size_t userSelectPairCount = selectPairs.GetSize();
	for( size_t selectPairIndex = 0; selectPairIndex < userSelectPairCount; ++selectPairIndex )
	{
		const Shader::SelectPair& rPair = selectPairs[ selectPairIndex ];

		PlatformPreprocessor::ShaderToken* pToken = shaderTokens.New();
		HELIUM_ASSERT( pToken );
		StringConverter< tchar_t, char >::Convert( pToken->name, *rPair.name );
		pToken->definition = "1";

		pToken = shaderTokens.New();
		HELIUM_ASSERT( pToken );
		StringConverter< tchar_t, char >::Convert( pToken->name, *rPair.choice );
		pToken->definition = "1";
	}

	size_t userShaderTokenCount = shaderTokens.GetSize();

	// Load the entire shader resource into memory.
	FileStream* pSourceFileStream = FileStream::OpenFileStream( rSourceFilePath, FileStream::MODE_READ );
	if( !pSourceFileStream )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "ShaderVariantResourceHandler: Source file for shader variant resource \"%s\" failed to open " )
			TXT( "properly.\n" ) ),
			*pVariant->GetPath().ToString() );

		return false;
	}

	int64_t size64 = pSourceFileStream->GetSize();
	HELIUM_ASSERT( size64 != -1 );

	HELIUM_ASSERT( static_cast< uint64_t >( size64 ) <= static_cast< size_t >( -1 ) );
	if( size64 > static_cast< uint64_t >( static_cast< size_t >( -1 ) ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "ShaderVariantResourceHandler: Source file for shader resource \"%s\" is too large to fit " )
			TXT( "into memory for preprocessing.\n" ) ),
			*pShader->GetPath().ToString() );

		delete pSourceFileStream;

		return false;
	}

	size_t size = static_cast< size_t >( size64 );

	DefaultAllocator allocator;
	void* pShaderSource = allocator.Allocate( size );
	HELIUM_ASSERT( pShaderSource );
	if( !pShaderSource )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "ShaderVariantResourceHandler: Failed to allocate %" ) PRIuSZ TXT( " bytes for loading the " )
			TXT( "source data of \"%s\" for preprocessing.\n" ) ),
			size,
			*pShader->GetPath().ToString() );

		delete pSourceFileStream;

		return false;
	}

	BufferedStream( pSourceFileStream ).Read( pShaderSource, 1, size );

	delete pSourceFileStream;

	// Compile each variant of system options for each shader profile in each supported target platform.
	const Shader::Options& rSystemOptions = pShader->GetSystemOptions();
	size_t systemOptionSetCount = rSystemOptions.ComputeOptionSetCount( shaderType );
	if( systemOptionSetCount > UINT32_MAX )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "ShaderVariantResourceHandler: System option set count (%" ) PRIuSZ TXT( ") in shader \"%s\" " )
			TXT( "exceeds the maximum supported (%" ) PRIuSZ TXT( ").\n" ) ),
			systemOptionSetCount,
			*pShader->GetPath().ToString(),
			static_cast< size_t >( UINT32_MAX ) );

		allocator.Free( pShaderSource );

		return false;
	}

	uint32_t systemOptionSetCount32 = static_cast< uint32_t >( systemOptionSetCount );

	for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
	{
		PlatformPreprocessor* pPreprocessor = pAssetPreprocessor->GetPlatformPreprocessor(
			static_cast< Cache::EPlatform >( platformIndex ) );
		if( !pPreprocessor )
		{
			continue;
		}

		Resource::PreprocessedData& rPreprocessedData = pVariant->GetPreprocessedData(
			static_cast< Cache::EPlatform >( platformIndex ) );

		Helium::StrongPtr< ShaderVariant::PersistentResourceData > persistentResourceData(new ShaderVariant::PersistentResourceData());
		persistentResourceData->m_resourceCount = systemOptionSetCount32;
		SaveObjectToPersistentDataBuffer(persistentResourceData.Get(), rPreprocessedData.persistentDataBuffer);

		size_t shaderProfileCount = pPreprocessor->GetShaderProfileCount();
		size_t shaderCount = shaderProfileCount * systemOptionSetCount;

		DynamicArray< DynamicArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;
		rSubDataBuffers.Reserve( shaderCount );
		rSubDataBuffers.Resize( 0 );
		rSubDataBuffers.Resize( shaderCount );
		rSubDataBuffers.Trim();

		rPreprocessedData.bLoaded = true;
	}

//     DynamicArray< uint8_t > compiledCodeBuffer;
//     DynamicArray< ShaderConstantBufferInfo > constantBuffers, pcSm4ConstantBuffers;
//     DynamicArray< ShaderSamplerInfo > samplerInputs;
//     DynamicArray< ShaderTextureInfo > textureInputs;
	
	CompiledShaderData csd_pc_sm4;
	csd_pc_sm4.GetRefCountProxy()->AddStrongRef(); // stack allocated object!!

	for( size_t systemOptionSetIndex = 0; systemOptionSetIndex < systemOptionSetCount; ++systemOptionSetIndex )
	{
		rSystemOptions.GetOptionSetFromIndex( shaderType, systemOptionSetIndex, toggleNames, selectPairs );

		size_t systemToggleNameCount = toggleNames.GetSize();
		for( size_t toggleNameIndex = 0; toggleNameIndex < systemToggleNameCount; ++toggleNameIndex )
		{
			PlatformPreprocessor::ShaderToken* pToken = shaderTokens.New();
			HELIUM_ASSERT( pToken );
			StringConverter< tchar_t, char >::Convert( pToken->name, *toggleNames[ toggleNameIndex ] );
			pToken->definition = "1";
		}

		size_t systemSelectPairCount = selectPairs.GetSize();
		for( size_t selectPairIndex = 0; selectPairIndex < systemSelectPairCount; ++selectPairIndex )
		{
			const Shader::SelectPair& rPair = selectPairs[ selectPairIndex ];

			PlatformPreprocessor::ShaderToken* pToken = shaderTokens.New();
			HELIUM_ASSERT( pToken );
			StringConverter< tchar_t, char >::Convert( pToken->name, *rPair.name );
			pToken->definition = "1";

			pToken = shaderTokens.New();
			HELIUM_ASSERT( pToken );
			StringConverter< tchar_t, char >::Convert( pToken->name, *rPair.choice );
			pToken->definition = "1";
		}

		// Compile for PC shader model 4 first so that we can get the constant buffer information.
		PlatformPreprocessor* pPreprocessor = pAssetPreprocessor->GetPlatformPreprocessor( Cache::PLATFORM_PC );
		HELIUM_ASSERT( pPreprocessor );

		csd_pc_sm4.compiledCodeBuffer.Resize( 0 );
		bool bCompiled = CompileShader(
			pVariant,
			pPreprocessor,
			Cache::PLATFORM_PC,
			ShaderProfile::PC_SM4,
			shaderType,
			pShaderSource,
			size,
			shaderTokens,
			csd_pc_sm4.compiledCodeBuffer );
		if( !bCompiled )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "ShaderVariantResourceHandler: Failed to compile shader for PC shader model 4, which is " )
				TXT( "needed for reflection purposes.  Additional shader targets will not be built.\n" ) ) );
		}
		else
		{
			csd_pc_sm4.constantBuffers.Resize( 0 );
			csd_pc_sm4.samplerInputs.Resize( 0 );
			csd_pc_sm4.textureInputs.Resize( 0 );
			bool bReadConstantBuffers = pPreprocessor->FillShaderReflectionData(
				ShaderProfile::PC_SM4,
				csd_pc_sm4.compiledCodeBuffer.GetData(),
				csd_pc_sm4.compiledCodeBuffer.GetSize(),
				csd_pc_sm4.constantBuffers,
				csd_pc_sm4.samplerInputs,
				csd_pc_sm4.textureInputs );
			if( !bReadConstantBuffers )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "ShaderVariantResourceHandler: Failed to read reflection information for PC shader " )
					TXT( "model 4.  Additional shader targets will not be built.\n" ) ) );
			}
			else
			{
				Resource::PreprocessedData& rPcPreprocessedData = pVariant->GetPreprocessedData(
					Cache::PLATFORM_PC );
				DynamicArray< DynamicArray< uint8_t > >& rPcSubDataBuffers = rPcPreprocessedData.subDataBuffers;
				DynamicArray< uint8_t >& rPcSm4SubDataBuffer =
					rPcSubDataBuffers[ ShaderProfile::PC_SM4 * systemOptionSetCount + systemOptionSetIndex ];

				Cache::WriteCacheObjectToBuffer( &csd_pc_sm4, rPcSm4SubDataBuffer);
				
				// FOR EACH PLATFORM
				for( size_t platformIndex = 0;
					platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX );
					++platformIndex )
				{
					PlatformPreprocessor* pPreprocessor = pAssetPreprocessor->GetPlatformPreprocessor(
						static_cast< Cache::EPlatform >( platformIndex ) );
					if( !pPreprocessor )
					{
						continue;
					}
					
					// GET PLATFORM'S SUBDATA BUFFER
					Resource::PreprocessedData& rPreprocessedData = pVariant->GetPreprocessedData(
						static_cast< Cache::EPlatform >( platformIndex ) );
					DynamicArray< DynamicArray< uint8_t > >& rSubDataBuffers = rPreprocessedData.subDataBuffers;

					size_t shaderProfileCount = pPreprocessor->GetShaderProfileCount();
					for( size_t shaderProfileIndex = 0;
						shaderProfileIndex < shaderProfileCount;
						++shaderProfileIndex )
					{
						CompiledShaderData csd;
						csd.GetRefCountProxy()->AddStrongRef(); // stack allocated object!!

						// Already cached PC shader model 4...
						if( shaderProfileIndex == ShaderProfile::PC_SM4 && platformIndex == Cache::PLATFORM_PC )
						{
							continue;
						}

						bCompiled = CompileShader(
							pVariant,
							pPreprocessor,
							platformIndex,
							shaderProfileIndex,
							shaderType,
							pShaderSource,
							size,
							shaderTokens,
							csd.compiledCodeBuffer );
						if( !bCompiled )
						{
							continue;
						}

						csd.constantBuffers = csd_pc_sm4.constantBuffers;
						csd.samplerInputs.Resize( 0 );
						csd.textureInputs.Resize( 0 );
						bReadConstantBuffers = pPreprocessor->FillShaderReflectionData(
							shaderProfileIndex,
							csd.compiledCodeBuffer.GetData(),
							csd.compiledCodeBuffer.GetSize(),
							csd.constantBuffers,
							csd.samplerInputs,
							csd.textureInputs );
						if( !bReadConstantBuffers )
						{
							continue;
						}

						DynamicArray< uint8_t >& rTargetSubDataBuffer =
							rSubDataBuffers[ shaderProfileIndex * systemOptionSetCount + systemOptionSetIndex ];
						Cache::WriteCacheObjectToBuffer( &csd, rTargetSubDataBuffer);
					}
				}
			}
		}

		// Trim the system tokens off the shader token list for the next pass.
		shaderTokens.Resize( userShaderTokenCount );
	}

	allocator.Free( pShaderSource );

	return true;
}

/// Begin asynchronous loading of a shader variant.
///
/// @param[in] pShader          Parent shader resource.
/// @param[in] shaderType       Shader type.
/// @param[in] userOptionIndex  Index associated with the user option combination for the shader variant.
///
/// @return  ID associated with the load procedure, or an invalid index if the load could not be started.
///
/// @see TryFinishLoadVariant()
size_t ShaderVariantResourceHandler::BeginLoadVariant(
	Shader* pShader,
	RShader::EType shaderType,
	uint32_t userOptionIndex )
{
	HELIUM_ASSERT( pShader );
	HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

	// Attempt to locate an existing load request for the specified shader variant.
	LoadRequest* pLoadRequest = m_loadRequestPool.Allocate();
	HELIUM_ASSERT( pLoadRequest );
	pLoadRequest->shaderType = shaderType;
	pLoadRequest->userOptionIndex = userOptionIndex;
	HELIUM_ASSERT( !pLoadRequest->spVariant );
	pLoadRequest->requestCount = 1;

	LoadRequestSetType::ConstAccessor loadRequestConstAccessor;
	if( !m_loadRequestSet.Insert( loadRequestConstAccessor, pLoadRequest ) )
	{
		// Request exists, so increment its reference count.
		m_loadRequestPool.Release( pLoadRequest );

		pLoadRequest = *loadRequestConstAccessor;
		HELIUM_ASSERT( pLoadRequest );

		AtomicIncrementAcquire( pLoadRequest->requestCount );

		size_t loadId = m_loadRequestPool.GetIndex( pLoadRequest );
		HELIUM_ASSERT( IsValid( loadId ) );

		return loadId;
	}

	// Adding a new request, so create the variant object if it does not yet exist.
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
	variantNameString.Format( TXT( "%c%" ) PRIu32, shaderTypeCharacter, userOptionIndex );

	Name variantName( variantNameString );
	variantNameString.Clear();

	pLoadRequest->spVariant.Set( Reflect::AssertCast< ShaderVariant >( pShader->FindChild( variantName ) ) );
	if( !pLoadRequest->spVariant )
	{
		if( !Asset::Create< ShaderVariant >( pLoadRequest->spVariant, variantName, pShader ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "ShaderVariantResourceHandler::BeginLoadVariant(): Failed to create shader variant object " )
				  TXT( "\"%s:%s\".\n" ) ),
				*pShader->GetPath().ToString(),
				*variantName );
		}
		else
		{
			HELIUM_ASSERT( pLoadRequest->spVariant );
		}
	}

	// If we have an object for the shader variant, attempt to load its resource data.
	ShaderVariant* pVariant = pLoadRequest->spVariant;
	if( pVariant && !pVariant->GetAnyFlagSet( Asset::FLAG_PRECACHED ) )
	{
		AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetStaticInstance();
		HELIUM_ASSERT( pAssetPreprocessor );
		
		pAssetPreprocessor->LoadResourceData( pVariant );

		// Resource data loaded, so deserialize the persistent data for the current platform and begin precaching.
		CacheManager& rCacheManager = CacheManager::GetStaticInstance();
		const Resource::PreprocessedData& rPreprocessedData = pVariant->GetPreprocessedData(
			rCacheManager.GetCurrentPlatform() );
		const DynamicArray< uint8_t >& rPersistentDataBuffer = rPreprocessedData.persistentDataBuffer;

		Reflect::ObjectPtr persistent_resource_data = Cache::ReadCacheObjectFromBuffer(rPersistentDataBuffer);
		pVariant->LoadPersistentResourceObject(persistent_resource_data);
		pVariant->BeginPrecacheResourceData();
	}

	size_t loadId = m_loadRequestPool.GetIndex( pLoadRequest );
	HELIUM_ASSERT( IsValid( loadId ) );

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
bool ShaderVariantResourceHandler::TryFinishLoadVariant( size_t loadId, ShaderVariantPtr& rspVariant )
{
	HELIUM_ASSERT( IsValid( loadId ) );

	// Get the load request and acquire exclusive access to the request entry in the lookup hash set.
	LoadRequest* pLoadRequest = m_loadRequestPool.GetObject( loadId );
	HELIUM_ASSERT( pLoadRequest );

	LoadRequestSetType::Accessor loadRequestAccessor;
	HELIUM_VERIFY( m_loadRequestSet.Find( loadRequestAccessor, pLoadRequest ) );

	// Check if the load request has completed.
	ShaderVariant* pVariant = pLoadRequest->spVariant;
	if( pVariant && !pVariant->GetAnyFlagSet( Asset::FLAG_PRECACHED ) )
	{
		if( !pVariant->TryFinishPrecacheResourceData() )
		{
			return false;
		}

		pVariant->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED | Asset::FLAG_PRECACHED );
		pVariant->ConditionalFinalizeLoad();

		// Cache the shader data, but don't evict the raw resource data for the current platform.
		AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
		HELIUM_ASSERT( pAssetLoader );
		pAssetLoader->CacheObject( pVariant, false );
	}

	rspVariant = pVariant;

	// Decrement the request count and release the entry if the count has reached zero.
	int32_t newRequestCount = AtomicDecrementRelease( pLoadRequest->requestCount );
	if( newRequestCount == 0 )
	{
		m_loadRequestSet.Remove( loadRequestAccessor );

		pLoadRequest->spVariant.Release();
		m_loadRequestPool.Release( pLoadRequest );
	}

	return true;
}

/// Callback registered with the Shader class to override variant begin-load calls.
///
/// @param[in] pCallbackData    Pointer to the ShaderVariantResourceHandler instance registered as the callback data
///                             with the Shader class.
/// @param[in] pShader          Parent shader resource.
/// @param[in] shaderType       Shader type.
/// @param[in] userOptionIndex  Index associated with the user option combination for the shader variant.
///
/// @return  ID associated with the load procedure, or an invalid index if the load could not be started.
///
/// @see TryFinishLoadVariantCallback(), BeginLoadVariant(), TryFinishLoadVariant()
size_t ShaderVariantResourceHandler::BeginLoadVariantCallback(
	void* pCallbackData,
	Shader* pShader,
	RShader::EType shaderType,
	uint32_t userOptionIndex )
{
	ShaderVariantResourceHandler* pResourceHandler = static_cast< ShaderVariantResourceHandler* >( pCallbackData );
	HELIUM_ASSERT( pResourceHandler );

	size_t loadId = pResourceHandler->BeginLoadVariant( pShader, shaderType, userOptionIndex );

	return loadId;
}

/// Callback registered with the Shader class to override variant try-finish-load calls.
///
/// @param[in]  pCallbackData  Pointer to the ShaderVariantResourceHandler instance registered as the callback data
///                            with the Shader class.
/// @param[in]  loadId         Load request ID.
/// @param[out] rspVariant     Smart pointer set to the variant reference if loading has completed.
///
/// @return  True if loading has completed, false if not.  Note that if this returns true, the load request ID will
///          no longer be valid for its current load request and should not be reused.
///
/// @see BeginLoadVariantCallback(), TryFinishLoadVariant(), BeginLoadVariant()
bool ShaderVariantResourceHandler::TryFinishLoadVariantCallback(
	void* pCallbackData,
	size_t loadId,
	ShaderVariantPtr& rspVariant )
{
	ShaderVariantResourceHandler* pResourceHandler = static_cast< ShaderVariantResourceHandler* >( pCallbackData );
	HELIUM_ASSERT( pResourceHandler );

	bool bFinished = pResourceHandler->TryFinishLoadVariant( loadId, rspVariant );

	return bFinished;
}

/// Helper function for compiling a shader for a specific profile.
///
/// @param[in]  pVariant             Shader variant for which we are compiling.
/// @param[in]  pPreprocessor        Platform preprocessor to use for compiling.
/// @param[in]  platformIndex        Platform index.
/// @param[in]  shaderProfileIndex   Index of the target shader profile.
/// @param[in]  shaderType           Type of shader to compile.
/// @param[in]  pShaderSourceData    Buffer in which the shader source code is stored.
/// @param[in]  shaderSourceSize     Size of the shader source buffer, in bytes.
/// @param[in]  rTokens              Array specifying preprocessor tokens to pass to the shader compiler.
/// @param[out] rCompiledCodeBuffer  Buffer in which the compiled code will be stored.
///
/// @return  True if compiling was successful, false if not.
bool ShaderVariantResourceHandler::CompileShader(
	ShaderVariant* pVariant,
	PlatformPreprocessor* pPreprocessor,
	size_t platformIndex,
	size_t shaderProfileIndex,
	RShader::EType shaderType,
	const void* pShaderSourceData,
	size_t shaderSourceSize,
	const DynamicArray< PlatformPreprocessor::ShaderToken >& rTokens,
	DynamicArray< uint8_t >& rCompiledCodeBuffer )
{
	HELIUM_ASSERT( pVariant );
	HELIUM_ASSERT( pPreprocessor );
	HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );
	HELIUM_ASSERT( pShaderSourceData || shaderSourceSize == 0 );

	HELIUM_UNREF( platformIndex );  // Variable is used for logging only.

	rCompiledCodeBuffer.Resize( 0 );

#if HELIUM_ENABLE_TRACE
	DynamicArray< String > errorMessages;
#endif

	FilePath shaderFilePath;
	if ( !FileLocations::GetDataDirectory( shaderFilePath ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "ShaderVariantResourceHandler: Failed to obtain data directory." ) );

		return false;
	}

	shaderFilePath += pVariant->GetPath().GetParent().ToFilePathString().GetData();

	bool bCompileResult = pPreprocessor->CompileShader(
		shaderFilePath,
		shaderProfileIndex,
		shaderType,
		pShaderSourceData,
		shaderSourceSize,
		rTokens.GetData(),
		rTokens.GetSize(),
		rCompiledCodeBuffer
#if HELIUM_ENABLE_TRACE
		, &errorMessages
#else
		, NULL
#endif
		);
	if( !bCompileResult )
	{
		rCompiledCodeBuffer.Resize( 0 );

#if HELIUM_ENABLE_TRACE
		String tokenList;
#if HELIUM_WCHAR_T
		String convertedToken;
#endif
		size_t tokenCount = rTokens.GetSize();
		for( size_t tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex )
		{
			tokenList += TXT( ' ' );
#if HELIUM_WCHAR_T
			StringConverter< char, tchar_t >::Convert( convertedToken, rTokens[ tokenIndex ].name );
			tokenList += convertedToken;
#else
			tokenList += rTokens[ tokenIndex ].name;
#endif
		}

		size_t errorCount = errorMessages.GetSize();

		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "ShaderVariantResourceHandler: Failed to compile \"%s\" for platform %" ) PRIuSZ
			TXT( ", profile %" ) PRIuSZ TXT( "; %" ) PRIuSZ TXT( " errors (tokens:%s):\n" ) ),
			*pVariant->GetPath().ToString(),
			platformIndex,
			shaderProfileIndex,
			errorCount,
			*tokenList );

		for( size_t errorIndex = 0; errorIndex < errorCount; ++errorIndex )
		{
			HELIUM_TRACE( TraceLevels::Error, TXT( "- %s\n" ), *errorMessages[ errorIndex ] );
		}
#endif  // HELIUM_ENABLE_TRACE
	}

	return bCompileResult;
}

/// Compute a hash value for a shader variant load request.
///
/// @param[in] pRequest  Load request.
///
/// @return  Hash value.
size_t ShaderVariantResourceHandler::LoadRequestHash::operator()( const LoadRequest* pRequest ) const
{
	HELIUM_ASSERT( pRequest );

	return ( static_cast< size_t >( pRequest->userOptionIndex ) * RShader::TYPE_MAX + pRequest->shaderType );
}

/// Test whether the two given load requests are for the same shader variant.
///
/// @param[in] pRequest0  Load request.
/// @param[in] pRequest1  Another load request.
///
/// @return  True if the requests are for the same shader variant, false if not.
bool ShaderVariantResourceHandler::LoadRequestEquals::operator()(
	const LoadRequest* pRequest0,
	const LoadRequest* pRequest1 ) const
{
	HELIUM_ASSERT( pRequest0 );
	HELIUM_ASSERT( pRequest1 );

	return ( pRequest0->userOptionIndex == pRequest1->userOptionIndex &&
		pRequest0->shaderType == pRequest1->shaderType );
}

#endif  // HELIUM_TOOLS
