#include "Precompile.h"
#include "PreprocessingPc/PcPreprocessor.h"

#include "Foundation/FilePath.h"
#include "Foundation/FileStream.h"
#include "Foundation/StringConverter.h"
#include "Rendering/ShaderProfiles.h"
#include "Graphics/Shader.h"
#include "Engine/FileLocations.h"

#if HELIUM_DIRECT3D

#include "Platform/SystemWin.h"

#pragma warning( push )
#pragma warning( disable:4005 )
#include <d3d9.h>
#include <d3d11.h>
#include <d3dx9.h>
#include <d3dcompiler.h>
#include <d3d10_1.h>
#pragma warning( pop )

#endif

using namespace Helium;

#if HELIUM_DIRECT3D

/// Include handler to use when compiling Direct3D HLSL shaders.
class D3DIncludeHandler : public ID3D10Include
{
public:
    /// @name Construction/Destruction
    //@{
    explicit D3DIncludeHandler( const FilePath& rShaderPath );
    virtual ~D3DIncludeHandler();
    //@}

    /// @name ID3D10Include Interface
    //@{
    STDMETHOD( Open )( D3D10_INCLUDE_TYPE includeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes );
    STDMETHOD( Close )( LPCVOID pData );
    //@}

private:
    /// Directory containing the shader file being processed.
    FilePath m_shaderDirectory;
};

/// Constructor.
///
/// @param[in] rShaderPath  FilePath to the shader file being processed.
D3DIncludeHandler::D3DIncludeHandler( const FilePath& rShaderPath )
{
    m_shaderDirectory.Set( rShaderPath.Directory().Get() );
}

/// Destructor.
D3DIncludeHandler::~D3DIncludeHandler()
{
}

/// Open a shader include file.
///
/// @param[in]  includeType  Location of the include file.
/// @param[in]  pFileName    Name of the include file.
/// @param[in]  pParentData  Pointer to the container that includes the include file.
/// @param[out] ppData       Pointer to the returned buffer that contains the include directives.  This pointer
///                          remains valid until Close() is called.
/// @param[out] pBytes       Number of bytes returned in ppData.
///
/// @return  S_OK if the include file was loaded successfully, an error code if not.
///
/// @see Close()
HRESULT D3DIncludeHandler::Open(
                                D3D10_INCLUDE_TYPE /*includeType*/,
                                LPCSTR pFileName,
                                LPCVOID /*pParentData*/,
                                LPCVOID* ppData,
                                UINT* pBytes )
{
    HELIUM_ASSERT( pFileName );
    HELIUM_ASSERT( ppData );
    HELIUM_ASSERT( pBytes );

    // Build the path to the file to include.
    String fileName;
    StringConverter< char, char >::Convert( fileName, pFileName );

    FilePath includePath( m_shaderDirectory + fileName.GetData() );

    // Attempt to open and read the contents of the include file.
    FileStream* pIncludeFileStream = FileStream::OpenFileStream( includePath.Data(), FileStream::MODE_READ );
    if( !pIncludeFileStream )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3DIncludeHandler::Open(): Failed to open include file \"%s\" for reading.\n",
            includePath.Data() );

        return E_FAIL;
    }

    int64_t fileSizeActual = pIncludeFileStream->GetSize();
    HELIUM_ASSERT( fileSizeActual >= 0 );
    if( fileSizeActual > UINT32_MAX )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3DIncludeHandler::Open(): Include file \"%s\" is larger than 4 GB and cannot be read.\n",
            includePath.Data() );

        delete pIncludeFileStream;

        return E_FAIL;
    }

    uint32_t fileSize = static_cast< uint32_t >( fileSizeActual );

    DefaultAllocator allocator;
    void* pBuffer = allocator.Allocate( fileSize );
    if( !pBuffer )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3DIncludeHandler::Open(): Failed to allocate %" PRIu32 " bytes for loading include file \"%s\".\n",
            fileSize,
            includePath.Data() );

        delete pIncludeFileStream;

        return E_FAIL;
    }

    size_t bytesRead = pIncludeFileStream->Read( pBuffer, 1, fileSize );
    if( bytesRead != fileSize )
    {
        HELIUM_TRACE(
            TraceLevels::Warning,
            "D3DIncludeHandler::Open(): Include file \"%s\" claimed to be %" PRIu32 " bytes, but only %" PRIuSZ " bytes could be read.\n",
            includePath.Data(),
            fileSize,
            bytesRead );

        fileSize = static_cast< uint32_t >( bytesRead );
    }

    delete pIncludeFileStream;

    *ppData = pBuffer;
    *pBytes = fileSize;

    return S_OK;
}

/// Close a shader include file previously opened using Open().
///
/// @param[in] pData  Pointer to the buffer previously returned by Open() that contains the include directives.
///
/// @return  S_OK if the file was closed successfully, an error code if not.
///
/// @see Open()
HRESULT D3DIncludeHandler::Close( LPCVOID pData )
{
    HELIUM_ASSERT( pData );

    DefaultAllocator().Free( const_cast< void* >( pData ) );

    return S_OK;
}

#endif // HELIUM_DIRECT3D

/// Constructor.
PcPreprocessor::PcPreprocessor()
{
}

/// Destructor.
PcPreprocessor::~PcPreprocessor()
{
}

/// @copydoc PlatformPreprocessor::GetByteOrder()
PlatformPreprocessor::EByteOrder PcPreprocessor::GetByteOrder() const
{
	return BYTE_ORDER_LITTLE;
}

/// @copydoc PlatformPreprocessor::GetShaderProfileCount()
size_t PcPreprocessor::GetShaderProfileCount() const
{
	return static_cast< size_t >( ShaderProfile::PC_MAX );
}

/// @copydoc PlatformPreprocessor::CompileShader()
bool PcPreprocessor::CompileShader(
								   const FilePath& rShaderPath,
								   size_t profileIndex,
								   RShader::EType type,
								   const void* pShaderCode,
								   size_t shaderCodeSize,
								   const ShaderToken* pTokens,
								   size_t tokenCount,
								   DynamicArray< uint8_t >& rCompiledCode,
								   DynamicArray< String >* pErrorMessages )
{
	HELIUM_ASSERT( profileIndex < static_cast< size_t >( ShaderProfile::PC_MAX ) );
	HELIUM_ASSERT( static_cast< size_t >( type ) < static_cast< size_t >( RShader::TYPE_MAX ) );
	HELIUM_ASSERT( pShaderCode );
	HELIUM_ASSERT( pTokens || tokenCount == 0 );

	rCompiledCode.Resize( 0 );
	if( pErrorMessages )
	{
		pErrorMessages->Resize( 0 );
	}

#if HELIUM_DIRECT3D

	DynamicArray< D3D10_SHADER_MACRO > defines;
	D3D10_SHADER_MACRO macro;

	const char* pProfile;

	switch( static_cast< ShaderProfile::EPc >( profileIndex ) )
	{
	case ShaderProfile::PC_SM2b:
		{
			macro.Name = "HELIUM_PROFILE_PC_SM2b";
			macro.Definition = "1";
			defines.Push( macro );

			// Also define HELIUM_PROFILE_PC_SM2 for consistency and legacy support.
			macro.Name = "HELIUM_PROFILE_PC_SM2";
			defines.Push( macro );

			pProfile = ( type == RShader::TYPE_VERTEX ? "vs_2_0" : "ps_2_b" );

			break;
		}

	case ShaderProfile::PC_SM3:
		{
			macro.Name = "HELIUM_PROFILE_PC_SM3";
			macro.Definition = "1";
			defines.Push( macro );

			pProfile = ( type == RShader::TYPE_VERTEX ? "vs_3_0" : "ps_3_0" );

			break;
		}

	case ShaderProfile::PC_SM4:
		{
			macro.Name = "HELIUM_PROFILE_PC_SM4";
			macro.Definition = "1";
			defines.Push( macro );

			pProfile = ( type == RShader::TYPE_VERTEX ? "vs_4_0" : "ps_4_0" );

			break;
		}

	default:
		{
			HELIUM_BREAK_MSG( "PcPreprocessor::CompileShader(): Invalid shader profile index.\n" );

			return false;
		}
	}

	switch( type )
	{
	case RShader::TYPE_VERTEX:
		{
			macro.Name = "HELIUM_TYPE_VERTEX";
			macro.Definition = "1";
			defines.Push( macro );

			break;
		}

	case RShader::TYPE_PIXEL:
		{
			macro.Name = "HELIUM_TYPE_PIXEL";
			macro.Definition = "1";
			defines.Push( macro );

			break;
		}

	default:
		{
			HELIUM_BREAK_MSG( "PcPreprocessor::CompileShader(): Invalid shader type.\n" );

			return false;
		}
	}

	StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
	StackMemoryHeap<>::Marker stackMarker( rStackHeap );

	for( size_t tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex )
	{
		const ShaderToken& rToken = pTokens[ tokenIndex ];

		size_t nameBufferSize = rToken.name.GetSize() + 1;
		char* pNameBuffer = static_cast< char* >( rStackHeap.Allocate( nameBufferSize ) );
		HELIUM_ASSERT( pNameBuffer );
		MemoryCopy( pNameBuffer, *rToken.name, nameBufferSize );
		macro.Name = pNameBuffer;

		size_t definitionBufferSize = rToken.definition.GetSize() + 1;
		char* pDefinitionBuffer = static_cast< char* >( rStackHeap.Allocate( definitionBufferSize ) );
		HELIUM_ASSERT( pDefinitionBuffer );
		MemoryCopy( pDefinitionBuffer, *rToken.definition, definitionBufferSize );
		macro.Definition = pDefinitionBuffer;
		
		HELIUM_TRACE(
			TraceLevels::Debug,
			"PcPreprocessor::CompileShader(): Defining option %s = %s (profile index: %" PRIuSZ ").\n",
			macro.Name,
			macro.Definition,
			profileIndex );

		defines.Push( macro );
	}

	macro.Name = NULL;
	macro.Definition = NULL;
	defines.Push( macro );

	D3DIncludeHandler includeHandler( rShaderPath );
	ID3D10Blob* pCompiledCodeBlob = NULL;
	ID3D10Blob* pErrorMessageBlob = NULL;
	// XXX TMC: Always use row-major packing, since that's the only option with Cg.
	HRESULT hResult = D3DCompile(
		pShaderCode,
		shaderCodeSize,
		NULL,
		defines.GetData(),
		&includeHandler,
		"main",
		pProfile,
		D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_WARNINGS_ARE_ERRORS,
		0,
		&pCompiledCodeBlob,
		( pErrorMessages ? &pErrorMessageBlob : NULL ) );

	stackMarker.Pop();

	if( pErrorMessageBlob )
	{
		HELIUM_ASSERT( pErrorMessages );

		const char* pErrorMessageData = static_cast< const char* >( pErrorMessageBlob->GetBufferPointer() );
		size_t errorMessageSize = pErrorMessageBlob->GetBufferSize();
		HELIUM_ASSERT( pErrorMessageData || errorMessageSize == 0 );

		CharString messageString;
		for( DWORD characterIndex = 0; characterIndex < errorMessageSize; ++characterIndex )
		{
			char character = *pErrorMessageData;
			++pErrorMessageData;

			if( character == '\n' || character == '\0' )
			{
				if( !messageString.IsEmpty() )
				{
					String* pErrorMessageString = pErrorMessages->New();
					HELIUM_ASSERT( pErrorMessageString );
					StringConverter< char, char >::Convert( *pErrorMessageString, messageString );

					messageString.Remove( 0, messageString.GetSize() );
				}
			}
			else
			{
				messageString.Add( character );
			}
		}

		if( !messageString.IsEmpty() )
		{
			String* pErrorMessageString = pErrorMessages->New();
			HELIUM_ASSERT( pErrorMessageString );
			StringConverter< char, char >::Convert( *pErrorMessageString, messageString );
		}

		pErrorMessageBlob->Release();
	}

	if( FAILED( hResult ) )
	{
		if( pCompiledCodeBlob )
		{
			pCompiledCodeBlob->Release();
		}

		return false;
	}

	HELIUM_ASSERT( pCompiledCodeBlob );

	const uint8_t* pShaderData = static_cast< const uint8_t* >( pCompiledCodeBlob->GetBufferPointer() );
	size_t shaderSize = pCompiledCodeBlob->GetBufferSize();
	HELIUM_ASSERT( pShaderData || shaderSize == 0 );

#if HELIUM_ENABLE_TRACE
	UINT instructionCountTotal = 0;
	UINT instructionCountAlu = 0;
	UINT instructionCountTexture = 0;

	if( profileIndex == ShaderProfile::PC_SM4 )
	{
		ID3D11ShaderReflection* pShaderReflect = NULL;
		D3DReflect(
			pShaderData,
			shaderSize,
			IID_ID3D11ShaderReflection,
			reinterpret_cast< void** >( &pShaderReflect ) );
		if( pShaderReflect )
		{
			D3D11_SHADER_DESC shaderDesc;
			if( SUCCEEDED( pShaderReflect->GetDesc( &shaderDesc ) ) )
			{
				// XXX TODO: See if there's a better accurate method for computing the ALU instruction count (or
				// perhaps some other instruction count value appropriate for basic performance evaluation).
				instructionCountTotal = shaderDesc.InstructionCount;
				instructionCountAlu = shaderDesc.FloatInstructionCount + shaderDesc.IntInstructionCount +
					shaderDesc.UintInstructionCount;
				instructionCountTexture = shaderDesc.TextureNormalInstructions +
					shaderDesc.TextureLoadInstructions + shaderDesc.TextureCompInstructions +
					shaderDesc.TextureBiasInstructions + shaderDesc.TextureGradientInstructions;
			}

			pShaderReflect->Release();
		}
	}
	else
	{
		ID3DXBuffer* pShaderDisassembly = NULL;
		D3DXDisassembleShader( reinterpret_cast< const DWORD* >( pShaderData ), FALSE, NULL, &pShaderDisassembly );
		if( pShaderDisassembly )
		{
			const char* pShaderCode = static_cast< const char* >( pShaderDisassembly->GetBufferPointer() );
			if( pShaderCode )
			{
				const char instructionCountStartMarker[] = "// approximately ";
				const char* pInstructionCountStart = strstr( pShaderCode, instructionCountStartMarker );
				if( pInstructionCountStart )
				{
					pInstructionCountStart += HELIUM_ARRAY_COUNT( instructionCountStartMarker ) - 1;

					int parseCount = StringScan(
						pInstructionCountStart,
						"%u instruction slots used (%u texture, %u arithmetic)",
						&instructionCountTotal,
						&instructionCountTexture,
						&instructionCountAlu );
					if( parseCount <= 1 )
					{
						instructionCountAlu = instructionCountTotal;
						instructionCountTexture = 0;
					}
				}
			}

			pShaderDisassembly->Release();
		}
	}

	String profileString;
	StringConverter< char, char >::Convert( profileString, pProfile );

	UINT instructionCountNonOther = instructionCountAlu + instructionCountTexture;
	UINT instructionCountOther =
		( instructionCountNonOther >= instructionCountTotal
		? 0
		: instructionCountTotal - instructionCountNonOther );

	HELIUM_TRACE(
		TraceLevels::Debug,
		"Compiled %s shader \"%s\" for profile %s (%" PRIuSZ " bytes; approximately %u instructions: %u ALU + %u texture + %u other).\n",
		type == RShader::TYPE_VERTEX ? "vertex" : "pixel",
		rShaderPath.Data(),
		*profileString,
		shaderSize,
		instructionCountTotal,
		instructionCountAlu,
		instructionCountTexture,
		instructionCountOther );
#endif

	rCompiledCode.Reserve( shaderSize );
	rCompiledCode.AddArray( pShaderData, shaderSize );

	pCompiledCodeBlob->Release();

#else // HELIUM_OPENGL

	HELIUM_ASSERT( false )

#endif // HELIUM_OPENGL

	return true;
}

/// @copydoc PlatformPreprocessor::FillShaderReflectionData()
bool PcPreprocessor::FillShaderReflectionData(
	size_t profileIndex,
	const void* pCompiledCode,
	size_t compiledCodeSize,
	DynamicArray< ShaderConstantBufferInfo >& rConstantBuffers,
	DynamicArray< ShaderSamplerInfo >& rSamplers,
	DynamicArray< ShaderTextureInfo >& rTextures )
{
	HELIUM_ASSERT( profileIndex < static_cast< size_t >( ShaderProfile::PC_MAX ) );
	HELIUM_ASSERT( pCompiledCode || compiledCodeSize == 0 );

	// Samplers and textures are always initialized from an empty set.
	rSamplers.Clear();
	rTextures.Clear();

	if( compiledCodeSize == 0 )
	{
		return true;
	}

#if HELIUM_DIRECT3D

	if( profileIndex >= ShaderProfile::PC_SM4 )
	{
		ID3D11ShaderReflection* pShaderReflect = NULL;
		HRESULT hResult = D3DReflect(
			pCompiledCode,
			compiledCodeSize,
			IID_ID3D11ShaderReflection,
			reinterpret_cast< void** >( &pShaderReflect ) );
		if( FAILED( hResult ) )
		{
			HELIUM_ASSERT( !pShaderReflect );

			HELIUM_TRACE(
				TraceLevels::Error,
				"PcPreprocessor::FillShaderReflectionData(): Failed to acquire reflection information for shader model 4 shader.\n" );

			rConstantBuffers.Clear();

			return false;
		}

		HELIUM_ASSERT( pShaderReflect );

		D3D11_SHADER_DESC shaderDesc;
		hResult = pShaderReflect->GetDesc( &shaderDesc );
		HELIUM_ASSERT( SUCCEEDED( hResult ) );

		String propertyName;

		// Get information about shader constant buffers.
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		D3D11_SHADER_VARIABLE_DESC variableDesc;

		size_t constantBufferCount = shaderDesc.ConstantBuffers;
		if( constantBufferCount > UINT16_MAX )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"PcPreprocessor::FillShaderReflectionData(): Constant buffer count (%" PRIuSZ ") exceeds the maximum supported size (%" PRIuSZ ") for shader model 4 shader.\n",
				constantBufferCount,
				static_cast< size_t >( UINT16_MAX ) );

			rConstantBuffers.Clear();

			return false;
		}

		rConstantBuffers.Reserve( constantBufferCount );
		rConstantBuffers.Resize( constantBufferCount );
		rConstantBuffers.Trim();

		for( size_t constantBufferIndex = 0; constantBufferIndex < constantBufferCount; ++constantBufferIndex )
		{
			ShaderConstantBufferInfo& rBufferInfo = rConstantBuffers[ constantBufferIndex ];
			rBufferInfo.index = static_cast< uint16_t >( constantBufferIndex );

			ID3D11ShaderReflectionConstantBuffer* pConstantBufferReflect = pShaderReflect->GetConstantBufferByIndex(
				static_cast< UINT >( constantBufferIndex ) );
			HELIUM_ASSERT( pConstantBufferReflect );

			hResult = pConstantBufferReflect->GetDesc( &bufferDesc );
			HELIUM_ASSERT( SUCCEEDED( hResult ) );

			StringConverter< char, char >::Convert( propertyName, bufferDesc.Name );
			rBufferInfo.name = Name( propertyName );

			UINT bufferSize = bufferDesc.Size;
			if( bufferSize > UINT16_MAX )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					"PcPreprocessor::FillShaderReflectionData(): Constant buffer \"%s\" exceeds the maximum supported size (size: %u; max: %" PRIu16 ").\n",
					*rBufferInfo.name,
					bufferSize,
					static_cast< uint16_t >( UINT16_MAX ) );

				rConstantBuffers.Clear();

				return false;
			}

			rBufferInfo.size = static_cast< uint16_t >( bufferSize );

			size_t constantCount = bufferDesc.Variables;

			DynamicArray< ShaderConstantInfo >& rConstants = rBufferInfo.constants;
			rConstants.Reserve( constantCount );
			rConstants.Resize( constantCount );
			rConstants.Trim();

			for( size_t constantIndex = 0; constantIndex < constantCount; ++constantIndex )
			{
				ShaderConstantInfo& rConstantInfo = rConstants[ constantIndex ];

				ID3D11ShaderReflectionVariable* pVariableReflect = pConstantBufferReflect->GetVariableByIndex(
					static_cast< UINT >( constantIndex ) );
				HELIUM_ASSERT( pVariableReflect );

				pVariableReflect->GetDesc( &variableDesc );
				HELIUM_ASSERT( SUCCEEDED( hResult ) );

				StringConverter< char, char >::Convert( propertyName, variableDesc.Name );
				rConstantInfo.name = Name( propertyName );

				UINT variableOffset = variableDesc.StartOffset;
				if( variableOffset > UINT16_MAX )
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						"PcPreprocessor::FillShaderReflectionData(): Constant \"%s\" of constant buffer \"%s\" offset exceeds the maximum supported range (offset: %u; max: %" PRIuSZ ").\n",
						*rConstantInfo.name,
						*rBufferInfo.name,
						variableOffset,
						static_cast< size_t >( UINT16_MAX ) );

					rConstantBuffers.Clear();

					return false;
				}

				UINT variableSize = variableDesc.Size;
				if( variableSize > UINT16_MAX )
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						"PcPreprocessor::FillShaderReflectionData(): Constant \"%s\" of constant buffer \"%s\" exceeds the maximum supported size (size: %u; max: %" PRIuSZ ").\n",
						*rConstantInfo.name,
						*rBufferInfo.name,
						variableSize,
						static_cast< size_t >( UINT16_MAX ) );

					rConstantBuffers.Clear();

					return false;
				}

				rConstantInfo.offset = static_cast< uint16_t >( variableOffset );
				rConstantInfo.size = static_cast< uint16_t >( variableSize );
				rConstantInfo.usedSize = static_cast< uint16_t >( variableSize );
			}
		}

		// Get information about shader sampler and texture bindings.
		D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;

		size_t boundResourceCount = shaderDesc.BoundResources;

		rSamplers.Reserve( boundResourceCount );
		rTextures.Reserve( boundResourceCount );

		for( size_t resourceIndex = 0; resourceIndex < boundResourceCount; ++resourceIndex )
		{
			hResult = pShaderReflect->GetResourceBindingDesc(
				static_cast< UINT >( resourceIndex ),
				&inputBindDesc );
			HELIUM_ASSERT( SUCCEEDED( hResult ) );

			switch( inputBindDesc.Type )
			{
			case D3D10_SIT_SAMPLER:
				{
					StringConverter< char, char >::Convert( propertyName, inputBindDesc.Name );

					if( inputBindDesc.BindPoint > UINT16_MAX )
					{
						HELIUM_TRACE(
							TraceLevels::Error,
							"PcPreprocessor::FillShaderConstantOffsets(): Bind point for sampler \"%s\" (%u) exceeds the maximum supported bind point index (%" PRIuSZ ") for shader model 4 shader.\n",
							*propertyName,
							inputBindDesc.BindPoint,
							static_cast< size_t >( UINT16_MAX ) );

						pShaderReflect->Release();

						rConstantBuffers.Clear();
						rSamplers.Clear();
						rTextures.Clear();

						return false;
					}

					ShaderSamplerInfo* pSamplerInfo = rSamplers.New();
					HELIUM_ASSERT( pSamplerInfo );
					pSamplerInfo->name = Name( propertyName );
					pSamplerInfo->bindIndex = static_cast< uint16_t >( inputBindDesc.BindPoint );

					break;
				}

			case D3D10_SIT_TEXTURE:
				{
					StringConverter< char, char >::Convert( propertyName, inputBindDesc.Name );

					if( inputBindDesc.BindPoint > UINT16_MAX )
					{
						HELIUM_TRACE(
							TraceLevels::Error,
							"PcPreprocessor::FillShaderConstantOffsets(): Bind point for texture \"%s\" (%u) exceeds the maximum supported bind point index (%" PRIuSZ ") for shader model 4 shader.\n",
							*propertyName,
							inputBindDesc.BindPoint,
							static_cast< size_t >( UINT16_MAX ) );

						pShaderReflect->Release();

						rConstantBuffers.Clear();
						rSamplers.Clear();
						rTextures.Clear();

						return false;
					}

					ShaderTextureInfo* pTextureInfo = rTextures.New();
					HELIUM_ASSERT( pTextureInfo );
					pTextureInfo->name = Name( propertyName );
					pTextureInfo->bindIndex = static_cast< uint16_t >( inputBindDesc.BindPoint );

					break;
				}
			}
		}

		rSamplers.Trim();
		rTextures.Trim();

		pShaderReflect->Release();

		return true;
	}

	// Get information about constants exposed in the shader.
	ID3DXConstantTable* pConstantTable = NULL;
	HRESULT hResult = D3DXGetShaderConstantTable( static_cast< const DWORD* >( pCompiledCode ), &pConstantTable );
	if( FAILED( hResult ) )
	{
		HELIUM_ASSERT( !pConstantTable );

		HELIUM_TRACE(
			TraceLevels::Error,
			"PcPreprocessor::FillShaderReflectionData(): Failed to retrieve constant table for shader (profile index: %" PRIuSZ ").\n",
			profileIndex );

		return false;
	}

	HELIUM_ASSERT( pConstantTable );

	bool bParseResult = true;
	UINT bufferOffset = 0;
	CharString propertyName;
	D3DXCONSTANT_DESC constantDesc;

	size_t previousNonEmptyBufferIndex;
	SetInvalid( previousNonEmptyBufferIndex );

	size_t bufferCount = rConstantBuffers.GetSize();
	for( size_t bufferIndex = 0; bufferIndex < bufferCount; ++bufferIndex )
	{
		ShaderConstantBufferInfo& rBufferInfo = rConstantBuffers[ bufferIndex ];

		DynamicArray< ShaderConstantInfo >& rConstants = rBufferInfo.constants;
		size_t constantCount = rConstants.GetSize();
		if( constantCount == 0 )
		{
			rBufferInfo.size = 0;

			continue;
		}

		for( size_t constantIndex = 0; constantIndex < constantCount; ++constantIndex )
		{
			ShaderConstantInfo& rConstantInfo = rConstants[ constantIndex ];

			StringConverter< char, char >::Convert( propertyName, *rConstantInfo.name );
			D3DXHANDLE hConstant = pConstantTable->GetConstantByName( NULL, *propertyName );
			if( !hConstant )
			{
				bParseResult = false;

				HELIUM_TRACE(
					TraceLevels::Error,
					"PcPreprocessor::FillShaderReflectionData(): Failed to retrieve handle for shader constant \"%s\" (profile index: %" PRIuSZ ").\n",
					*rConstantInfo.name,
					profileIndex );

				if( constantIndex == 0 )
				{
					// Intentionally breaking instead of continuing here, as none of the offsets of any constants
					// are going to be valid.
					break;
				}

				continue;
			}

			UINT constantDescCount = 1;
			hResult = pConstantTable->GetConstantDesc( hConstant, &constantDesc, &constantDescCount );
			HELIUM_ASSERT( SUCCEEDED( hResult ) );
			HELIUM_ASSERT( constantDescCount != 0 );

			if( constantIndex == 0 )
			{
				if( IsValid( previousNonEmptyBufferIndex ) )
				{
					UINT bufferOffsetNew = constantDesc.RegisterIndex;
					if( bufferOffsetNew > ( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ) )
					{
						bParseResult = false;

						HELIUM_TRACE(
							TraceLevels::Error,
							"PcPreprocessor::FillShaderReflectionData(): Register offset of the first constant in constant buffer \"%s\" exceeds the maximum supported offset (offset: %u; max: %" PRIuSZ ") (profile index: %" PRIuSZ ").\n",
							*rBufferInfo.name,
							bufferOffset,
							static_cast< size_t >( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ),
							profileIndex );

						// Intentionally breaking instead of continuing here, as none of the offsets of any
						// constants are going to be valid.
						break;
					}

					rConstantBuffers[ previousNonEmptyBufferIndex ].size =
						static_cast< uint16_t >( ( bufferOffsetNew - bufferOffset ) * ( sizeof( float32_t ) * 4 ) );

					bufferOffset = bufferOffsetNew;
				}
				else
				{
					bufferOffset = 0;
				}

				previousNonEmptyBufferIndex = bufferIndex;
			}

			UINT constantOffset = constantDesc.RegisterIndex;
			if( constantOffset < bufferOffset )
			{
				bParseResult = false;

				HELIUM_TRACE(
					TraceLevels::Error,
					"PcPreprocessor::FillShaderReflectionData(): Register offset of constant \"%s\" is less than the offset of constant buffer \"%s\" (constant offset: %u; buffer offset: %u) (profile index: %" PRIuSZ ").\n",
					*rConstantInfo.name,
					*rBufferInfo.name,
					constantOffset,
					bufferOffset,
					profileIndex );

				continue;
			}

			constantOffset -= bufferOffset;
			if( constantOffset > ( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ) )
			{
				bParseResult = false;

				HELIUM_TRACE(
					TraceLevels::Error,
					"PcPreprocessor::FillShaderReflectionData(): Register offset of constant \"%s\" within constant buffer \"%s\" exceeds the maximum supported offset (offset: %u; max: %" PRIuSZ ") (profile index: %" PRIuSZ ").\n",
					*rConstantInfo.name,
					*rBufferInfo.name,
					constantOffset,
					static_cast< size_t >( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ),
					profileIndex );

				continue;
			}

			UINT registerCount = constantDesc.RegisterCount;
			if( registerCount > ( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ) )
			{
				bParseResult = false;

				HELIUM_TRACE(
					TraceLevels::Error,
					"PcPreprocessor::FillShaderReflectionData(): Register count of constant \"%s\" exceeds the maximum supported count (count: %u; max: %" PRIuSZ ") (profile index: %" PRIuSZ ").\n",
					*rConstantInfo.name,
					registerCount,
					static_cast< size_t >( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ),
					profileIndex );

				continue;
			}

			if( constantOffset + registerCount > ( UINT16_MAX / ( sizeof( float32_t ) * 4 ) ) )
			{
				bParseResult = false;

				HELIUM_TRACE(
					TraceLevels::Error,
					"PcPreprocessor::FillShaderReflectionData(): Register range of constant \"%s\" exceeds the maximum supported range (count: %u; max: %" PRIuSZ ") (profile index: %" PRIuSZ ").\n",
					*rConstantInfo.name,
					registerCount,
					static_cast< size_t >( UINT16_MAX / ( sizeof( float32_t ) * 4 ) - constantOffset ),
					profileIndex );

				continue;
			}

			rConstantInfo.offset = static_cast< uint16_t >( constantOffset * ( sizeof( float32_t ) * 4 ) );
			rConstantInfo.size = static_cast< uint16_t >( registerCount * ( sizeof( float32_t ) * 4 ) );

			if( ( constantDesc.Class == D3DXPC_SCALAR || constantDesc.Class == D3DXPC_VECTOR ) &&
				constantDesc.Type == D3DXPT_FLOAT &&
				constantDesc.Rows == 1 )
			{
				rConstantInfo.usedSize = static_cast< uint16_t >( constantDesc.Columns * sizeof( float32_t ) );
				HELIUM_ASSERT( rConstantInfo.usedSize <= rConstantInfo.size );
			}
			else
			{
				rConstantInfo.usedSize = rConstantInfo.size;
			}
		}
	}

	if( IsValid( previousNonEmptyBufferIndex ) )
	{
		ShaderConstantBufferInfo& rBufferInfo = rConstantBuffers[ previousNonEmptyBufferIndex ];
		DynamicArray< ShaderConstantInfo >& rConstants = rBufferInfo.constants;
		HELIUM_ASSERT( !rConstants.IsEmpty() );
		ShaderConstantInfo& rConstantInfo = rConstants.GetLast();
		rBufferInfo.size = rConstantInfo.offset + rConstantInfo.size;
	}

	pConstantTable->Release();

	// Get information about the sampler and texture inputs exposed in the shader (Direct2D 9 and earlier have a
	// 1-to-1 correlation between samplers and textures, which the HLSL compiler for Direct3D 10 and later handles
	// by combining the sampler and texture names into a single sampler name).
	UINT samplerCount = 0;
	hResult = D3DXGetShaderSamplers( static_cast< const DWORD* >( pCompiledCode ), NULL, &samplerCount );
	if( FAILED( hResult ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"PcPreprocessor::FillShaderReflectionData(): Failed to retrieve texture sampler list shader (profile index: %" PRIuSZ ").\n",
			profileIndex );

		return false;
	}

	if( samplerCount != 0 )
	{
		if( samplerCount > UINT16_MAX )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				"PcPreprocessor::FillShaderReflectionData(): Sampler count count (%u) exceeds the maximum supported size (%" PRIuSZ ") for shader (profile index: %" PRIuSZ ").\n",
				samplerCount,
				static_cast< size_t >( UINT16_MAX ),
				profileIndex );

			return false;
		}

		DynamicArray< LPCSTR > samplerNames;
		samplerNames.Reserve( samplerCount );
		samplerNames.Resize( samplerCount );

		rSamplers.Reserve( samplerCount );
		rTextures.Reserve( samplerCount );

		LPCSTR* ppSamplerNames = samplerNames.GetData();
		HELIUM_ASSERT( ppSamplerNames );

		hResult = D3DXGetShaderSamplers(
			static_cast< const DWORD* >( pCompiledCode ),
			ppSamplerNames,
			&samplerCount );
		HELIUM_ASSERT( SUCCEEDED( hResult ) );

		String samplerName, textureName;
		for( UINT samplerIndex = 0; samplerIndex < samplerCount; ++samplerIndex )
		{
			LPCSTR pName = ppSamplerNames[ samplerIndex ];
			HELIUM_ASSERT( pName );
			StringConverter< char, char >::Convert( samplerName, pName );
			textureName = samplerName;

			// Separate the sampler state and texture names in the shader sampler string.
			size_t stateSeparatorIndex = samplerName.Find( '+' );
			if( IsValid( stateSeparatorIndex ) )
			{
				samplerName.Remove( stateSeparatorIndex, samplerName.GetSize() - stateSeparatorIndex );
				textureName.Remove( 0, stateSeparatorIndex + 1 );
			}

			ShaderSamplerInfo* pSamplerInfo = rSamplers.New();
			HELIUM_ASSERT( pSamplerInfo );
			pSamplerInfo->name = Name( samplerName );
			pSamplerInfo->bindIndex = static_cast< uint16_t >( samplerIndex );

			ShaderTextureInfo* pTextureInfo = rTextures.New();
			HELIUM_ASSERT( pTextureInfo );
			pTextureInfo->name = Name( textureName );
			pTextureInfo->bindIndex = static_cast< uint16_t >( samplerIndex );
		}
	}

	return bParseResult;

#else // HELIUM_DIRECT3D

	return HELIUM_VERIFY( false );

#endif // HELIUM_DIRECT3D
}
