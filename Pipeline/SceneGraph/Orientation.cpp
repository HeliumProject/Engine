/*#include "Precompile.h"*/
#include "Orientation.h"

#include "Foundation/Math/Constants.h"

using namespace Helium;

#ifndef Z_UP

//
// Y Up
//

const Axis SceneGraph::SideAxis = SingleAxes::X;
const Vector3 SceneGraph::SideVector (1.f, 0.f, 0.f);

const Axis SceneGraph::OutAxis = SingleAxes::Z;
const Vector3 SceneGraph::OutVector (0.f, 0.f, 1.f);

const Axis SceneGraph::UpAxis = SingleAxes::Y;
const Vector3 SceneGraph::UpVector (0.f, 1.f, 0.f);

#else

//
// Z Up
//

const Axis SceneGraph::SideAxis = SingleAxes::Y;
const Vector3 SceneGraph::SideVector (0.f, 1.f, 0.f);

const Axis SceneGraph::OutAxis = SingleAxes::X;
const Vector3 SceneGraph::OutVector (1.f, 0.f, 0.f);

const Axis SceneGraph::UpAxis = SingleAxes::Z;
const Vector3 SceneGraph::UpVector (0.f, 0.f, 1.f);

#endif
