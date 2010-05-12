#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Extension
  {
  public:
    const static ExtensionSpec ACV;
    const static ExtensionSpec BANK;
    const static ExtensionSpec BIK;
    const static ExtensionSpec BMP;
    const static ExtensionSpec BNK;
    const static ExtensionSpec COLLADA;
    const static ExtensionSpec CFG;
    const static ExtensionSpec DATA;
    const static ExtensionSpec DB;
    const static ExtensionSpec GENERATED;
    const static ExtensionSpec GIF;
    const static ExtensionSpec GRAPH_SHADER;
    const static ExtensionSpec HDR;
    const static ExtensionSpec IMPL;
    const static ExtensionSpec JPG;
    const static ExtensionSpec LMAP;
    const static ExtensionSpec LUA;
    const static ExtensionSpec MAYA;
    const static ExtensionSpec MAYA_BINARY;
    const static ExtensionSpec MAYA_OBJ;
    const static ExtensionSpec MENTALRAY;
    const static ExtensionSpec MP3;
    const static ExtensionSpec PNG;
    const static ExtensionSpec REFLECT_BINARY;
    const static ExtensionSpec REFLECT_TEXT;
    const static ExtensionSpec SFX;
    const static ExtensionSpec SPEEDTREE;
    const static ExtensionSpec SQL;
    const static ExtensionSpec SUMMARY;
    const static ExtensionSpec TGA;
    const static ExtensionSpec TP;
    const static ExtensionSpec TPH;
    const static ExtensionSpec TXT;
    const static ExtensionSpec VAG;
    const static ExtensionSpec VLD;
    const static ExtensionSpec WAV;
    const static ExtensionSpec XGF;
    const static ExtensionSpec XML;
    const static ExtensionSpec XVAG;

    const static FamilySpec CONFIG_FAMILY;
    const static DecorationSpec CONFIG_DECORATION;

    static FilterSpec ALL_FILTER;
    static FilterSpec AUDIO_FILTER;
    static FilterSpec HDR_TEXTUREMAP_FILTER;
    static FilterSpec TEXTUREMAP_FILTER;   

    static void Init();
  };
}
