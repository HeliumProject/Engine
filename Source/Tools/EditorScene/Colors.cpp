#include "Precompile.h"
#include "Colors.h"

using namespace Helium;
using namespace Helium::Editor;

void Editor::Colors::UnpackColor( uint32_t packed, uint32_t& a, uint32_t& r, uint32_t& g, uint32_t& b )
{
	a = ( packed >> 24 ) & 0xff;
	r = ( packed >> 16) & 0xff;
	g = ( packed >> 8 ) & 0xff;
	b = ( packed & 0xff );
}

Helium::Color Editor::Colors::BlendColor( Helium::Color color1, Helium::Color color2, float32_t weight )
{
	uint32_t a1, a2, r1, r2, g1, g2, b1, b2;

	Editor::Colors::UnpackColor( color1.GetArgb(), a1, r1, g1, b1 );
	Editor::Colors::UnpackColor( color2.GetArgb(), a2, r2, g2, b2 );

	return Helium::Color(
		static_cast< uint8_t >( static_cast< float32_t >( r1 ) + static_cast< float32_t >( r2 - r1 ) * weight + 0.5f ),
		static_cast< uint8_t >( static_cast< float32_t >( g1 ) + static_cast< float32_t >( g2 - g1 ) * weight + 0.5f ),
		static_cast< uint8_t >( static_cast< float32_t >( b1 ) + static_cast< float32_t >( b2 - b1 ) * weight + 0.5f ),
		static_cast< uint8_t >( static_cast< float32_t >( a1 ) + static_cast< float32_t >( a2 - a1 ) * weight + 0.5f ) );
}

const Helium::Color Editor::Colors::SNOW( 255, 250, 250, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GHOSTWHITE( 248, 248, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::WHITESMOKE( 245, 245, 245, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GAINSBORO( 220, 220, 220, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::FLORALWHITE( 255, 250, 240, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::OLDLACE( 253, 245, 230, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LINEN( 250, 240, 230, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ANTIQUEWHITE( 250, 235, 215, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PAPAYAWHIP( 255, 239, 213, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BLANCHEDALMOND( 255, 235, 205, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BISQUE( 255, 228, 196, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PEACHPUFF( 255, 218, 185, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::NAVAJOWHITE( 255, 222, 173, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MOCCASIN( 255, 228, 181, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CORNSILK( 255, 248, 220, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::IVORY( 255, 255, 240, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LEMONCHIFFON( 255, 250, 205, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SEASHELL( 255, 245, 238, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::HONEYDEW( 240, 255, 240, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MINTCREAM( 245, 255, 250, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::AZURE( 240, 255, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ALICEBLUE( 240, 248, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LAVENDER( 230, 230, 250, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LAVENDERBLUSH( 255, 240, 245, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MISTYROSE( 255, 228, 225, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::WHITE( 255, 255, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BLACK( 0, 0, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SILVER( 192, 192, 192, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKSLATEGRAY( 47, 79, 79, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKSLATEGREY( 47, 79, 79, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DIMGRAY( 105, 105, 105, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DIMGREY( 105, 105, 105, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SLATEGRAY( 112, 128, 144, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SLATEGREY( 112, 128, 144, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSLATEGRAY( 119, 136, 153, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSLATEGREY( 119, 136, 153, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GRAY( 190, 190, 190, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GREY( 190, 190, 190, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTGREY( 211, 211, 211, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTGRAY( 211, 211, 211, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MIDNIGHTBLUE( 25, 25, 112, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::NAVY( 0, 0, 128, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::NAVYBLUE( 0, 0, 128, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CORNFLOWERBLUE( 100, 149, 237, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKSLATEBLUE( 72, 61, 139, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SLATEBLUE( 106, 90, 205, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMSLATEBLUE( 123, 104, 238, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSLATEBLUE( 132, 112, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMBLUE( 0, 0, 205, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ROYALBLUE( 65, 105, 225, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BLUE( 0, 0, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DODGERBLUE( 30, 144, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DEEPSKYBLUE( 0, 191, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SKYBLUE( 135, 206, 235, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSKYBLUE( 135, 206, 250, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::STEELBLUE( 70, 130, 180, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSTEELBLUE( 176, 196, 222, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTBLUE( 173, 216, 230, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::POWDERBLUE( 176, 224, 230, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PALETURQUOISE( 175, 238, 238, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKTURQUOISE( 0, 206, 209, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMTURQUOISE( 72, 209, 204, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::TURQUOISE( 64, 224, 208, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CYAN( 0, 255, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTCYAN( 224, 255, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CADETBLUE( 95, 158, 160, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMAQUAMARINE( 102, 205, 170, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::AQUAMARINE( 127, 255, 212, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKGREEN( 0, 100, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKOLIVEGREEN( 85, 107, 47, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKSEAGREEN( 143, 188, 143, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SEAGREEN( 46, 139, 87, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMSEAGREEN( 60, 179, 113, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSEAGREEN( 32, 178, 170, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PALEGREEN( 152, 251, 152, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SPRINGGREEN( 0, 235, 127, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LAWNGREEN( 124, 252, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GREEN( 0, 255, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CHARTREUSE( 127, 255, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMSPRINGGREEN( 0, 250, 154, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GREENYELLOW( 173, 255, 47, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIMEGREEN( 50, 205, 50, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::YELLOWGREEN( 154, 205, 50, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::FORESTGREEN( 44, 149, 44, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::OLIVEDRAB( 107, 142, 35, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKKHAKI( 189, 183, 107, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::KHAKI( 240, 230, 140, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PALEGOLDENROD( 238, 232, 170, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTGOLDENRODYELLOW( 250, 250, 210, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTYELLOW( 255, 255, 224, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::YELLOW( 255, 255, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GOLD( 255, 215, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTGOLDENROD( 238, 221, 130, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::GOLDENROD( 218, 165, 32, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKGOLDENROD( 184, 134, 11, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ROSYBROWN( 188, 143, 143, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::INDIAN( 205, 92, 92, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::INDIANRED( 205, 92, 92, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SADDLEBROWN( 139, 69, 19, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SIENNA( 160, 82, 45, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PERU( 205, 133, 63, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BURLYWOOD( 222, 184, 135, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BEIGE( 245, 245, 220, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::WHEAT( 245, 222, 179, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SANDYBROWN( 244, 164, 96, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::TAN( 210, 180, 140, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CHOCOLATE( 210, 105, 30, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::FIREBRICK( 178, 34, 34, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BROWN( 165, 42, 42, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKSALMON( 233, 150, 122, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::SALMON( 250, 128, 114, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTSALMON( 255, 160, 122, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ORANGE( 255, 165, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKORANGE( 255, 140, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::CORAL( 255, 127, 80, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTCORAL( 240, 128, 128, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::TOMATO( 255, 99, 71, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ORANGERED( 255, 69, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::RED( 255, 0, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTRED( 255, 127, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::HOTPINK( 255, 105, 180, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DEEPPINK( 255, 20, 147, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PINK( 255, 192, 203, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTPINK( 255, 182, 193, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PALEVIOLETRED( 219, 112, 147, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MAROON( 176, 48, 96, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMVIOLETRED( 199, 21, 133, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::VIOLETRED( 208, 32, 144, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MAGENTA( 255, 0, 255, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::VIOLET( 238, 130, 238, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PLUM( 221, 160, 221, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::ORCHID( 218, 112, 214, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMORCHID( 186, 85, 211, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKORCHID( 153, 50, 204, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKVIOLET( 148, 0, 211, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::BLUEVIOLET( 138, 43, 226, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::PURPLE( 160, 32, 240, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::MEDIUMPURPLE( 147, 112, 219, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::THISTLE( 216, 191, 216, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKGREY( 169, 169, 169, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKGRAY( 169, 169, 169, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKBLUE( 0, 0, 139, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKCYAN( 0, 139, 139, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKMAGENTA( 139, 0, 139, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::DARKRED( 139, 0, 0, static_cast< uint8_t >( 255 ) );
const Helium::Color Editor::Colors::LIGHTGREEN( 144, 238, 144, static_cast< uint8_t >( 255 ) );
