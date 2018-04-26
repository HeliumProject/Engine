#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RVertexDescription.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL vertex description.
	class GLVertexDescription : public RVertexDescription
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLVertexDescription();
		//@}

		struct DescriptionElement
		{
			/// Vertex attribute name
			const GLchar* name;
			/// Vertex attribute size
			GLint size;
			/// Vertex attribute type
			GLenum type;
			/// Vertex attribute normalized flag
			GLboolean isNormalized;
			/// Vertex attribute stride
			GLsizei stride;

			/// @name Construction/Destruction
			//@{
			inline DescriptionElement();
			//@}
		};

		/// Collection of vertex attribute description elements.
		DescriptionElement* m_pDescription;
		/// Number of elements in description.
		size_t m_elementCount;

		/// @name Initialization
		//@{
		bool Initialize( const RVertexDescription::Element* pElements, size_t elementCount );
		//@}

	private:

		/// @name Construction/Destruction
		//@{
		~GLVertexDescription();
		//@}
	};
}

#include "RenderingGL/GLVertexDescription.inl"
