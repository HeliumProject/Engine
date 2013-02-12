#pragma once

#include "Reflect/Data/DataDeduction.h"

// See comment at the top of FrameworkSimpleData.h.
#include "FrameworkSimpleData.h"
REFLECT_SPECIALIZE_DATA( Helium::Framework::Vector2Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Vector3Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Vector4Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Matrix3Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Matrix4Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Color3Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Color4Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::HDRColor3Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::HDRColor4Data );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Vector2StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Vector3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Vector4StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Matrix3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Matrix4StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Color3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::Color4StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::HDRColor3StlVectorData );
REFLECT_SPECIALIZE_DATA( Helium::Framework::HDRColor4StlVectorData );