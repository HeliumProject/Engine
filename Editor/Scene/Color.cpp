#include "Precompile.h"
#include "Color.h"

using namespace Editor;

DWORD Color::ColorValueToColor( D3DCOLORVALUE value )
{
  return D3DCOLOR_ARGB((DWORD)(value.a * 255.f), (DWORD)(value.r * 255.f), (DWORD)(value.g * 255.f), (DWORD)(value.b * 255.f));
}

D3DCOLORVALUE Color::ColorToColorValue( DWORD argb )
{
  DWORD a, r, g, b;
  UnpackColor(argb, a, r, g, b);

  D3DCOLORVALUE value;
  value.a = a / 255.f;
  value.r = r / 255.f;
  value.g = g / 255.f;
  value.b = b / 255.f;
  return value;
}

D3DCOLORVALUE Color::ColorToColorValue( DWORD a, DWORD r, DWORD g, DWORD b )
{
  D3DCOLORVALUE value;
  value.a = a / 255.f;
  value.r = r / 255.f;
  value.g = g / 255.f;
  value.b = b / 255.f;
  return value;
}

void Color::UnpackColor(DWORD packed, DWORD& a, DWORD& r, DWORD& g, DWORD& b)
{
  a = (packed >> 24) & 0xff;
  r = (packed >> 16) & 0xff;
  g = (packed >> 8) & 0xff;
  b = (packed & 0xff);
}

DWORD Color::BlendColor(DWORD color1, DWORD color2, float weight)
{
  DWORD a1, a2, r1, r2, g1, g2, b1, b2;

  Color::UnpackColor(color1, a1, r1, g1, b1);
  Color::UnpackColor(color2, a2, r2, g2, b2);

  DWORD a = (DWORD)((float)(a1) * (1.0f - weight)) + (DWORD)((float)(a2) * weight);
  DWORD r = (DWORD)((float)(r1) * (1.0f - weight)) + (DWORD)((float)(r2) * weight);
  DWORD g = (DWORD)((float)(g1) * (1.0f - weight)) + (DWORD)((float)(g2) * weight);
  DWORD b = (DWORD)((float)(b1) * (1.0f - weight)) + (DWORD)((float)(b2) * weight);

  return D3DCOLOR_ARGB(a, r, g, b);
}

const D3DCOLORVALUE Color::SNOW = ColorToColorValue(255, 255, 250, 250);
const D3DCOLORVALUE Color::GHOSTWHITE = ColorToColorValue(255, 248, 248, 255);
const D3DCOLORVALUE Color::WHITESMOKE = ColorToColorValue(255, 245, 245, 245);
const D3DCOLORVALUE Color::GAINSBORO = ColorToColorValue(255, 220, 220, 220);
const D3DCOLORVALUE Color::FLORALWHITE = ColorToColorValue(255, 255, 250, 240);
const D3DCOLORVALUE Color::OLDLACE = ColorToColorValue(255, 253, 245, 230);
const D3DCOLORVALUE Color::LINEN = ColorToColorValue(255, 250, 240, 230);
const D3DCOLORVALUE Color::ANTIQUEWHITE = ColorToColorValue(255, 250, 235, 215);
const D3DCOLORVALUE Color::PAPAYAWHIP = ColorToColorValue(255, 255, 239, 213);
const D3DCOLORVALUE Color::BLANCHEDALMOND = ColorToColorValue(255, 255, 235, 205);
const D3DCOLORVALUE Color::BISQUE = ColorToColorValue(255, 255, 228, 196);
const D3DCOLORVALUE Color::PEACHPUFF = ColorToColorValue(255, 255, 218, 185);
const D3DCOLORVALUE Color::NAVAJOWHITE = ColorToColorValue(255, 255, 222, 173);
const D3DCOLORVALUE Color::MOCCASIN = ColorToColorValue(255, 255, 228, 181);
const D3DCOLORVALUE Color::CORNSILK = ColorToColorValue(255, 255, 248, 220);
const D3DCOLORVALUE Color::IVORY = ColorToColorValue(255, 255, 255, 240);
const D3DCOLORVALUE Color::LEMONCHIFFON = ColorToColorValue(255, 255, 250, 205);
const D3DCOLORVALUE Color::SEASHELL = ColorToColorValue(255, 255, 245, 238);
const D3DCOLORVALUE Color::HONEYDEW = ColorToColorValue(255, 240, 255, 240);
const D3DCOLORVALUE Color::MINTCREAM = ColorToColorValue(255, 245, 255, 250);
const D3DCOLORVALUE Color::AZURE = ColorToColorValue(255, 240, 255, 255);
const D3DCOLORVALUE Color::ALICEBLUE = ColorToColorValue(255, 240, 248, 255);
const D3DCOLORVALUE Color::LAVENDER = ColorToColorValue(255, 230, 230, 250);
const D3DCOLORVALUE Color::LAVENDERBLUSH = ColorToColorValue(255, 255, 240, 245);
const D3DCOLORVALUE Color::MISTYROSE = ColorToColorValue(255, 255, 228, 225);
const D3DCOLORVALUE Color::WHITE = ColorToColorValue(255, 255, 255, 255);
const D3DCOLORVALUE Color::BLACK = ColorToColorValue(255, 0, 0, 0);
const D3DCOLORVALUE Color::SILVER = ColorToColorValue(255, 192, 192, 192);
const D3DCOLORVALUE Color::DARKSLATEGRAY = ColorToColorValue(255, 47, 79, 79);
const D3DCOLORVALUE Color::DARKSLATEGREY = ColorToColorValue(255, 47, 79, 79);
const D3DCOLORVALUE Color::DIMGRAY = ColorToColorValue(255, 105, 105, 105);
const D3DCOLORVALUE Color::DIMGREY = ColorToColorValue(255, 105, 105, 105);
const D3DCOLORVALUE Color::SLATEGRAY = ColorToColorValue(255, 112, 128, 144);
const D3DCOLORVALUE Color::SLATEGREY = ColorToColorValue(255, 112, 128, 144);
const D3DCOLORVALUE Color::LIGHTSLATEGRAY = ColorToColorValue(255, 119, 136, 153);
const D3DCOLORVALUE Color::LIGHTSLATEGREY = ColorToColorValue(255, 119, 136, 153);
const D3DCOLORVALUE Color::GRAY = ColorToColorValue(255, 190, 190, 190);
const D3DCOLORVALUE Color::GREY = ColorToColorValue(255, 190, 190, 190);
const D3DCOLORVALUE Color::LIGHTGREY = ColorToColorValue(255, 211, 211, 211);
const D3DCOLORVALUE Color::LIGHTGRAY = ColorToColorValue(255, 211, 211, 211);
const D3DCOLORVALUE Color::MIDNIGHTBLUE = ColorToColorValue(255, 25, 25, 112);
const D3DCOLORVALUE Color::NAVY = ColorToColorValue(255, 0, 0, 128);
const D3DCOLORVALUE Color::NAVYBLUE = ColorToColorValue(255, 0, 0, 128);
const D3DCOLORVALUE Color::CORNFLOWERBLUE = ColorToColorValue(255, 100, 149, 237);
const D3DCOLORVALUE Color::DARKSLATEBLUE = ColorToColorValue(255, 72, 61, 139);
const D3DCOLORVALUE Color::SLATEBLUE = ColorToColorValue(255, 106, 90, 205);
const D3DCOLORVALUE Color::MEDIUMSLATEBLUE = ColorToColorValue(255, 123, 104, 238);
const D3DCOLORVALUE Color::LIGHTSLATEBLUE = ColorToColorValue(255, 132, 112, 255);
const D3DCOLORVALUE Color::MEDIUMBLUE = ColorToColorValue(255, 0, 0, 205);
const D3DCOLORVALUE Color::ROYALBLUE = ColorToColorValue(255, 65, 105, 225);
const D3DCOLORVALUE Color::BLUE = ColorToColorValue(255, 0, 0, 255);
const D3DCOLORVALUE Color::DODGERBLUE = ColorToColorValue(255, 30, 144, 255);
const D3DCOLORVALUE Color::DEEPSKYBLUE = ColorToColorValue(255, 0, 191, 255);
const D3DCOLORVALUE Color::SKYBLUE = ColorToColorValue(255, 135, 206, 235);
const D3DCOLORVALUE Color::LIGHTSKYBLUE = ColorToColorValue(255, 135, 206, 250);
const D3DCOLORVALUE Color::STEELBLUE = ColorToColorValue(255, 70, 130, 180);
const D3DCOLORVALUE Color::LIGHTSTEELBLUE = ColorToColorValue(255, 176, 196, 222);
const D3DCOLORVALUE Color::LIGHTBLUE = ColorToColorValue(255, 173, 216, 230);
const D3DCOLORVALUE Color::POWDERBLUE = ColorToColorValue(255, 176, 224, 230);
const D3DCOLORVALUE Color::PALETURQUOISE = ColorToColorValue(255, 175, 238, 238);
const D3DCOLORVALUE Color::DARKTURQUOISE = ColorToColorValue(255, 0, 206, 209);
const D3DCOLORVALUE Color::MEDIUMTURQUOISE = ColorToColorValue(255, 72, 209, 204);
const D3DCOLORVALUE Color::TURQUOISE = ColorToColorValue(255, 64, 224, 208);
const D3DCOLORVALUE Color::CYAN = ColorToColorValue(255, 0, 255, 255);
const D3DCOLORVALUE Color::LIGHTCYAN = ColorToColorValue(255, 224, 255, 255);
const D3DCOLORVALUE Color::CADETBLUE = ColorToColorValue(255, 95, 158, 160);
const D3DCOLORVALUE Color::MEDIUMAQUAMARINE = ColorToColorValue(255, 102, 205, 170);
const D3DCOLORVALUE Color::AQUAMARINE = ColorToColorValue(255, 127, 255, 212);
const D3DCOLORVALUE Color::DARKGREEN = ColorToColorValue(255, 0, 100, 0);
const D3DCOLORVALUE Color::DARKOLIVEGREEN = ColorToColorValue(255, 85, 107, 47);
const D3DCOLORVALUE Color::DARKSEAGREEN = ColorToColorValue(255, 143, 188, 143);
const D3DCOLORVALUE Color::SEAGREEN = ColorToColorValue(255, 46, 139, 87);
const D3DCOLORVALUE Color::MEDIUMSEAGREEN = ColorToColorValue(255, 60, 179, 113);
const D3DCOLORVALUE Color::LIGHTSEAGREEN = ColorToColorValue(255, 32, 178, 170);
const D3DCOLORVALUE Color::PALEGREEN = ColorToColorValue(255, 152, 251, 152);
const D3DCOLORVALUE Color::SPRINGGREEN = ColorToColorValue(255, 0, 235, 127);
const D3DCOLORVALUE Color::LAWNGREEN = ColorToColorValue(255, 124, 252, 0);
const D3DCOLORVALUE Color::GREEN = ColorToColorValue(255, 0, 255, 0);
const D3DCOLORVALUE Color::CHARTREUSE = ColorToColorValue(255, 127, 255, 0);
const D3DCOLORVALUE Color::MEDIUMSPRINGGREEN = ColorToColorValue(255, 0, 250, 154);
const D3DCOLORVALUE Color::GREENYELLOW = ColorToColorValue(255, 173, 255, 47);
const D3DCOLORVALUE Color::LIMEGREEN = ColorToColorValue(255, 50, 205, 50);
const D3DCOLORVALUE Color::YELLOWGREEN = ColorToColorValue(255, 154, 205, 50);
const D3DCOLORVALUE Color::FORESTGREEN = ColorToColorValue(255, 44, 149, 44);
const D3DCOLORVALUE Color::OLIVEDRAB = ColorToColorValue(255, 107, 142, 35);
const D3DCOLORVALUE Color::DARKKHAKI = ColorToColorValue(255, 189, 183, 107);
const D3DCOLORVALUE Color::KHAKI = ColorToColorValue(255, 240, 230, 140);
const D3DCOLORVALUE Color::PALEGOLDENROD = ColorToColorValue(255, 238, 232, 170);
const D3DCOLORVALUE Color::LIGHTGOLDENRODYELLOW = ColorToColorValue(255, 250, 250, 210);
const D3DCOLORVALUE Color::LIGHTYELLOW = ColorToColorValue(255, 255, 255, 224);
const D3DCOLORVALUE Color::YELLOW = ColorToColorValue(255, 255, 255, 0);
const D3DCOLORVALUE Color::GOLD = ColorToColorValue(255, 255, 215, 0);
const D3DCOLORVALUE Color::LIGHTGOLDENROD = ColorToColorValue(255, 238, 221, 130);
const D3DCOLORVALUE Color::GOLDENROD = ColorToColorValue(255, 218, 165, 32);
const D3DCOLORVALUE Color::DARKGOLDENROD = ColorToColorValue(255, 184, 134, 11);
const D3DCOLORVALUE Color::ROSYBROWN = ColorToColorValue(255, 188, 143, 143);
const D3DCOLORVALUE Color::INDIAN = ColorToColorValue(255, 205, 92, 92);
const D3DCOLORVALUE Color::INDIANRED = ColorToColorValue(255, 205, 92, 92);
const D3DCOLORVALUE Color::SADDLEBROWN = ColorToColorValue(255, 139, 69, 19);
const D3DCOLORVALUE Color::SIENNA = ColorToColorValue(255, 160, 82, 45);
const D3DCOLORVALUE Color::PERU = ColorToColorValue(255, 205, 133, 63);
const D3DCOLORVALUE Color::BURLYWOOD = ColorToColorValue(255, 222, 184, 135);
const D3DCOLORVALUE Color::BEIGE = ColorToColorValue(255, 245, 245, 220);
const D3DCOLORVALUE Color::WHEAT = ColorToColorValue(255, 245, 222, 179);
const D3DCOLORVALUE Color::SANDYBROWN = ColorToColorValue(255, 244, 164, 96);
const D3DCOLORVALUE Color::TAN = ColorToColorValue(255, 210, 180, 140);
const D3DCOLORVALUE Color::CHOCOLATE = ColorToColorValue(255, 210, 105, 30);
const D3DCOLORVALUE Color::FIREBRICK = ColorToColorValue(255, 178, 34, 34);
const D3DCOLORVALUE Color::BROWN = ColorToColorValue(255, 165, 42, 42);
const D3DCOLORVALUE Color::DARKSALMON = ColorToColorValue(255, 233, 150, 122);
const D3DCOLORVALUE Color::SALMON = ColorToColorValue(255, 250, 128, 114);
const D3DCOLORVALUE Color::LIGHTSALMON = ColorToColorValue(255, 255, 160, 122);
const D3DCOLORVALUE Color::ORANGE = ColorToColorValue(255, 255, 165, 0);
const D3DCOLORVALUE Color::DARKORANGE = ColorToColorValue(255, 255, 140, 0);
const D3DCOLORVALUE Color::CORAL = ColorToColorValue(255, 255, 127, 80);
const D3DCOLORVALUE Color::LIGHTCORAL = ColorToColorValue(255, 240, 128, 128);
const D3DCOLORVALUE Color::TOMATO = ColorToColorValue(255, 255, 99, 71);
const D3DCOLORVALUE Color::ORANGERED = ColorToColorValue(255, 255, 69, 0);
const D3DCOLORVALUE Color::RED = ColorToColorValue(255, 255, 0, 0);
const D3DCOLORVALUE Color::LIGHTRED = ColorToColorValue(255, 255, 127, 0);
const D3DCOLORVALUE Color::HOTPINK = ColorToColorValue(255, 255, 105, 180);
const D3DCOLORVALUE Color::DEEPPINK = ColorToColorValue(255, 255, 20, 147);
const D3DCOLORVALUE Color::PINK = ColorToColorValue(255, 255, 192, 203);
const D3DCOLORVALUE Color::LIGHTPINK = ColorToColorValue(255, 255, 182, 193);
const D3DCOLORVALUE Color::PALEVIOLETRED = ColorToColorValue(255, 219, 112, 147);
const D3DCOLORVALUE Color::MAROON = ColorToColorValue(255, 176, 48, 96);
const D3DCOLORVALUE Color::MEDIUMVIOLETRED = ColorToColorValue(255, 199, 21, 133);
const D3DCOLORVALUE Color::VIOLETRED = ColorToColorValue(255, 208, 32, 144);
const D3DCOLORVALUE Color::MAGENTA = ColorToColorValue(255, 255, 0, 255);
const D3DCOLORVALUE Color::VIOLET = ColorToColorValue(255, 238, 130, 238);
const D3DCOLORVALUE Color::PLUM = ColorToColorValue(255, 221, 160, 221);
const D3DCOLORVALUE Color::ORCHID = ColorToColorValue(255, 218, 112, 214);
const D3DCOLORVALUE Color::MEDIUMORCHID = ColorToColorValue(255, 186, 85, 211);
const D3DCOLORVALUE Color::DARKORCHID = ColorToColorValue(255, 153, 50, 204);
const D3DCOLORVALUE Color::DARKVIOLET = ColorToColorValue(255, 148, 0, 211);
const D3DCOLORVALUE Color::BLUEVIOLET = ColorToColorValue(255, 138, 43, 226);
const D3DCOLORVALUE Color::PURPLE = ColorToColorValue(255, 160, 32, 240);
const D3DCOLORVALUE Color::MEDIUMPURPLE = ColorToColorValue(255, 147, 112, 219);
const D3DCOLORVALUE Color::THISTLE = ColorToColorValue(255, 216, 191, 216);
const D3DCOLORVALUE Color::DARKGREY = ColorToColorValue(255, 169, 169, 169);
const D3DCOLORVALUE Color::DARKGRAY = ColorToColorValue(255, 169, 169, 169);
const D3DCOLORVALUE Color::DARKBLUE = ColorToColorValue(255, 0, 0, 139);
const D3DCOLORVALUE Color::DARKCYAN = ColorToColorValue(255, 0, 139, 139);
const D3DCOLORVALUE Color::DARKMAGENTA = ColorToColorValue(255, 139, 0, 139);
const D3DCOLORVALUE Color::DARKRED = ColorToColorValue(255, 139, 0, 0);
const D3DCOLORVALUE Color::LIGHTGREEN = ColorToColorValue(255, 144, 238, 144);
