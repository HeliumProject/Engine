/*#include "Precompile.h"*/
#include "Orientation.h"

#include "Foundation/Math/Constants.h"

using namespace Helium;

#ifndef Z_UP

//
// Y Up
//

const Math::Axis Core::SideAxis = Math::SingleAxes::X;
const Math::Vector3 Core::SideVector (1.f, 0.f, 0.f);

const Math::Axis Core::OutAxis = Math::SingleAxes::Z;
const Math::Vector3 Core::OutVector (0.f, 0.f, 1.f);

const Math::Axis Core::UpAxis = Math::SingleAxes::Y;
const Math::Vector3 Core::UpVector (0.f, 1.f, 0.f);

#else

//
// Z Up
//

const Math::Axis Core::SideAxis = Math::SingleAxes::Y;
const Math::Vector3 Core::SideVector (0.f, 1.f, 0.f);

const Math::Axis Core::OutAxis = Math::SingleAxes::X;
const Math::Vector3 Core::OutVector (1.f, 0.f, 0.f);

const Math::Axis Core::UpAxis = Math::SingleAxes::Z;
const Math::Vector3 Core::UpVector (0.f, 0.f, 1.f);

#endif
