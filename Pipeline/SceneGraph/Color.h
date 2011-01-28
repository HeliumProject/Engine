#pragma once

#include "Rendering/Color.h"

#include "Pipeline/API.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Color
        {
        public:
            // unpack a color into components
            static void UnpackColor( uint32_t packed, uint32_t& a, uint32_t& r, uint32_t& g, uint32_t& b );

            // blend color with a weight
            static Lunar::Color BlendColor( Lunar::Color color1, Lunar::Color color2, float32_t weight );

            static const Lunar::Color SNOW;
            static const Lunar::Color GHOSTWHITE;
            static const Lunar::Color WHITESMOKE;
            static const Lunar::Color GAINSBORO;
            static const Lunar::Color FLORALWHITE;
            static const Lunar::Color OLDLACE;
            static const Lunar::Color LINEN;
            static const Lunar::Color ANTIQUEWHITE;
            static const Lunar::Color PAPAYAWHIP;
            static const Lunar::Color BLANCHEDALMOND;
            static const Lunar::Color BISQUE;
            static const Lunar::Color PEACHPUFF;
            static const Lunar::Color NAVAJOWHITE;
            static const Lunar::Color MOCCASIN;
            static const Lunar::Color CORNSILK;
            static const Lunar::Color IVORY;
            static const Lunar::Color LEMONCHIFFON;
            static const Lunar::Color SEASHELL;
            static const Lunar::Color HONEYDEW;
            static const Lunar::Color MINTCREAM;
            static const Lunar::Color AZURE;
            static const Lunar::Color ALICEBLUE;
            static const Lunar::Color LAVENDER;
            static const Lunar::Color LAVENDERBLUSH;
            static const Lunar::Color MISTYROSE;
            static const Lunar::Color WHITE;
            static const Lunar::Color BLACK;
            static const Lunar::Color SILVER;
            static const Lunar::Color DARKSLATEGRAY;
            static const Lunar::Color DARKSLATEGREY;
            static const Lunar::Color DIMGRAY;
            static const Lunar::Color DIMGREY;
            static const Lunar::Color SLATEGRAY;
            static const Lunar::Color SLATEGREY;
            static const Lunar::Color LIGHTSLATEGRAY;
            static const Lunar::Color LIGHTSLATEGREY;
            static const Lunar::Color GRAY;
            static const Lunar::Color GREY;
            static const Lunar::Color LIGHTGREY;
            static const Lunar::Color LIGHTGRAY;
            static const Lunar::Color MIDNIGHTBLUE;
            static const Lunar::Color NAVY;
            static const Lunar::Color NAVYBLUE;
            static const Lunar::Color CORNFLOWERBLUE;
            static const Lunar::Color DARKSLATEBLUE;
            static const Lunar::Color SLATEBLUE;
            static const Lunar::Color MEDIUMSLATEBLUE;
            static const Lunar::Color LIGHTSLATEBLUE;
            static const Lunar::Color MEDIUMBLUE;
            static const Lunar::Color ROYALBLUE;
            static const Lunar::Color BLUE;
            static const Lunar::Color DODGERBLUE;
            static const Lunar::Color DEEPSKYBLUE;
            static const Lunar::Color SKYBLUE;
            static const Lunar::Color LIGHTSKYBLUE;
            static const Lunar::Color STEELBLUE;
            static const Lunar::Color LIGHTSTEELBLUE;
            static const Lunar::Color LIGHTBLUE;
            static const Lunar::Color POWDERBLUE;
            static const Lunar::Color PALETURQUOISE;
            static const Lunar::Color DARKTURQUOISE;
            static const Lunar::Color MEDIUMTURQUOISE;
            static const Lunar::Color TURQUOISE;
            static const Lunar::Color CYAN;
            static const Lunar::Color LIGHTCYAN;
            static const Lunar::Color CADETBLUE;
            static const Lunar::Color MEDIUMAQUAMARINE;
            static const Lunar::Color AQUAMARINE;
            static const Lunar::Color DARKGREEN;
            static const Lunar::Color DARKOLIVEGREEN;
            static const Lunar::Color DARKSEAGREEN;
            static const Lunar::Color SEAGREEN;
            static const Lunar::Color MEDIUMSEAGREEN;
            static const Lunar::Color LIGHTSEAGREEN;
            static const Lunar::Color PALEGREEN;
            static const Lunar::Color SPRINGGREEN;
            static const Lunar::Color LAWNGREEN;
            static const Lunar::Color GREEN;
            static const Lunar::Color CHARTREUSE;
            static const Lunar::Color MEDIUMSPRINGGREEN;
            static const Lunar::Color GREENYELLOW;
            static const Lunar::Color LIMEGREEN;
            static const Lunar::Color YELLOWGREEN;
            static const Lunar::Color FORESTGREEN;
            static const Lunar::Color OLIVEDRAB;
            static const Lunar::Color DARKKHAKI;
            static const Lunar::Color KHAKI;
            static const Lunar::Color PALEGOLDENROD;
            static const Lunar::Color LIGHTGOLDENRODYELLOW;
            static const Lunar::Color LIGHTYELLOW;
            static const Lunar::Color YELLOW;
            static const Lunar::Color GOLD;
            static const Lunar::Color LIGHTGOLDENROD;
            static const Lunar::Color GOLDENROD;
            static const Lunar::Color DARKGOLDENROD;
            static const Lunar::Color ROSYBROWN;
            static const Lunar::Color INDIAN;
            static const Lunar::Color INDIANRED;
            static const Lunar::Color SADDLEBROWN;
            static const Lunar::Color SIENNA;
            static const Lunar::Color PERU;
            static const Lunar::Color BURLYWOOD;
            static const Lunar::Color BEIGE;
            static const Lunar::Color WHEAT;
            static const Lunar::Color SANDYBROWN;
            static const Lunar::Color TAN;
            static const Lunar::Color CHOCOLATE;
            static const Lunar::Color FIREBRICK;
            static const Lunar::Color BROWN;
            static const Lunar::Color DARKSALMON;
            static const Lunar::Color SALMON;
            static const Lunar::Color LIGHTSALMON;
            static const Lunar::Color ORANGE;
            static const Lunar::Color DARKORANGE;
            static const Lunar::Color CORAL;
            static const Lunar::Color LIGHTCORAL;
            static const Lunar::Color TOMATO;
            static const Lunar::Color ORANGERED;
            static const Lunar::Color RED;
            static const Lunar::Color LIGHTRED;
            static const Lunar::Color HOTPINK;
            static const Lunar::Color DEEPPINK;
            static const Lunar::Color PINK;
            static const Lunar::Color LIGHTPINK;
            static const Lunar::Color PALEVIOLETRED;
            static const Lunar::Color MAROON;
            static const Lunar::Color MEDIUMVIOLETRED;
            static const Lunar::Color VIOLETRED;
            static const Lunar::Color MAGENTA;
            static const Lunar::Color VIOLET;
            static const Lunar::Color PLUM;
            static const Lunar::Color ORCHID;
            static const Lunar::Color MEDIUMORCHID;
            static const Lunar::Color DARKORCHID;
            static const Lunar::Color DARKVIOLET;
            static const Lunar::Color BLUEVIOLET;
            static const Lunar::Color PURPLE;
            static const Lunar::Color MEDIUMPURPLE;
            static const Lunar::Color THISTLE;
            static const Lunar::Color DARKGREY;
            static const Lunar::Color DARKGRAY;
            static const Lunar::Color DARKBLUE;
            static const Lunar::Color DARKCYAN;
            static const Lunar::Color DARKMAGENTA;
            static const Lunar::Color DARKRED;
            static const Lunar::Color LIGHTGREEN;
        };
    }
}