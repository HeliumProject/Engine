#include "ExtensionSpecs.h"

#include "Finder.h"

namespace FinderSpecs
{
  const ExtensionSpec Extension::ACV                   ( "Extension::ACV",                   "Adobe Photoshop Saved Curve",      "acv" );
  const ExtensionSpec Extension::BANK                  ( "Extension::BANK",                  "SCREAM Session File",              "bank" );
  const ExtensionSpec Extension::BIK                   ( "Extension::BIK",                   "Bink Movie File",                  "bik" );
  const ExtensionSpec Extension::BMP                   ( "Extension::BMP",                   "Bitmap File",                      "bmp" );
  const ExtensionSpec Extension::BNK                   ( "Extension::BNK",                   "SCREAM Binary File",               "bnk" );
  const ExtensionSpec Extension::CFG                   ( "Extension::CFG",                   "Config File",                      "cfg" );
  const ExtensionSpec Extension::COLLADA               ( "Extension::COLLADA",               "Collada File",                     "dae" );
  const ExtensionSpec Extension::DATA                  ( "Extension::DATA",                  "Data File",                        "dat" );
  const ExtensionSpec Extension::DB                    ( "Extension::DB",                    "Database File",                    "db" );
  const ExtensionSpec Extension::GENERATED             ( "Extension::GENERATED",             "Generated File",                   "generated" );
  const ExtensionSpec Extension::GIF                   ( "Extension::GIF",                   "GIF",                              "gif" );
  const ExtensionSpec Extension::GRAPH_SHADER          ( "Extension::GRAPH_SHADER",          "Graph Shader",                     "graphshader" );
  const ExtensionSpec Extension::HDR                   ( "Extension::HDR",                   "HDR",                              "hdr" );
  const ExtensionSpec Extension::IMPL                  ( "Extension::IMPL",                  "IMPL",                             "impl" );
  const ExtensionSpec Extension::JPG                   ( "Extension::JPG",                   "JPG",                              "jpg" );
  const ExtensionSpec Extension::LMAP                  ( "Extension::LMAP",                  "Lightmap File",                    "tga" );
  const ExtensionSpec Extension::LUA                   ( "Extension::LUA",                   "LUA",                              "lua" );
  const ExtensionSpec Extension::MAYA_BINARY           ( "Extension::MAYA_BINARY",           "Maya Binary File",                 "mb" );
  const ExtensionSpec Extension::MAYA_OBJ              ( "Extension::MAYA_OBJ",              "Maya OBJ File",                    "obj" );
  const ExtensionSpec Extension::MENTALRAY             ( "Extension::MENTALRAY",             "Mental Ray File",                  "mi" );
  const ExtensionSpec Extension::MP3                   ( "Extension::MP3",                   "Mp3 File",                         "mp3" );
  const ExtensionSpec Extension::PNG                   ( "Extension::PNG",                   "PNG",                              "png" );
  const ExtensionSpec Extension::REFLECT_BINARY        ( "Extension::REFLECT_BINARY",        "Reflect Binary File",              "rb" );
  const ExtensionSpec Extension::REFLECT_TEXT          ( "Extension::REFLECT_TEXT",          "Reflect XML File",                 "rx" );
  const ExtensionSpec Extension::SFX                   ( "Extension::SFX",                   "SCREAM Sfx 4.0 File",              "sfx" );
  const ExtensionSpec Extension::SPEEDTREE             ( "Extension::SPEEDTREE",             "Speed Tree",                       "spt" );
  const ExtensionSpec Extension::SQL                   ( "Extension::SQL",                   "SQL",                              "sql" );
  const ExtensionSpec Extension::SUMMARY               ( "Extension::SUMMARY",               "SCREAM Summary File",              "summary" );
  const ExtensionSpec Extension::TGA                   ( "Extension::TGA",                   "Targa File",                       "tga" );
  const ExtensionSpec Extension::TP                    ( "Extension::TP",                    "Texture Pack Texel Data",          "tp" );
  const ExtensionSpec Extension::TPH                   ( "Extension::TPH",                   "Texture Pack Header Data",         "tph" );
  const ExtensionSpec Extension::TXT                   ( "Extension::TXT",                   "Text File",                        "txt" );
  const ExtensionSpec Extension::VAG                   ( "Extension::VAG",                   "Sony ADPCM File",                  "vag" );
  const ExtensionSpec Extension::VLD                   ( "Extension::VLD",                   "VLD File",                         "vld" );
  const ExtensionSpec Extension::WAV                   ( "Extension::WAV",                   "Windows Wave File",                "wav" );
  const ExtensionSpec Extension::XGF                   ( "Extension::XGF",                   "XGF File",                         "xgf" );
  const ExtensionSpec Extension::XML                   ( "Extension::XML",                   "XML File",                         "xml" );
  const ExtensionSpec Extension::XVAG                  ( "Extension::XVAG",                  "XVag File",                        "xvag" );

  const FamilySpec Extension::CONFIG_FAMILY            ( "Extension::CONFIG_FAMILY",         "Config File",        "config" );
  const DecorationSpec Extension::CONFIG_DECORATION    ( "Extension::CONFIG_DECORATION",     "Config File",        CONFIG_FAMILY,     Extension::XML );

  FilterSpec Extension::ALL_FILTER                     ( "Extension::ALL_FILTER",            "All",                "*.*" );
  FilterSpec Extension::AUDIO_FILTER                   ( "Extension::AUDIO_FILTER",          "Audio File" );
  FilterSpec Extension::HDR_TEXTUREMAP_FILTER          ( "Extension::HDR_TEXTUREMAP_FILTER", "HDR Texture map" );
  FilterSpec Extension::TEXTUREMAP_FILTER              ( "Extension::TEXTUREMAP_FILTER",     "Texture map" );
  
  void Extension::Init()
  {
    TEXTUREMAP_FILTER.AddSpec( GIF );
    TEXTUREMAP_FILTER.AddSpec( HDR );
    TEXTUREMAP_FILTER.AddSpec( JPG );
    TEXTUREMAP_FILTER.AddSpec( PNG );
    TEXTUREMAP_FILTER.AddSpec( TGA );
    
    AUDIO_FILTER.AddSpec( BANK );
    AUDIO_FILTER.AddSpec( MP3 );
    AUDIO_FILTER.AddSpec( SFX );
    AUDIO_FILTER.AddSpec( VAG );
    AUDIO_FILTER.AddSpec( XVAG );
    AUDIO_FILTER.AddSpec( WAV );

    HDR_TEXTUREMAP_FILTER.AddSpec( HDR );
  }
}
