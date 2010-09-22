/*#include "Precompile.h"*/
#include "Orientation.h"

#include "Foundation/Math/Constants.h"

using namespace Helium;

#ifndef Z_UP

//
// Y Up
//

const Math::Axis SceneGraph::SideAxis = Math::SingleAxes::X;
const Math::Vector3 SceneGraph::SideVector (1.f, 0.f, 0.f);

const Math::Axis SceneGraph::OutAxis = Math::SingleAxes::Z;
const Math::Vector3 SceneGraph::OutVector (0.f, 0.f, 1.f);

const Math::Axis SceneGraph::UpAxis = Math::SingleAxes::Y;
const Math::Vector3 SceneGraph::UpVector (0.f, 1.f, 0.f);

#else

//
// Z Up
//

const Math::Axis SceneGraph::SideAxis = Math::SingleAxes::Y;
const Math::Vector3 SceneGraph::SideVector (0.f, 1.f, 0.f);

const Math::Axis SceneGraph::OutAxis = Math::SingleAxes::X;
const Math::Vector3 SceneGraph::OutVector (1.f, 0.f, 0.f);

const Math::Axis SceneGraph::UpAxis = Math::SingleAxes::Z;
const Math::Vector3 SceneGraph::UpVector (0.f, 0.f, 1.f);

#endif
