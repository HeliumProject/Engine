#include "Precompile.h"
#include "Orientation.h"

#include "Foundation/Math/Constants.h"

#ifndef Z_UP

//
// Y Up
//

const Math::Axis Luna::SideAxis = Math::SingleAxes::X;
const Math::Vector3 Luna::SideVector (1.f, 0.f, 0.f);

const Math::Axis Luna::OutAxis = Math::SingleAxes::Z;
const Math::Vector3 Luna::OutVector (0.f, 0.f, 1.f);

const Math::Axis Luna::UpAxis = Math::SingleAxes::Y;
const Math::Vector3 Luna::UpVector (0.f, 1.f, 0.f);

#else

//
// Z Up
//

const Math::Axis Luna::SideAxis = Math::SingleAxes::Y;
const Math::Vector3 Luna::SideVector (0.f, 1.f, 0.f);

const Math::Axis Luna::OutAxis = Math::SingleAxes::X;
const Math::Vector3 Luna::OutVector (1.f, 0.f, 0.f);

const Math::Axis Luna::UpAxis = Math::SingleAxes::Z;
const Math::Vector3 Luna::UpVector (0.f, 0.f, 1.f);

#endif
