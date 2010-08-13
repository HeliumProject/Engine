#pragma once

#include <map>

#include "Core/API.h"
#include "Core/Render/Mesh.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace Core
    {
        namespace Render
        {
            enum
            {
                PARSE_FILE_OK     = 0,
                PARSE_FILE_FIXUP  = 1,      //file was fixed up during load
                PARSE_FILE_FAILED = 2,      //file failed to load
            };

            class CORE_API ObjectLoader : public Helium::RefCountBase<ObjectLoader>
            {
            public:
                ObjectLoader();
                virtual ~ObjectLoader();

                // returns of the above error codes, error code is also put in m_parse_error
                virtual u32 ParseFile( const tchar* filename,bool winding=false ) = 0;
                virtual u32 GetNumFragments( int bangleIndex = -1 );

                void Compile(bool flip=false);
                void ComputeTangents();
                void ComputeNormals();
                void ComputeBoundingBox( D3DXVECTOR3& min, D3DXVECTOR3& max );
                void SetNoTangents();
                void InsertColors();
                void InsertTexCoords();
                void Rescale( float radius, D3DXVECTOR3& min, D3DXVECTOR3& max );

                std::vector<float>  m_positions;
                std::vector<float>  m_normals;
                std::vector<float>  m_texcoords;
                std::vector<float>  m_sTangents;
                std::vector<float>  m_colors;
                std::map<int, bool> m_bangleInfo;

                std::vector<ShaderFrag> m_fragments;
                std::vector<float>  m_vertices;

                int                 m_tcSize;
                int                 m_posSize;

                int m_pOffset;
                int m_nOffset;
                int m_tcOffset;
                int m_sTanOffset;
                int m_cOffset;
                int m_vtxSize;

                u32 m_parse_error;
                u32 m_parse_warnings;
            };

            typedef Helium::SmartPtr<ObjectLoader> ObjectLoaderPtr;
        }
    }
}