#pragma once

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"

#include "Platform/Trace.h"
#include "Foundation/StringConverter.h"
#include "Engine/Serializer.h"
#include "Rendering/RRenderResource.h"
#include "Reflect/Enumeration.h"
#include "Reflect/Structure.h"

namespace Helium
{
    HELIUM_DECLARE_RPTR( RTexture2d );

    /// Font resource.
    class HELIUM_GRAPHICS_API Font : public Resource
    {
        HELIUM_DECLARE_OBJECT( Font, Resource );

    public:

        /// Font texture sheet compression options.
        class ECompression
        {
        public:
            enum Enum
            {
                GRAYSCALE_UNCOMPRESSED,
                COLOR_COMPRESSED,
            };

            REFLECT_DECLARE_ENUMERATION( ECompression );

            static void EnumerateEnum( Helium::Reflect::Enumeration& info )
            {
                info.AddElement( GRAYSCALE_UNCOMPRESSED,    TXT( "GRAYSCALE_UNCOMPRESSED" ) );
                info.AddElement( COLOR_COMPRESSED,          TXT( "COLOR_COMPRESSED" ) );
            }
        };

        /*
        /// Font texture sheet compression options.
        HELIUM_ENUM( ECompression, COMPRESSION,
            ( GRAYSCALE_UNCOMPRESSED )  // Grayscale only, no compression (8 bits per pixel).
            ( COLOR_COMPRESSED ) );     // Block compression (DXT1).
        */

        /// Default texture point size.
        static const uint32_t DEFAULT_POINT_SIZE = 12;
        /// Default display resolution, in DPI.
        static const uint32_t DEFAULT_DPI = 72;

        /// Default texture sheet width, in pixels.
        static const uint16_t DEFAULT_TEXTURE_SHEET_WIDTH = 256;
        /// Default texture sheet height, in pixels.
        static const uint16_t DEFAULT_TEXTURE_SHEET_HEIGHT = 256;

        /// Default texture compression scheme.
        static const ECompression::Enum DEFAULT_TEXTURE_COMPRESSION = ECompression::COLOR_COMPRESSED;

        /// Character information.
        //TODO: Would be nice if we had good structure support for DynamicArrays so we didn't have to make this an Object
        struct HELIUM_GRAPHICS_API Character //: public Object
        {
            REFLECT_DECLARE_BASE_STRUCTURE(Font::Character);
            //REFLECT_DECLARE_OBJECT(Font::Character, Reflect::Object);

            static void PopulateComposite( Reflect::Composite& comp );

            bool                  operator== (const Character& rhs) const
            {
                return (codePoint == rhs.codePoint && 
                    imageX == rhs.imageX &&
                    imageY == rhs.imageY &&
                    imageWidth == rhs.imageWidth &&
                    imageHeight == rhs.imageHeight &&
                    width == rhs.width &&
                    height == rhs.height &&
                    bearingX == rhs.bearingX &&
                    bearingY == rhs.bearingY &&
                    advance == rhs.advance &&
                    texture == rhs.texture); 
            }

            bool                  operator!= (const Character& rhs) const
            {
                return !(*this == rhs);
            }

            /// Unicode code point value.
            uint32_t codePoint;

            /// Horizontal pixel coordinate of the top-left corner of the character in the texture sheet.
            uint16_t imageX;
            /// Vertical pixel coordinate of the top-left corner of the character in the texture sheet.
            uint16_t imageY;
            /// Pixel width of the character in the texture sheet.
            uint16_t imageWidth;
            /// Pixel height of the character in the texture sheet.
            uint16_t imageHeight;

            /// Character bounding box width (26.6 fixed-point value, in pixels).
            int32_t width;
            /// Character bounding box height (26.6 fixed-point value, in pixels).
            int32_t height;
            /// Horizontal distance from the cursor position to the left-most border of the character bounding box (26.6
            /// fixed-point value, in pixels).
            int32_t bearingX;
            /// Vertical distance from the cursor position (on the baseline) to the top-most border of the character
            /// bounding box (26.6 fixed-point value, in pixels).
            int32_t bearingY;
            /// Horizontal distance used to increment the cursor position when drawing the character (26.6 fixed-point
            /// value, in pixels).
            int32_t advance;

            /// Texture sheet index.
            uint8_t texture;
// 
//             /// @name Serialization
//             //@{
//             //PMDTODO: HACK - Remove Serialize()
//             void Serialize( Serializer& s );
//             //@}
        };
        //typedef Helium::StrongPtr<Character> CharacterPtr;
        
        struct HELIUM_GRAPHICS_API PersistentResourceData : public Object
        {
            REFLECT_DECLARE_OBJECT(Font::PersistentResourceData, Reflect::Object);

            PersistentResourceData();
            static void PopulateComposite( Reflect::Composite& comp );

            /// Cached ascender height, in pixels (26.6 fixed-point value).
            int32_t m_ascender;
            /// Cached descender depth, in pixels (26.6 fixed-point value).
            int32_t m_descender;
            /// Cached font height, in pixels (26.6 fixed-point value).
            int32_t m_height;
            /// Maximum advance width when rendering text, in pixels (26.6 fixed-point value).
            int32_t m_maxAdvance;

            /// Array of characters (ordered by code point value to allow for binary searching).
            //DynamicArray<CharacterPtr> m_characters;
            DynamicArray<Character> m_characters;

            /// Array of texture sheets.
            RTexture2dPtr* m_pspTextures;
            /// Texture sheet load IDs.
            size_t* m_pTextureLoadIds;
            /// Number of texture sheets.
            uint8_t m_textureCount;
        };
        
        /// Persistent shader resource data.
        PersistentResourceData m_persistentResourceData;


        /// Base template type for handling conversions to wide-character strings as needed for ProcessText().
        template< typename CharType >
        class ProcessTextConverter
        {
        };

        /// Single-byte to wide-character string conversion support for ProcessText().
        template<>
        class HELIUM_GRAPHICS_API ProcessTextConverter< char >
        {
        public:
            /// Maximum length for converted strings.
            static const size_t STRING_LENGTH_MAX = 1024;

            /// @name Construction/Destruction
            //@{
            inline ProcessTextConverter( const char* pString, size_t length );
            //@}

            /// @name Data Access
            //@{
            inline const wchar_t* GetString() const;
            inline size_t GetLength() const;
            //@}

        private:
            /// Converted string.
            wchar_t m_string[ STRING_LENGTH_MAX ];
            /// Cached string length.
            size_t m_length;
        };

        /// Wide-character to wide-character (dummy) string conversion support for ProcessText().
        template<>
        class HELIUM_GRAPHICS_API ProcessTextConverter< wchar_t >
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline ProcessTextConverter( const wchar_t* pString, size_t length );
            //@}

            /// @name Data Access
            //@{
            inline const wchar_t* GetString() const;
            inline size_t GetLength() const;
            //@}

        private:
            /// Cached string pointer.
            const wchar_t* m_pString;
            /// Cached string length.
            size_t m_length;
        };

        /// @name Construction/Destruction
        //@{
        Font();
        virtual ~Font();
        //@}

        /// @name Serialization
        //@{
        //virtual void Serialize( Serializer& s );
        
        static void PopulateComposite( Reflect::Composite& comp );

        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();
        //@}

        /// @name Resource Serialization
        //@{    
        //virtual void SerializePersistentResourceData( Serializer& s );
        virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object);
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

        /// @name Data Access
        //@{
        inline float32_t GetPointSize() const;
        inline uint32_t GetDpi() const;

        inline uint16_t GetTextureSheetWidth() const;
        inline uint16_t GetTextureSheetHeight() const;

        inline ECompression GetTextureCompression() const;

        inline bool GetAntialiased() const;

        inline int32_t GetAscenderFixed() const;
        inline int32_t GetDescenderFixed() const;
        inline int32_t GetHeightFixed() const;
        inline int32_t GetMaxAdvanceFixed() const;

        inline float32_t GetAscenderFloat() const;
        inline float32_t GetDescenderFloat() const;
        inline float32_t GetHeightFloat() const;
        inline float32_t GetMaxAdvanceFloat() const;
        //@}

        /// @name Character Information
        /// All character codes must be provided as Unicode code points (this may require conversion of string data if
        /// the application is not built in Unicode mode, or for other special cases such as surrogate pairs in UTF-16
        /// strings).
        //@{
        inline uint32_t GetCharacterCount() const;
        inline const Character& GetCharacter( uint32_t index ) const;
        inline uint32_t GetCharacterIndex( const Character* pCharacter ) const;

        inline const Character* FindCharacter( uint32_t codePoint ) const;
        //@}

        /// @name Texture Sheet Access
        //@{
        inline uint8_t GetTextureSheetCount() const;
        inline RTexture2d* GetTextureSheet( uint8_t index ) const;
        //@}

        /// @name Text Processing Support
        //@{
        template< typename GlyphHandler, typename CharType > void ProcessText(
            const CharType* pString, GlyphHandler& rGlyphHandler ) const;
        template< typename GlyphHandler, typename CharType, typename Allocator > void ProcessText(
            const StringBase< CharType, Allocator >& rString, GlyphHandler& rGlyphHandler ) const;
        //@}

        /// @name Static Utility Functions
        //@{
        inline static float32_t Fixed26x6ToFloat32( int32_t value );
        inline static int32_t Float32ToFixed26x6( float32_t value );
        //@}

    private:
        /// Font size in points.
        float32_t m_pointSize;
        /// Font resolution, in DPI.
        uint32_t m_dpi;

        /// Width of each texture sheet.
        uint16_t m_textureSheetWidth;
        /// Height of each texture sheet.
        uint16_t m_textureSheetHeight;

        /// Texture compression scheme.
        ECompression m_textureCompression;
        
        /// True if this font should use anti-aliasing to smooth edges, false if not.
        bool m_bAntialiased;

        /// @name Text Processing Support, Private
        //@{
        template< typename GlyphHandler, typename CharType > void ProcessText(
            const CharType* pString, size_t characterCount, GlyphHandler& rGlyphHandler ) const;
        //@}
    };
}

#include "Graphics/Font.inl"
