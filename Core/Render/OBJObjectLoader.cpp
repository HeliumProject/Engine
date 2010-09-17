/*#include "Precompile.h"*/
#include "OBJObjectLoader.h"

#include "Foundation/Log.h"

#include <map>
#include <set>

#define BUF_SIZE 1024

using namespace Helium;
using namespace Helium::Core;

////////////////////////////////////////////////////////////////////////////////////////////////
static void skipLine(char * buf, int size, FILE * fp)
{
    do 
    {
        buf[size-1] = '$';
        fgets(buf, size, fp);
    } while (buf[size-1] != '$');
}

////////////////////////////////////////////////////////////////////////////////////////////////
static void ObjError(FILE* fp, int fpos, int currpos,const char* error)
{
    char buf[BUF_SIZE];

    Log::Warning( TXT( "%s [file offset = %d]\n" ),error,fpos);
    fseek(fp,fpos,SEEK_SET);
    for (int rr=0;rr<128;rr++)
    {
        int c = getc(fp);
        Log::Print( TXT( "%c" ),c);
    }
    Log::Print( TXT( "\n\n" ) );
    fseek(fp,currpos,SEEK_SET);
    skipLine( buf, BUF_SIZE, fp);
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 OBJObjectLoader::ParseFile(const tchar* fname, bool winding)
{
    //winding = true;
    FILE *fp;

    fp = _tfopen( fname, TXT( "rb" ) );
    if (!fp) 
    {
        m_parse_error = PARSE_FILE_FAILED;
        return PARSE_FILE_FAILED;
    }

    char buf[BUF_SIZE];
    float val[4];
    int idx[3][3];
    int match;
    bool vtx4Comp = false;
    bool tex3Comp = false;
    bool hasTC = false;
    bool hasNormals = false;
    bool fixup=false;
    u32 fragid=-1;

    m_parse_warnings = 0;
    while ( 1 ) 
    {
        int fpos = ftell(fp);

        if (fscanf( fp, "%s", buf) == EOF)
            break;

        int currpos=ftell(fp);

        switch (buf[0]) 
        {
        case '#':
            //comment line, eat the remainder
            skipLine( buf, BUF_SIZE, fp);
            break;

        case 'v':
            switch (buf[1]) 
            {          
            case '\0':
                //vertex, 3 or 4 components
                val[3] = 1.0f;  //default w coordinate
                match = fscanf( fp, "%f %f %f %f", &val[0], &val[1], &val[2], &val[3]);
                if (match==3 || match==4)
                {
                    m_positions.push_back( val[0]);
                    m_positions.push_back( val[1]);
                    m_positions.push_back( val[2]);
                    m_positions.push_back( val[3]);
                    vtx4Comp |= ( match == 4);
                }
                else
                {
                    m_positions.push_back( 0.0f);
                    m_positions.push_back( 0.0f);
                    m_positions.push_back( 0.0f);
                    m_positions.push_back( 0.0f);
                    m_parse_warnings++;
                    ObjError(fp,fpos,currpos,"Invalid position, setting to origin");
                }
                break;

            case 'n':
                //normal, 3 components
                match = fscanf( fp, "%f %f %f", &val[0], &val[1], &val[2]);
                if (match==3)
                {
                    m_normals.push_back( val[0]);
                    m_normals.push_back( val[1]);
                    m_normals.push_back( val[2]);
                }
                else
                {
                    m_normals.push_back( 0.0f);
                    m_normals.push_back( 0.0f);
                    m_normals.push_back( 1.0f);
                    m_parse_warnings++;
                    ObjError(fp,fpos,currpos,"Invalid normal, setting to 0,0,1");
                }
                break;

            case 't':
                {
                    //texcoord, 2 or 3 components
                    val[2] = 0.0f;  //default r coordinate
                    match = fscanf( fp, "%f %f %f", &val[0], &val[1], &val[2]);            
                    if (match==2 || match==3)
                    {
                        m_texcoords.push_back( val[0]);
                        m_texcoords.push_back( val[1]);
                        m_texcoords.push_back( val[2]);
                        tex3Comp |= ( match == 3);
                    }
                    else
                    {
                        m_texcoords.push_back( 0.0f);
                        m_texcoords.push_back( 0.0f);
                        m_texcoords.push_back( 0.0f);
                        m_parse_warnings++;
                        ObjError(fp,fpos,currpos,"Invalid texture coordinate, setting to u=0.0, v=0.0");
                    }

                    break;
                }            
            }
            break;

        case 'u':
            if (strncmp(buf,"usemtl",6)==0)
            {
                tchar shader_name[256];
                _ftscanf( fp, TXT( "%s" ) , shader_name);

                // got through the shader array and see if this shader already exists, if it does use the same frag ID
                fragid = 0xffffffff;
                for (u32 fr=0;fr<(u32)m_fragments.size();fr++)
                {
                    if (m_fragments[fr].m_shader == shader_name)
                    {
                        fragid = fr;
                        break;
                    }
                }

                if (fragid==0xffffffff)
                {
                    ShaderFrag frag;
                    frag.m_shader = shader_name;
                    m_fragments.push_back(frag);
                    fragid = (u32)m_fragments.size()-1;
                }
            }
            break;

        case 'f':
            //face
            fscanf( fp, "%s", buf);

            // no material has been defined so we need to allocate a fragment
            if (fragid==0xffffffff)
            {
                ShaderFrag frag;
                frag.m_shader = TXT( "default" );
                m_fragments.push_back(frag);
                fragid = (u32)m_fragments.size()-1;
            }

            //determine the type, and read the initial vertex, all entries in a face must have the same format
            if ( sscanf( buf, "%d//%d", &idx[0][0], &idx[0][1]) == 2) 
            {
                //This face has vertex and normal indices

                //remap them to the right spot
                idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int)m_positions.size() - idx[0][0]);
                idx[0][1] = (idx[0][1] > 0) ? (idx[0][1] - 1) : ((int)m_normals.size() - idx[0][1]);

                //grab the second vertex to prime
                fscanf( fp, "%d//%d", &idx[1][0], &idx[1][1]);

                //remap them to the right spot
                idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int)m_positions.size() - idx[1][0]);
                idx[1][1] = (idx[1][1] > 0) ? (idx[1][1] - 1) : ((int)m_normals.size() - idx[1][1]);

                //create the fan
                while ( fscanf( fp, "%d//%d", &idx[2][0], &idx[2][1]) == 2) 
                {
                    //remap them to the right spot
                    idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int)m_positions.size() - idx[2][0]);
                    idx[2][1] = (idx[2][1] > 0) ? (idx[2][1] - 1) : ((int)m_normals.size() - idx[2][1]);

                    //add the indices
                    if (winding)
                    {
                        for (int ii = 2; ii >= 0; ii--) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                            m_fragments[fragid].m_nIndex.push_back( idx[ii][1]);
                        }
                    }
                    else
                    {
                        for (int ii = 0; ii < 3; ii++) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                            m_fragments[fragid].m_nIndex.push_back( idx[ii][1]);
                        }
                    }            
                    //prepare for the next iteration
                    idx[1][0] = idx[2][0];
                    idx[1][1] = idx[2][1];
                }
                hasNormals = true;
            }
            else if ( sscanf( buf, "%d/%d/%d", &idx[0][0], &idx[0][1], &idx[0][2]) == 3) 
            {
                //This face has vertex, texture coordinate, and normal indices

                //remap them to the right spot
                idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int)m_positions.size() - idx[0][0]);
                idx[0][1] = (idx[0][1] > 0) ? (idx[0][1] - 1) : ((int)m_texcoords.size() - idx[0][1]);
                idx[0][2] = (idx[0][2] > 0) ? (idx[0][2] - 1) : ((int)m_normals.size() - idx[0][2]);

                //grab the second vertex to prime
                fscanf( fp, "%d/%d/%d", &idx[1][0], &idx[1][1], &idx[1][2]);

                //remap them to the right spot
                idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int)m_positions.size() - idx[1][0]);
                idx[1][1] = (idx[1][1] > 0) ? (idx[1][1] - 1) : ((int)m_texcoords.size() - idx[1][1]);
                idx[1][2] = (idx[1][2] > 0) ? (idx[1][2] - 1) : ((int)m_normals.size() - idx[1][2]);

                //create the fan
                while ( fscanf( fp, "%d/%d/%d", &idx[2][0], &idx[2][1], &idx[2][2]) == 3) 
                {
                    //remap them to the right spot
                    idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int)m_positions.size() - idx[2][0]);
                    idx[2][1] = (idx[2][1] > 0) ? (idx[2][1] - 1) : ((int)m_texcoords.size() - idx[2][1]);
                    idx[2][2] = (idx[2][2] > 0) ? (idx[2][2] - 1) : ((int)m_normals.size() - idx[2][2]);

                    if (winding)
                    {
                        for (int ii = 2; ii >= 0; ii--) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                            m_fragments[fragid].m_tIndex.push_back( idx[ii][1]);
                            m_fragments[fragid].m_nIndex.push_back( idx[ii][2]);
                        }
                    }
                    else
                    {
                        //add the indices
                        for (int ii = 0; ii < 3; ii++) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                            m_fragments[fragid].m_tIndex.push_back( idx[ii][1]);
                            m_fragments[fragid].m_nIndex.push_back( idx[ii][2]);
                        }
                    }

                    //prepare for the next iteration
                    idx[1][0] = idx[2][0];
                    idx[1][1] = idx[2][1];
                    idx[1][2] = idx[2][2];
                }
                hasTC = true;
                hasNormals = true;
            }
            else if ( sscanf( buf, "%d/%d", &idx[0][0], &idx[0][1]) == 2) 
            {
                //This face has vertex and texture coordinate indices

                //remap them to the right spot
                idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int)m_positions.size() - idx[0][0]);
                idx[0][1] = (idx[0][1] > 0) ? (idx[0][1] - 1) : ((int)m_texcoords.size() - idx[0][1]);

                //grab the second vertex to prime
                fscanf( fp, "%d/%d", &idx[1][0], &idx[1][1]);

                //remap them to the right spot
                idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int)m_positions.size() - idx[1][0]);
                idx[1][1] = (idx[1][1] > 0) ? (idx[1][1] - 1) : ((int)m_texcoords.size() - idx[1][1]);

                //create the fan
                while ( fscanf( fp, "%d/%d", &idx[2][0], &idx[2][1]) == 2) 
                {
                    //remap them to the right spot
                    idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int)m_positions.size() - idx[2][0]);
                    idx[2][1] = (idx[2][1] > 0) ? (idx[2][1] - 1) : ((int)m_texcoords.size() - idx[2][1]);

                    //add the indices
                    if (winding)
                    {
                        for (int ii = 2; ii >= 0; ii--) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                            m_fragments[fragid].m_tIndex.push_back( idx[ii][1]);
                        }
                    }
                    else
                    {
                        for (int ii = 0; ii < 3; ii++) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                            m_fragments[fragid].m_tIndex.push_back( idx[ii][1]);
                        }
                    }

                    //prepare for the next iteration
                    idx[1][0] = idx[2][0];
                    idx[1][1] = idx[2][1];
                }
                hasTC = true;
            }
            else if ( sscanf( buf, "%d", &idx[0][0]) == 1) 
            {
                //This face has only vertex indices

                //remap them to the right spot
                idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int)m_positions.size() - idx[0][0]);

                //grab the second vertex to prime
                fscanf( fp, "%d", &idx[1][0]);

                //remap them to the right spot
                idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int)m_positions.size() - idx[1][0]);

                //create the fan
                while ( fscanf( fp, "%d", &idx[2][0]) == 1) 
                {
                    //remap them to the right spot
                    idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int)m_positions.size() - idx[2][0]);

                    if (winding)
                    {
                        //add the indices
                        for (int ii = 2; ii >= 0; ii--) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                        }
                    }
                    else
                    {
                        //add the indices
                        for (int ii = 0; ii<3; ii++) 
                        {
                            m_fragments[fragid].m_pIndex.push_back( idx[ii][0]);
                        }
                    }

                    //prepare for the next iteration
                    idx[1][0] = idx[2][0];
                }
            }
            else 
            {
                //bad format
                HELIUM_ASSERT(0);
                skipLine( buf, BUF_SIZE, fp);
            }
            break;

        case 's':
        case 'g':
        default:
            skipLine( buf, BUF_SIZE, fp);
        };
    }

    fclose(fp);

    //post-process data

    //free anything that ended up being unused
    //This will occur if the mesh has normals but the faces didn't use them
    if (!hasNormals) 
    {
        m_normals.clear();
    }

    if (!hasTC) 
    {
        m_texcoords.clear();
    }

    //set the defaults as the worst-case for an obj file
    m_posSize = 4;
    m_tcSize = 3;

    //compact to 3 component vertices if possible
    if (!vtx4Comp) 
    {
        std::vector<float>::iterator src = m_positions.begin();
        std::vector<float>::iterator dst = m_positions.begin();
        for ( ; src < m_positions.end(); ) 
        {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src++;
        }
        m_positions.resize( (m_positions.size() / 4) * 3);
        m_posSize = 3;
    }
    else
    {
        HELIUM_ASSERT(0);
    }

    //compact to 2 component tex coords if possible
    if (!tex3Comp) 
    {
        std::vector<float>::iterator src = m_texcoords.begin();
        std::vector<float>::iterator dst = m_texcoords.begin();
        for ( ; src < m_texcoords.end(); ) 
        {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src++;
        }

        m_texcoords.resize( (m_texcoords.size() / 3) * 2);
        m_tcSize = 2; 

        // fixup for maya's fudged V-channel
        for ( std::vector<float>::iterator itr = m_texcoords.begin(), end = m_texcoords.end(); itr != end; ++itr ) 
        {
            ++itr;
            *itr = -*itr + 1.0f;
        }
    }
    else
    {
        HELIUM_ASSERT(0);
    }

    m_parse_error = m_parse_warnings?PARSE_FILE_FIXUP:PARSE_FILE_OK;;

    return m_parse_error;
}