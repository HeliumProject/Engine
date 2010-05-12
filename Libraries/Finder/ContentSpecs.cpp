#include "ContentSpecs.h"

#include "Finder.h"
#include "AssetSpecs.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  //
  // files
  //

  const FileSpec Content::MANIFEST_FILE                       ( "Content::MANIFEST_FILE",           "",     "" );             
  const FileSpec Content::RIGGED_FILE                         ( "Content::RIGGED_FILE",             "",     "" );          
  const FileSpec Content::STATIC_FILE                         ( "Content::STATIC_FILE",             "",     "" );
  const FileSpec Content::COLLISION_FILE                      ( "Content::COLLISION_FILE",          "",     "" );
  const FileSpec Content::DESTRUCTION_GLUE_FILE               ( "Content::DESTRUCTION_GLUE_FILE",   "",     "" );
  const FileSpec Content::PATHFINDING_FILE                    ( "Content::PATHFINDING_FILE",        "",     "" );
  const FileSpec Content::INSTANCES_FILE                      ( "Content::INSTANCES_FILE",          "",     "" );
  //const FileSpec Content::LIGHTS_FILE                         ( "Content::LIGHTS_FILE",             "",     "" );
  const FileSpec Content::MENTALRAY_FILE                      ( "Content::MENTALRAY_FILE",          "",     "" );
  const FileSpec Content::ANIMATION_FILE                      ( "Content::ANIMATION_FILE",          "",     "" );
  const FileSpec Content::CINESCENE_FILE                      ( "Content::CINESCENE_FILE",          "",     "" );
  const FileSpec Content::SHADOWCASTERS_FILE                  ( "Content::SHADOWCASTERS_FILE",      "shadowcasters",  Extension::TXT,   "" );
  const FileSpec Content::OBJECTS_FILE                        ( "Content::OBJECTS_FILE",             "",     "" );

  //
  // suffix
  //

  const SuffixSpec Content::MANIFEST_SUFFIX                   ( "Content::MANIFEST_SUFFIX",         "_manifest" );
  const SuffixSpec Content::RIGGED_SUFFIX                     ( "Content::RIGGED_SUFFIX",           "_rigged" );
  const SuffixSpec Content::STATIC_SUFFIX                     ( "Content::STATIC_SUFFIX",           "_static" );
  const SuffixSpec Content::COLLISION_SUFFIX                  ( "Content::COLLISION_SUFFIX",        "_collision" );
  const SuffixSpec Content::DESTRUCTION_GLUE_SUFFIX           ( "Content::DESTRUCTION_GLUE_SUFFIX", "_destructglue" );
  const SuffixSpec Content::PATHFINDING_SUFFIX                ( "Content::PATHFINDING_SUFFIX",      "_pathfinding" );
  const SuffixSpec Content::INSTANCES_SUFFIX                  ( "Content::INSTANCES_SUFFIX",        "_instances" );
  //const SuffixSpec Content::LIGHTS_SUFFIX                     ( "Content::LIGHTS_SUFFIX",           "_lights" );
  const SuffixSpec Content::MENTALRAY_SUFFIX                  ( "Content::MENTALRAY_SUFFIX",        "_mentalray" );
  const SuffixSpec Content::ANIMATION_SUFFIX                  ( "Content::ANIMATION_SUFFIX",        "_animation" );
  const SuffixSpec Content::CINESCENE_SUFFIX                  ( "Content::CINESCENE_SUFFIX",        "_cinescene" );
  const SuffixSpec Content::OBJECTS_SUFFIX                    ( "Content::OBJECTS_SUFFIX",           "_objects" );

  //
  // decorations
  //

  const DecorationSpec Content::MANIFEST_DECORATION           ( "Content::MANIFEST",        "Manifest File",         MANIFEST_SUFFIX,         Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::RIGGED_DECORATION             ( "Content::RIGGED",          "Rigged File",           RIGGED_SUFFIX,           Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::STATIC_DECORATION             ( "Content::STATIC",          "Static File",           STATIC_SUFFIX,           Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::COLLISION_DECORATION          ( "Content::COLLISION",       "Collision File",        COLLISION_SUFFIX,        Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::DESTRUCTION_GLUE_DECORATION   ( "Content::DESTRUCTION_GLUE","Destruction Glue File", DESTRUCTION_GLUE_SUFFIX, Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::PATHFINDING_DECORATION        ( "Content::PATHFINDING",     "Pathfinding File",      PATHFINDING_SUFFIX,      Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  //const DecorationSpec Content::LIGHTS_DECORATION             ( "Content::LIGHTS",          "Lights File",           LIGHTS_SUFFIX,           Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::MENTALRAY_DECORATION          ( "Content::MENTALRAY",       "Mentalray File",        MENTALRAY_SUFFIX,        Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::ANIMATION_DECORATION          ( "Content::ANIMATION",       "Animation File",        ANIMATION_SUFFIX,        Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::CINESCENE_DECORATION          ( "Content::CINESCENE",       "Cinescene File",        CINESCENE_SUFFIX,        Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Content::OBJECTS_DECORATION            ( "Content::OBJECTS",         "Objects File",          OBJECTS_SUFFIX,          Asset::CONTENT_FAMILY,   Extension::REFLECT_BINARY );
}
