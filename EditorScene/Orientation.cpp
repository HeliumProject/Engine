#include "Precompile.h"
#include "Orientation.h"

#include "Math/Axes.h"

using namespace Helium;

#ifndef Z_UP

//
// Y Up
//

const Axis Editor::SideAxis = SingleAxes::X;
const Vector3 Editor::SideVector (1.f, 0.f, 0.f);

const Axis Editor::OutAxis = SingleAxes::Z;
const Vector3 Editor::OutVector (0.f, 0.f, 1.f);

const Axis Editor::UpAxis = SingleAxes::Y;
const Vector3 Editor::UpVector (0.f, 1.f, 0.f);

#else

//
// Z Up
//

const Axis Editor::SideAxis = SingleAxes::Y;
const Vector3 Editor::SideVector (0.f, 1.f, 0.f);

const Axis Editor::OutAxis = SingleAxes::X;
const Vector3 Editor::OutVector (1.f, 0.f, 0.f);

const Axis Editor::UpAxis = SingleAxes::Z;
const Vector3 Editor::UpVector (0.f, 0.f, 1.f);

#endif
