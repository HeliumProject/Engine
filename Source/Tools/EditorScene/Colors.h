#pragma once

#include "MathSimd/Color.h"

#include "EditorScene/API.h"

namespace Helium
{
	namespace Editor
	{
		class Colors
		{
		public:
			// unpack a color into components
			static void UnpackColor( uint32_t packed, uint32_t& a, uint32_t& r, uint32_t& g, uint32_t& b );

			// blend color with a weight
			static Helium::Color BlendColor( Helium::Color color1, Helium::Color color2, float32_t weight );

			static const Helium::Color SNOW;
			static const Helium::Color GHOSTWHITE;
			static const Helium::Color WHITESMOKE;
			static const Helium::Color GAINSBORO;
			static const Helium::Color FLORALWHITE;
			static const Helium::Color OLDLACE;
			static const Helium::Color LINEN;
			static const Helium::Color ANTIQUEWHITE;
			static const Helium::Color PAPAYAWHIP;
			static const Helium::Color BLANCHEDALMOND;
			static const Helium::Color BISQUE;
			static const Helium::Color PEACHPUFF;
			static const Helium::Color NAVAJOWHITE;
			static const Helium::Color MOCCASIN;
			static const Helium::Color CORNSILK;
			static const Helium::Color IVORY;
			static const Helium::Color LEMONCHIFFON;
			static const Helium::Color SEASHELL;
			static const Helium::Color HONEYDEW;
			static const Helium::Color MINTCREAM;
			static const Helium::Color AZURE;
			static const Helium::Color ALICEBLUE;
			static const Helium::Color LAVENDER;
			static const Helium::Color LAVENDERBLUSH;
			static const Helium::Color MISTYROSE;
			static const Helium::Color WHITE;
			static const Helium::Color BLACK;
			static const Helium::Color SILVER;
			static const Helium::Color DARKSLATEGRAY;
			static const Helium::Color DARKSLATEGREY;
			static const Helium::Color DIMGRAY;
			static const Helium::Color DIMGREY;
			static const Helium::Color SLATEGRAY;
			static const Helium::Color SLATEGREY;
			static const Helium::Color LIGHTSLATEGRAY;
			static const Helium::Color LIGHTSLATEGREY;
			static const Helium::Color GRAY;
			static const Helium::Color GREY;
			static const Helium::Color LIGHTGREY;
			static const Helium::Color LIGHTGRAY;
			static const Helium::Color MIDNIGHTBLUE;
			static const Helium::Color NAVY;
			static const Helium::Color NAVYBLUE;
			static const Helium::Color CORNFLOWERBLUE;
			static const Helium::Color DARKSLATEBLUE;
			static const Helium::Color SLATEBLUE;
			static const Helium::Color MEDIUMSLATEBLUE;
			static const Helium::Color LIGHTSLATEBLUE;
			static const Helium::Color MEDIUMBLUE;
			static const Helium::Color ROYALBLUE;
			static const Helium::Color BLUE;
			static const Helium::Color DODGERBLUE;
			static const Helium::Color DEEPSKYBLUE;
			static const Helium::Color SKYBLUE;
			static const Helium::Color LIGHTSKYBLUE;
			static const Helium::Color STEELBLUE;
			static const Helium::Color LIGHTSTEELBLUE;
			static const Helium::Color LIGHTBLUE;
			static const Helium::Color POWDERBLUE;
			static const Helium::Color PALETURQUOISE;
			static const Helium::Color DARKTURQUOISE;
			static const Helium::Color MEDIUMTURQUOISE;
			static const Helium::Color TURQUOISE;
			static const Helium::Color CYAN;
			static const Helium::Color LIGHTCYAN;
			static const Helium::Color CADETBLUE;
			static const Helium::Color MEDIUMAQUAMARINE;
			static const Helium::Color AQUAMARINE;
			static const Helium::Color DARKGREEN;
			static const Helium::Color DARKOLIVEGREEN;
			static const Helium::Color DARKSEAGREEN;
			static const Helium::Color SEAGREEN;
			static const Helium::Color MEDIUMSEAGREEN;
			static const Helium::Color LIGHTSEAGREEN;
			static const Helium::Color PALEGREEN;
			static const Helium::Color SPRINGGREEN;
			static const Helium::Color LAWNGREEN;
			static const Helium::Color GREEN;
			static const Helium::Color CHARTREUSE;
			static const Helium::Color MEDIUMSPRINGGREEN;
			static const Helium::Color GREENYELLOW;
			static const Helium::Color LIMEGREEN;
			static const Helium::Color YELLOWGREEN;
			static const Helium::Color FORESTGREEN;
			static const Helium::Color OLIVEDRAB;
			static const Helium::Color DARKKHAKI;
			static const Helium::Color KHAKI;
			static const Helium::Color PALEGOLDENROD;
			static const Helium::Color LIGHTGOLDENRODYELLOW;
			static const Helium::Color LIGHTYELLOW;
			static const Helium::Color YELLOW;
			static const Helium::Color GOLD;
			static const Helium::Color LIGHTGOLDENROD;
			static const Helium::Color GOLDENROD;
			static const Helium::Color DARKGOLDENROD;
			static const Helium::Color ROSYBROWN;
			static const Helium::Color INDIAN;
			static const Helium::Color INDIANRED;
			static const Helium::Color SADDLEBROWN;
			static const Helium::Color SIENNA;
			static const Helium::Color PERU;
			static const Helium::Color BURLYWOOD;
			static const Helium::Color BEIGE;
			static const Helium::Color WHEAT;
			static const Helium::Color SANDYBROWN;
			static const Helium::Color TAN;
			static const Helium::Color CHOCOLATE;
			static const Helium::Color FIREBRICK;
			static const Helium::Color BROWN;
			static const Helium::Color DARKSALMON;
			static const Helium::Color SALMON;
			static const Helium::Color LIGHTSALMON;
			static const Helium::Color ORANGE;
			static const Helium::Color DARKORANGE;
			static const Helium::Color CORAL;
			static const Helium::Color LIGHTCORAL;
			static const Helium::Color TOMATO;
			static const Helium::Color ORANGERED;
			static const Helium::Color RED;
			static const Helium::Color LIGHTRED;
			static const Helium::Color HOTPINK;
			static const Helium::Color DEEPPINK;
			static const Helium::Color PINK;
			static const Helium::Color LIGHTPINK;
			static const Helium::Color PALEVIOLETRED;
			static const Helium::Color MAROON;
			static const Helium::Color MEDIUMVIOLETRED;
			static const Helium::Color VIOLETRED;
			static const Helium::Color MAGENTA;
			static const Helium::Color VIOLET;
			static const Helium::Color PLUM;
			static const Helium::Color ORCHID;
			static const Helium::Color MEDIUMORCHID;
			static const Helium::Color DARKORCHID;
			static const Helium::Color DARKVIOLET;
			static const Helium::Color BLUEVIOLET;
			static const Helium::Color PURPLE;
			static const Helium::Color MEDIUMPURPLE;
			static const Helium::Color THISTLE;
			static const Helium::Color DARKGREY;
			static const Helium::Color DARKGRAY;
			static const Helium::Color DARKBLUE;
			static const Helium::Color DARKCYAN;
			static const Helium::Color DARKMAGENTA;
			static const Helium::Color DARKRED;
			static const Helium::Color LIGHTGREEN;
		};
	}
}