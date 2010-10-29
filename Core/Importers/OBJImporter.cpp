#include "Core/Importers/OBJImporter.h"

#include "Core/SceneGraph/Mesh.h"
#include "Core/SceneGraph/Shader.h"

#include "Platform/String.h"

#include <map>

#define BUF_SIZE 1024

using namespace Helium;

struct Triangle
{
    std::vector< uint32_t >  m_VertIndices;
    std::vector< uint32_t >  m_NormalIndices;
    std::vector< uint32_t >  m_TexCoordIndices;
    uint32_t                 m_ShaderIndex;
};

/* ReadMTL: read a wavefront material library file
*
* name  - name of the material library
*/
static SceneGraph::Shader* ReadMTL( const Path& path )
{
    FILE* file;
    char buf[ BUF_SIZE ];

    file = _tfopen( path.c_str(), TXT( "r" ) );
    if ( !file )
    {
        Log::Error( TXT( "ReadMTL() failed: can't open material file \"%s\"." ), path.c_str() );
        return NULL;
    }

    SceneGraph::Shader* shader = NULL;

    /* count the number of materials in the file */
    while( fscanf( file, "%s", buf ) != EOF )
    {
        switch( buf[0] )
        {
        case '#': /* comment */
            {
                /* eat up rest of line */
                fgets( buf, sizeof( buf ), file );
                break;
            }

        case 'n': /* newmtl */
            {
                if( strncmp( buf, "newmtl", 6 ) != 0 )
                {
                    Log::Error( TXT( "ReadMTL: Got \"%s\" instead of \"newmtl\" in file \"%s\"" ), buf, path.c_str() );
                }


                fgets( buf, sizeof( buf ), file );
                sscanf( buf, "%s %s", buf, buf );

                shader = new SceneGraph::Shader();

                break;
            }

        case 'N':
            {
                switch(buf[1])
                {
                case 's':
                    {
                        float shininess;
                        fscanf(file, "%f", &shininess );
                        Log::Warning( TXT( "Discarding shininess, unsupported." ) );
                        break;
                    }

                default:
                    {
                        Log::Warning( TXT( "ReadMTL: Command \"%s\" ignored") , buf );
                        fgets(buf, sizeof(buf), file);
                        break;
                    }
                }
                break;
            }

        case 'K':
            {
                switch(buf[1])
                {
                case 'a':
                    {
                        Math::Color3 ambient;
                        fscanf( file, "%f %f %f", &ambient.r, &ambient.g, &ambient.b );
                        Log::Warning( TXT( "Discarding ambient, unsupported" ) );
                        break;
                    }

                case 'd':
                    {
                        Math::Color3 diffuse;
                        fscanf( file, "%f %f %f", &diffuse.r, &diffuse.g, &diffuse.b );
                        Log::Warning( TXT( "Discarding diffuse, unsupported" ) );
                        break;
                    }

                case 's':
                    {
                        Math::Color3 specular;
                        fscanf( file, "%f %f %f", &specular.r, &specular.g, &specular.b );
                        Log::Warning( TXT( "Discarding specular, unsupported" ) );
                        break;
                    }

                default:
                    {
                        Log::Warning( TXT( "ReadMTL: Command \"%s\" ignored" ), buf );
                        fgets( buf, sizeof( buf ), file );
                        break;
                    }
                }
                break;
            }

        case 'd': /* d = Dissolve factor (pseudo-transparency). Values are from 0-1. 0 is completely transparent, 1 is opaque. */
            {
                float alpha;
                fscanf( file, "%f", &alpha );
                Log::Warning( TXT( "Discarding alpha, unsupported" ) );
                break;
            }

        case 'm': /* texture map */
            {
                char filenameBuf[ FILENAME_MAX ];
                fgets( filenameBuf, FILENAME_MAX, file );
                tstring temp;
                Helium::ConvertString( filenameBuf, temp );

                if( strncmp( buf, "map_Kd", 6 ) == 0 )
                {
                    HELIUM_ASSERT( shader );
                    shader->m_AssetPath.Set( temp );
                }
                else
                {
                    Log::Warning( TXT( "map %s %s ignored" ), buf, path.c_str() );
                    fgets( buf, sizeof( buf ), file );
                }
                break;
            }

        default: /* eat up rest of line */
            {
                fgets( buf, sizeof( buf ), file );
                break;
            }
        }
    }

    fclose( file );

    return shader;
}

SceneGraph::Mesh* Importers::ImportOBJ( const Path& path, bool flipWinding )
{
    FILE* file;

    /* open the file */
    file = _tfopen( path.c_str(), TXT( "r" ) );
    if ( !file )
    {
        Log::Error( TXT( "ImportOBJ() failed: can't open data file \"%s\"." ), path.c_str() );
        return NULL;
    }

    char buf[ BUF_SIZE ];

    SceneGraph::Mesh* mesh = new SceneGraph::Mesh();
    std::map< tstring, SceneGraph::Shader* > shaders;
    SceneGraph::Shader* curShader = NULL;
    uint32_t curShaderIndex = 0xffffffff;

    uint32_t match = 0;

    Math::Vector3 vertex;
    Math::Vector3 normal;
    Math::Vector2 texCoord;

    std::vector< Math::Vector3 > vertices;
    std::vector< Math::Vector3 > normals;
    std::vector< Math::Vector2 > texCoords;

    std::vector< Triangle > triangles;

    while ( 1 ) 
    {
        int fpos = ftell(file);

        if (fscanf( file, "%s", buf) == EOF)
            break;

        int currpos=ftell(file);

        switch (buf[0]) 
        {
        case '#':
            //comment line, eat the remainder
            fgets( buf, sizeof( buf ), file );
            break;

        case 'v':
            switch (buf[1]) 
            {          
            case '\0':
                //vertex, 3 or 4 components
                float discard;
                match = fscanf( file, "%f %f %f %f", &vertex.x, &vertex.y, &vertex.z, &discard );
                if ( match==3 || match==4 )
                {
                    vertices.push_back( vertex );
                }
                else
                {
                    vertices.push_back( Math::Vector3() );
                    Log::Warning( TXT( "Invalid position, setting to origin. (File: %s, pos: %d, curpos: %d)" ), path.c_str(), fpos, currpos );
                }
                break;

            case 'n':
                //normal, 3 components
                match = fscanf( file, "%f %f %f", &normal.x, &normal.y, &normal.z );
                if (match==3)
                {
                    normals.push_back( normal );
                }
                else
                {
                    normals.push_back( Math::Vector3( 0.0f, 0.0f, 1.0f ) );
                    Log::Warning( TXT( "Invalid position, setting to (0,0,1). (File: %s, pos: %d, curpos: %d)" ), path.c_str(), fpos, currpos );
                }
                break;

            case 't':
                {
                    //texcoord, 2 or 3 components
                    float32_t discard;
                    match = fscanf( file, "%f %f %f", &texCoord.x, &texCoord.y, &discard );
                    if ( match==2 || match==3 )
                    {
                        texCoords.push_back( texCoord );
                    }
                    else
                    {
                        texCoords.push_back( Math::Vector2() );
                        Log::Warning( TXT( "Invalid texture coordinate, setting to (0,0). (File: %s, pos: %d, curpos: %d)" ), path.c_str(), fpos, currpos );
                    }

                    break;
                }            
            }
            break;

        case 'u':
            if ( strncmp( buf,"usemtl",6 )==0 )
            {
                tchar shader_name[256];
                _ftscanf( file, TXT( "%s" ) , shader_name );

                std::map< tstring, SceneGraph::Shader* >::iterator itr = shaders.find( shader_name );
                if ( itr == shaders.end() )
                {
                    shaders[ shader_name ] = ReadMTL( path.Directory() + shader_name );
                    itr = shaders.find( shader_name );
                }

                curShader = (*itr).second;

                if ( curShader )
                {
                    mesh->AddShader( curShader );
                }
                else
                {
                    Log::Warning( TXT( "Could not parse shader from material: %s" ), shader_name );
                }
            }
            break;

        case 'f':
            //face
            fscanf( file, "%s", buf);

            {
                std::vector< int32_t > faceVertexIndices;
                std::vector< int32_t > faceNormalIndices;
                std::vector< int32_t > faceTexCoordIndices;

                int32_t vertexIndex;
                int32_t normalIndex;
                int32_t texCoordIndex;

                // no material has been defined so we need to allocate a fragment
                if ( !curShader )
                {
                    tstring name = TXT( "default shader" );
                    std::map< tstring, SceneGraph::Shader* >::iterator itr = shaders.find( name );
                    if ( itr != shaders.end() )
                    {
                        curShader = (*itr).second;
                    }
                    else
                    {
                        curShader = new SceneGraph::Shader();
                        curShader->SetName( name );
                        shaders[ curShader->GetName() ] = curShader;
                    }

                    curShaderIndex = mesh->AddShader( curShader );
                }

                //determine the type, and read the initial vertex, all entries in a face must have the same format
                if ( sscanf( buf, "%d//%d", &vertexIndex, &normalIndex ) == 2 ) 
                {
                    do
                    {
                        //remap them to the right spot
                        vertexIndex = ( vertexIndex > 0 ) ? ( vertexIndex - 1 ) : ( (int)vertices.size() + vertexIndex );
                        normalIndex = ( normalIndex > 0 ) ? ( normalIndex - 1 ) : ( (int)normals.size() + normalIndex );

                        faceVertexIndices.push_back( vertexIndex );
                        faceNormalIndices.push_back( normalIndex );
                    } while( fscanf( file, "%d//%d", &vertexIndex, &normalIndex ) == 2 );
                }

                else if ( sscanf( buf, "%d/%d/%d", &vertexIndex, &texCoordIndex, &normalIndex ) == 3 )
                {
                    //This face has vertex, texture coordinate, and normal indices

                    do
                    {
                        //remap them to the right spot
                        vertexIndex = ( vertexIndex > 0 ) ? ( vertexIndex - 1 ) : ( (int)vertices.size() + vertexIndex );
                        texCoordIndex = ( texCoordIndex > 0 ) ? ( texCoordIndex - 1 ) : ( (int)texCoords.size() + texCoordIndex );
                        normalIndex = ( normalIndex > 0 ) ? ( normalIndex - 1 ) : ( (int)normals.size() + normalIndex );

                        faceVertexIndices.push_back( vertexIndex );
                        faceTexCoordIndices.push_back( texCoordIndex );
                        faceNormalIndices.push_back( normalIndex );
                    } while( fscanf( file, "%d/%d/%d", &vertexIndex, &texCoordIndex, &normalIndex ) == 3 );
                }
                else if ( sscanf( buf, "%d/%d", &vertexIndex, &texCoordIndex ) == 2 )
                {
                    //This face has vertex and texture coordinate indices

                    do
                    {
                        //remap them to the right spot
                        vertexIndex = ( vertexIndex > 0 ) ? ( vertexIndex - 1 ) : ( (int)vertices.size() + vertexIndex );
                        texCoordIndex = ( texCoordIndex > 0 ) ? ( texCoordIndex - 1 ) : ( (int)texCoords.size() + texCoordIndex );

                        faceVertexIndices.push_back( vertexIndex );
                        faceTexCoordIndices.push_back( texCoordIndex );
                    } while( fscanf( file, "%d/%d", &vertexIndex, &texCoordIndex ) == 2 );
                }
                else if ( sscanf( buf, "%d", &vertexIndex ) == 1 )
                {
                    //This face has only vertex indices

                    do
                    {
                        //remap them to the right spot
                        vertexIndex = ( vertexIndex > 0 ) ? ( vertexIndex - 1 ) : ( (int)vertices.size() + vertexIndex );

                        faceVertexIndices.push_back( vertexIndex );

                        // but we default to having normals?
                        if ( !normals.empty() )
                        {
                            faceNormalIndices.push_back( vertexIndex );
                        }

                    } while ( fscanf( file, "%d", &vertexIndex ) == 1 );
                }
                else 
                {
                    //bad format
                    HELIUM_BREAK();
                    fgets( buf, sizeof( buf ), file );
                }

                for ( std::vector< int32_t >::const_iterator itr = faceVertexIndices.begin(), end = faceVertexIndices.end(); itr != end; ++itr )
                {
                    mesh->m_WireframeVertexIndices.push_back( (*itr) );
                    if ( itr != ( end - 1 ) )
                    {
                        mesh->m_WireframeVertexIndices.push_back( (*itr + 1 ) );
                    }
                    else
                    {
                        mesh->m_WireframeVertexIndices.push_back( faceVertexIndices.front() );
                    }
                }

                uint32_t numTris = (uint32_t)faceVertexIndices.size() - 2;

                for ( uint32_t i = 0; i < numTris; ++i )
                {
                    Triangle t;
                    t.m_ShaderIndex = curShaderIndex;

                    if ( flipWinding )
                    {
                        t.m_VertIndices.push_back( faceVertexIndices[ 2 + i ] );
                        t.m_VertIndices.push_back( faceVertexIndices[ 1 + i ] );
                        t.m_VertIndices.push_back( faceVertexIndices[ 0 ] );
                    }
                    else
                    {
                        t.m_VertIndices.push_back( faceVertexIndices[ 0 ] );
                        t.m_VertIndices.push_back( faceVertexIndices[ 1 + i ] );
                        t.m_VertIndices.push_back( faceVertexIndices[ 2 + i ] );
                    }

                    if ( !faceNormalIndices.empty() )
                    {
                        if ( flipWinding )
                        {
                            t.m_NormalIndices.push_back( faceNormalIndices[ 2 + i ] );
                            t.m_NormalIndices.push_back( faceNormalIndices[ 1 + i ] );
                            t.m_NormalIndices.push_back( faceNormalIndices[ 0 ] );
                        }
                        else
                        {
                            t.m_NormalIndices.push_back( faceNormalIndices[ 0 ] );
                            t.m_NormalIndices.push_back( faceNormalIndices[ 1 + i ] );
                            t.m_NormalIndices.push_back( faceNormalIndices[ 2 + i ] );
                        }
                    }

                    if ( !faceTexCoordIndices.empty() )
                    {
                        if ( flipWinding )
                        {
                            t.m_TexCoordIndices.push_back( faceTexCoordIndices[ 2 + i ] );
                            t.m_TexCoordIndices.push_back( faceTexCoordIndices[ 1 + i ] );
                            t.m_TexCoordIndices.push_back( faceTexCoordIndices[ 0 ] );
                        }
                        else
                        {
                            t.m_TexCoordIndices.push_back( faceTexCoordIndices[ 0 ] );
                            t.m_TexCoordIndices.push_back( faceTexCoordIndices[ 1 + i ] );
                            t.m_TexCoordIndices.push_back( faceTexCoordIndices[ 2 + i ] );
                        }
                    }

                    triangles.push_back( t );
                }
            }

            break;

        case 's':
        case 'g':
        default:
            fgets( buf, sizeof( buf ), file );
        };
    }

    fclose(file);

    mesh->m_Positions = vertices;
    mesh->m_Normals.resize( mesh->m_Positions.size() );
    mesh->m_BaseUVs.resize( mesh->m_Positions.size() );
    mesh->m_ShaderTriangleCounts.resize( mesh->m_ShaderIDs.size() );

    for ( uint32_t i = 0; i < triangles.size(); ++i )
    {
        HELIUM_ASSERT( triangles[ i ].m_VertIndices.size() == 3 );

        mesh->m_ShaderTriangleCounts[ triangles[ i ].m_ShaderIndex ]++;

        mesh->m_TriangleVertexIndices.push_back( triangles[ i ].m_VertIndices[ 0 ] );
        mesh->m_TriangleVertexIndices.push_back( triangles[ i ].m_VertIndices[ 1 ] );
        mesh->m_TriangleVertexIndices.push_back( triangles[ i ].m_VertIndices[ 2 ] );

        if ( !triangles[ i ].m_NormalIndices.empty() )
        {
            HELIUM_ASSERT( triangles[ i ].m_NormalIndices.size() == 3 );
            mesh->m_Normals[ triangles[ i ].m_VertIndices[ 0 ] ] = normals[ triangles[ i ].m_NormalIndices[ 0 ] ];
            mesh->m_Normals[ triangles[ i ].m_VertIndices[ 1 ] ] = normals[ triangles[ i ].m_NormalIndices[ 1 ] ];
            mesh->m_Normals[ triangles[ i ].m_VertIndices[ 2 ] ] = normals[ triangles[ i ].m_NormalIndices[ 2 ] ];
        }

        if ( !triangles[ i ].m_TexCoordIndices.empty() )
        {
            HELIUM_ASSERT( triangles[ i ].m_TexCoordIndices.size() == 3 );
            mesh->m_BaseUVs[ triangles[ i ].m_VertIndices[ 0 ] ] = texCoords[ triangles[ i ].m_TexCoordIndices[ 0 ] ];
            mesh->m_BaseUVs[ triangles[ i ].m_VertIndices[ 1 ] ] = texCoords[ triangles[ i ].m_TexCoordIndices[ 1 ] ];
            mesh->m_BaseUVs[ triangles[ i ].m_VertIndices[ 2 ] ] = texCoords[ triangles[ i ].m_TexCoordIndices[ 2 ] ];
        }
    }

    return mesh;
}
