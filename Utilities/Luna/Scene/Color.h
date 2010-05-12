#pragma once

#include "API.h"

namespace Luna
{
  class Color
  {
  public:
    // convert floating point to integer
    static DWORD ColorValueToColor( D3DCOLORVALUE color );

    // convert integer to floating point
    static D3DCOLORVALUE ColorToColorValue( DWORD argb );

    // convert integer to floating point
    static D3DCOLORVALUE ColorToColorValue( DWORD a, DWORD r, DWORD g, DWORD b );

    // unpack a color into components
    static void UnpackColor( DWORD packed, DWORD& a, DWORD& r, DWORD& g, DWORD& b );

    // blend color with a weight
    static DWORD BlendColor( DWORD color1, DWORD color2, float weight );

    static const D3DCOLORVALUE SNOW;
    static const D3DCOLORVALUE GHOSTWHITE;
    static const D3DCOLORVALUE WHITESMOKE;
    static const D3DCOLORVALUE GAINSBORO;
    static const D3DCOLORVALUE FLORALWHITE;
    static const D3DCOLORVALUE OLDLACE;
    static const D3DCOLORVALUE LINEN;
    static const D3DCOLORVALUE ANTIQUEWHITE;
    static const D3DCOLORVALUE PAPAYAWHIP;
    static const D3DCOLORVALUE BLANCHEDALMOND;
    static const D3DCOLORVALUE BISQUE;
    static const D3DCOLORVALUE PEACHPUFF;
    static const D3DCOLORVALUE NAVAJOWHITE;
    static const D3DCOLORVALUE MOCCASIN;
    static const D3DCOLORVALUE CORNSILK;
    static const D3DCOLORVALUE IVORY;
    static const D3DCOLORVALUE LEMONCHIFFON;
    static const D3DCOLORVALUE SEASHELL;
    static const D3DCOLORVALUE HONEYDEW;
    static const D3DCOLORVALUE MINTCREAM;
    static const D3DCOLORVALUE AZURE;
    static const D3DCOLORVALUE ALICEBLUE;
    static const D3DCOLORVALUE LAVENDER;
    static const D3DCOLORVALUE LAVENDERBLUSH;
    static const D3DCOLORVALUE MISTYROSE;
    static const D3DCOLORVALUE WHITE;
    static const D3DCOLORVALUE BLACK;
    static const D3DCOLORVALUE SILVER;
    static const D3DCOLORVALUE DARKSLATEGRAY;
    static const D3DCOLORVALUE DARKSLATEGREY;
    static const D3DCOLORVALUE DIMGRAY;
    static const D3DCOLORVALUE DIMGREY;
    static const D3DCOLORVALUE SLATEGRAY;
    static const D3DCOLORVALUE SLATEGREY;
    static const D3DCOLORVALUE LIGHTSLATEGRAY;
    static const D3DCOLORVALUE LIGHTSLATEGREY;
    static const D3DCOLORVALUE GRAY;
    static const D3DCOLORVALUE GREY;
    static const D3DCOLORVALUE LIGHTGREY;
    static const D3DCOLORVALUE LIGHTGRAY;
    static const D3DCOLORVALUE MIDNIGHTBLUE;
    static const D3DCOLORVALUE NAVY;
    static const D3DCOLORVALUE NAVYBLUE;
    static const D3DCOLORVALUE CORNFLOWERBLUE;
    static const D3DCOLORVALUE DARKSLATEBLUE;
    static const D3DCOLORVALUE SLATEBLUE;
    static const D3DCOLORVALUE MEDIUMSLATEBLUE;
    static const D3DCOLORVALUE LIGHTSLATEBLUE;
    static const D3DCOLORVALUE MEDIUMBLUE;
    static const D3DCOLORVALUE ROYALBLUE;
    static const D3DCOLORVALUE BLUE;
    static const D3DCOLORVALUE DODGERBLUE;
    static const D3DCOLORVALUE DEEPSKYBLUE;
    static const D3DCOLORVALUE SKYBLUE;
    static const D3DCOLORVALUE LIGHTSKYBLUE;
    static const D3DCOLORVALUE STEELBLUE;
    static const D3DCOLORVALUE LIGHTSTEELBLUE;
    static const D3DCOLORVALUE LIGHTBLUE;
    static const D3DCOLORVALUE POWDERBLUE;
    static const D3DCOLORVALUE PALETURQUOISE;
    static const D3DCOLORVALUE DARKTURQUOISE;
    static const D3DCOLORVALUE MEDIUMTURQUOISE;
    static const D3DCOLORVALUE TURQUOISE;
    static const D3DCOLORVALUE CYAN;
    static const D3DCOLORVALUE LIGHTCYAN;
    static const D3DCOLORVALUE CADETBLUE;
    static const D3DCOLORVALUE MEDIUMAQUAMARINE;
    static const D3DCOLORVALUE AQUAMARINE;
    static const D3DCOLORVALUE DARKGREEN;
    static const D3DCOLORVALUE DARKOLIVEGREEN;
    static const D3DCOLORVALUE DARKSEAGREEN;
    static const D3DCOLORVALUE SEAGREEN;
    static const D3DCOLORVALUE MEDIUMSEAGREEN;
    static const D3DCOLORVALUE LIGHTSEAGREEN;
    static const D3DCOLORVALUE PALEGREEN;
    static const D3DCOLORVALUE SPRINGGREEN;
    static const D3DCOLORVALUE LAWNGREEN;
    static const D3DCOLORVALUE GREEN;
    static const D3DCOLORVALUE CHARTREUSE;
    static const D3DCOLORVALUE MEDIUMSPRINGGREEN;
    static const D3DCOLORVALUE GREENYELLOW;
    static const D3DCOLORVALUE LIMEGREEN;
    static const D3DCOLORVALUE YELLOWGREEN;
    static const D3DCOLORVALUE FORESTGREEN;
    static const D3DCOLORVALUE OLIVEDRAB;
    static const D3DCOLORVALUE DARKKHAKI;
    static const D3DCOLORVALUE KHAKI;
    static const D3DCOLORVALUE PALEGOLDENROD;
    static const D3DCOLORVALUE LIGHTGOLDENRODYELLOW;
    static const D3DCOLORVALUE LIGHTYELLOW;
    static const D3DCOLORVALUE YELLOW;
    static const D3DCOLORVALUE GOLD;
    static const D3DCOLORVALUE LIGHTGOLDENROD;
    static const D3DCOLORVALUE GOLDENROD;
    static const D3DCOLORVALUE DARKGOLDENROD;
    static const D3DCOLORVALUE ROSYBROWN;
    static const D3DCOLORVALUE INDIAN;
    static const D3DCOLORVALUE INDIANRED;
    static const D3DCOLORVALUE SADDLEBROWN;
    static const D3DCOLORVALUE SIENNA;
    static const D3DCOLORVALUE PERU;
    static const D3DCOLORVALUE BURLYWOOD;
    static const D3DCOLORVALUE BEIGE;
    static const D3DCOLORVALUE WHEAT;
    static const D3DCOLORVALUE SANDYBROWN;
    static const D3DCOLORVALUE TAN;
    static const D3DCOLORVALUE CHOCOLATE;
    static const D3DCOLORVALUE FIREBRICK;
    static const D3DCOLORVALUE BROWN;
    static const D3DCOLORVALUE DARKSALMON;
    static const D3DCOLORVALUE SALMON;
    static const D3DCOLORVALUE LIGHTSALMON;
    static const D3DCOLORVALUE ORANGE;
    static const D3DCOLORVALUE DARKORANGE;
    static const D3DCOLORVALUE CORAL;
    static const D3DCOLORVALUE LIGHTCORAL;
    static const D3DCOLORVALUE TOMATO;
    static const D3DCOLORVALUE ORANGERED;
    static const D3DCOLORVALUE RED;
    static const D3DCOLORVALUE LIGHTRED;
    static const D3DCOLORVALUE HOTPINK;
    static const D3DCOLORVALUE DEEPPINK;
    static const D3DCOLORVALUE PINK;
    static const D3DCOLORVALUE LIGHTPINK;
    static const D3DCOLORVALUE PALEVIOLETRED;
    static const D3DCOLORVALUE MAROON;
    static const D3DCOLORVALUE MEDIUMVIOLETRED;
    static const D3DCOLORVALUE VIOLETRED;
    static const D3DCOLORVALUE MAGENTA;
    static const D3DCOLORVALUE VIOLET;
    static const D3DCOLORVALUE PLUM;
    static const D3DCOLORVALUE ORCHID;
    static const D3DCOLORVALUE MEDIUMORCHID;
    static const D3DCOLORVALUE DARKORCHID;
    static const D3DCOLORVALUE DARKVIOLET;
    static const D3DCOLORVALUE BLUEVIOLET;
    static const D3DCOLORVALUE PURPLE;
    static const D3DCOLORVALUE MEDIUMPURPLE;
    static const D3DCOLORVALUE THISTLE;
    static const D3DCOLORVALUE DARKGREY;
    static const D3DCOLORVALUE DARKGRAY;
    static const D3DCOLORVALUE DARKBLUE;
    static const D3DCOLORVALUE DARKCYAN;
    static const D3DCOLORVALUE DARKMAGENTA;
    static const D3DCOLORVALUE DARKRED;
    static const D3DCOLORVALUE LIGHTGREEN;
  };
}