#include "Precompile.h"
#include "RenderingGL/GLVertexDescription.h"

using namespace Helium;

/// Constructor
GLVertexDescription::GLVertexDescription()
: m_pDescription( NULL )
, m_elementCount( 0 )
{}

/// Destructor.
GLVertexDescription::~GLVertexDescription()
{
	if( m_pDescription )
	{
		delete [] m_pDescription;
		m_pDescription = NULL;
	}
	m_elementCount = 0;
}

/// Initialize this state object.
///
/// @param[in] pElements  State description array.
/// @param[in] elementCount  Number of elements in description array.
///
/// @return  True if initialization was successful, false if not.
bool GLVertexDescription::Initialize( const RVertexDescription::Element* pElements, size_t elementCount )
{
	HELIUM_ASSERT( pElements );
	HELIUM_ASSERT( elementCount != 0 );
	if( !pElements || elementCount == 0 )
	{
		return false;
	}

	// Allocate memory for our vertex description array.
	size_t descriptionArraySize = elementCount * sizeof( GLVertexDescription::DescriptionElement );
	GLVertexDescription::DescriptionElement* pDescription = new DescriptionElement[ descriptionArraySize ];
	HELIUM_ASSERT( pDescription );
	if( !pDescription )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLVertexDescription::Initialize(): Failed to allocate %" PRIuSZ " bytes for vertex description data.\n",
			descriptionArraySize );
		return false;
	}
	m_elementCount = elementCount;
	m_pDescription = pDescription;

	// Define lookup tables.
	static const GLchar* vertexAttribNames[ RENDERER_VERTEX_SEMANTIC_MAX ] =
	{
		"position",      // RENDERER_VERTEX_SEMANTIC_POSITION
		"blendweight",   // RENDERER_VERTEX_SEMANTIC_BLENDWEIGHT
		"blendindices",  // RENDERER_VERTEX_SEMANTIC_BLENDINDICES
		"normal",        // RENDERER_VERTEX_SEMANTIC_NORMAL
		"psize",         // RENDERER_VERTEX_SEMANTIC_PSIZE
		"texcoord",      // RENDERER_VERTEX_SEMANTIC_TEXCOORD
		"tangent",       // RENDERER_VERTEX_SEMANTIC_TANGENT
		"binormal",      // RENDERER_VERTEX_SEMANTIC_BINORMAL
		"color"          // RENDERER_VERTEX_SEMANTIC_COLOR
	};
	static const GLint vertexAttribSizes[ RENDERER_VERTEX_DATA_TYPE_MAX ][ 2 ] =
	{
		// { Count per vertex, size per element }
		{ 2, sizeof( GLfloat ) },     // RENDERER_VERTEX_DATA_TYPE_FLOAT32_2
		{ 3, sizeof( GLfloat ) },     // RENDERER_VERTEX_DATA_TYPE_FLOAT32_3
		{ 4, sizeof( GLfloat ) },     // RENDERER_VERTEX_DATA_TYPE_FLOAT32_4
		{ 4, sizeof( GLubyte ) },     // RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM
		{ 4, sizeof( GLubyte ) },     // RENDERER_VERTEX_DATA_TYPE_UINT8_4
		{ 2, sizeof( GLfloat ) / 2 }, // RENDERER_VERTEX_DATA_TYPE_FLOAT16_2
		{ 4, sizeof( GLfloat ) / 2 }  // RENDERER_VERTEX_DATA_TYPE_FLOAT16_4
	};
	static const GLenum vertexAttribTypes[ RENDERER_VERTEX_DATA_TYPE_MAX ] =
	{
		GL_FLOAT,         // RENDERER_VERTEX_DATA_TYPE_FLOAT32_2
		GL_FLOAT,         // RENDERER_VERTEX_DATA_TYPE_FLOAT32_3
		GL_FLOAT,         // RENDERER_VERTEX_DATA_TYPE_FLOAT32_4
		GL_UNSIGNED_BYTE, // RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM
		GL_UNSIGNED_BYTE, // RENDERER_VERTEX_DATA_TYPE_UINT8_4
		GL_HALF_FLOAT,    // RENDERER_VERTEX_DATA_TYPE_FLOAT16_2
		GL_HALF_FLOAT     // RENDERER_VERTEX_DATA_TYPE_FLOAT16_4
	};
	static const GLboolean vertexAttribNormalized[ RENDERER_VERTEX_DATA_TYPE_MAX ] =
	{
		GL_FALSE, // RENDERER_VERTEX_DATA_TYPE_FLOAT32_2
		GL_FALSE, // RENDERER_VERTEX_DATA_TYPE_FLOAT32_3
		GL_FALSE, // RENDERER_VERTEX_DATA_TYPE_FLOAT32_4
		GL_TRUE,  // RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM
		GL_FALSE, // RENDERER_VERTEX_DATA_TYPE_UINT8_4
		GL_FALSE, // RENDERER_VERTEX_DATA_TYPE_FLOAT16_2
		GL_FALSE  // RENDERER_VERTEX_DATA_TYPE_FLOAT16_4
	};

	GLsizei stride = 0;
	for( size_t elementIndex = 0; elementIndex < elementCount; ++elementIndex )
	{
		const RVertexDescription::Element& rElement = pElements[ elementIndex ];
		GLVertexDescription::DescriptionElement& rDescriptionElement = pDescription[ elementIndex ];

		// Range check arguments.
		HELIUM_ASSERT( static_cast< size_t >( rElement.type ) < static_cast< size_t >( RENDERER_VERTEX_DATA_TYPE_MAX ) );
		HELIUM_ASSERT( static_cast< size_t >( rElement.semantic ) < static_cast< size_t >( RENDERER_VERTEX_SEMANTIC_MAX ) );
		if( (static_cast< size_t >( rElement.type ) >= static_cast< size_t >( RENDERER_VERTEX_DATA_TYPE_MAX ) ) ||
			(static_cast< size_t >( rElement.type ) >= static_cast< size_t >( RENDERER_VERTEX_DATA_TYPE_MAX ) ) )
		{
			return false;
		}

		/// Internalize vertex attribute description.
		rDescriptionElement.name = vertexAttribNames[ rElement.semantic ];
		rDescriptionElement.size = vertexAttribSizes[ rElement.type ][ 0 ];
		rDescriptionElement.type = vertexAttribTypes[ rElement.type ];
		rDescriptionElement.isNormalized = vertexAttribNormalized[ rElement.type ];

		// Calculate vertex attribute stride.
		const GLsizei attribSizeBytes = rDescriptionElement.size * vertexAttribSizes[ rElement.type ][ 1 ];
		stride += attribSizeBytes;
		rDescriptionElement.stride = stride;
	}

	return true;
}
