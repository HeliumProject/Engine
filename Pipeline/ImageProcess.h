#pragma once

#include <string>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Automation/Event.h"

#include "Pipeline/Image/Image.h"

namespace Helium
{
    namespace ImageProcess
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // These globals contain the defaults which can get overridden from the command line
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////
        extern bool                   g_PowerOfTwo;
        extern float                  g_DefaultScaleX;
        extern float                  g_DefaultScaleY;
        extern Helium::OutputColorFormat  g_DefaultOutputFormat;
        extern Helium::PostMipImageFilter g_DefaultPostMipFilter;

        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Image
        //  Class to hold all the info about a single effects texture
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////

        class Definition : public Helium::RefCountBase<Definition>
        {
        public:
            Definition()
                : m_texture (NULL)
                , m_mips (NULL)
                , m_mip_levels (0)
                , m_force_single_mip_level( false )
                , m_user_data(NULL)
                , m_is_normal_map( false )
                , m_is_detail_normal_map( false )
                , m_is_detail_map_only( false )
            {
                m_force_power_of_2 = g_PowerOfTwo;    
                m_relscale_x = g_DefaultScaleX;
                m_relscale_y = g_DefaultScaleY;
                m_output_format = g_DefaultOutputFormat;
                m_post_filter = g_DefaultPostMipFilter;

                m_runtime.m_wrap_u = Helium::UV_WRAP;
                m_runtime.m_wrap_v = Helium::UV_WRAP;
                m_runtime.m_wrap_w = Helium::UV_WRAP;
                m_runtime.m_filter = Helium::FILTER_LINEAR_LINEAR_MIP;
                m_runtime.m_direct_uvs = false;
                m_runtime.m_expand_range = false;
            }

            ~Definition()
            {
                delete m_texture;
                delete m_mips;
            }

            // input data
            tstring                     m_enum;
            tstring                     m_texture_file;
            Helium::OutputColorFormat           m_output_format;
            Helium::PostMipImageFilter          m_post_filter;
            float                           m_relscale_x;
            float                           m_relscale_y;
            bool                            m_force_power_of_2;
            bool                            m_force_single_mip_level;
            bool                            m_is_normal_map;
            bool                            m_is_detail_normal_map;
            bool                            m_is_detail_map_only;

            // output data
            Helium::Image*                 m_texture;
            Helium::MipSet*                  m_mips;
            Helium::MipSet::RuntimeSettings  m_runtime;
            uint32_t                          m_mip_levels;

            // user data
            void*                        m_user_data;
        };

        typedef Helium::SmartPtr<Definition> DefinitionPtr;
        typedef std::vector<DefinitionPtr> V_Definition;

        typedef Helium::Signature< Definition*> DefinitionSignature;

        struct PostLoadArgs
        {

        };
        typedef Helium::Signature< const PostLoadArgs&> PostLoadSignature;

        //
        // The Bank class builds Defs into a packed bank of textures
        //

        class Bank
        {
        private:

            DefinitionSignature::Event m_ProcessDefinition;
            PostLoadSignature::Event m_PostLoad;

        public:
            // The list of textures to work with
            V_Definition m_textures;

            // Events
            void AddDefinitionProcessor( const DefinitionSignature::Delegate& listener )
            {
                m_ProcessDefinition.Add( listener );
            }

            void AddPostLoadListener( const PostLoadSignature::Delegate& listener )
            {
                m_PostLoad.Add( listener );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////
            //
            //  LoadImages
            //
            //  Loads the source textures and fills in the remainder of the process class
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////
            bool LoadImages();

            ////////////////////////////////////////////////////////////////////////////////////////////////
            //
            //  AdjustImages
            //
            //  Adjust the source image for being non power of 2 or if it requires prescaling
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////
            bool AdjustImages();

            ////////////////////////////////////////////////////////////////////////////////////////////////
            //
            //  Compress
            //
            //  Compress and generate the output data along with all the associated mip maps
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////
            bool CompressImages();

            ////////////////////////////////////////////////////////////////////////////////////////////////
            //
            //  Pack
            //
            //  Performs default processing of textures in global texture array
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////
            bool Pack();

            ////////////////////////////////////////////////////////////////////////////////////////////////
            //
            //  WriteDebugFile
            //
            //  Writes the debug text file with the filenames, compression settings, etc.
            //
            ////////////////////////////////////////////////////////////////////////////////////////////////
            bool WriteDebugFile( const tstring& debug_file );
        };
    }
}