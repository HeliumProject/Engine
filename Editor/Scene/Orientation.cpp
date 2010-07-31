#include "Precompile.h"
#include "Orientation.h"

#include "Foundation/Math/Constants.h"

using namespace Helium;

#ifndef Z_UP

//
// Y Up
//

const Math::Axis Editor::SideAxis = Math::SingleAxes::X;
const Math::Vector3 Editor::SideVector (1.f, 0.f, 0.f);

const Math::Axis Editor::OutAxis = Math::SingleAxes::Z;
const Math::Vector3 Editor::OutVector (0.f, 0.f, 1.f);

const Math::Axis Editor::UpAxis = Math::SingleAxes::Y;
const Math::Vector3 Editor::UpVector (0.f, 1.f, 0.f);

#else

//
// Z Up
//

const Math::Axis Editor::SideAxis = Math::SingleAxes::Y;
const Math::Vector3 Editor::SideVector (0.f, 1.f, 0.f);

const Math::Axis Editor::OutAxis = Math::SingleAxes::X;
const Math::Vector3 Editor::OutVector (1.f, 0.f, 0.f);

const Math::Axis Editor::UpAxis = Math::SingleAxes::Z;
const Math::Vector3 Editor::UpVector (0.f, 0.f, 1.f);

#endif
