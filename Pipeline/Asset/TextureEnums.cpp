#include "PipelinePch.h"
#include "TextureEnums.h"

using namespace Helium;
using namespace Helium::Asset;

HELIUM_COMPILE_ASSERT( TextureWrapMode::Count == Helium::UV_COUNT );
REFLECT_DEFINE_ENUMERATION( TextureWrapMode );

HELIUM_COMPILE_ASSERT( TextureColorFormat::COUNT == Helium::OUTPUT_CF_COUNT );
REFLECT_DEFINE_ENUMERATION( TextureColorFormat );

HELIUM_COMPILE_ASSERT( RunTimeFilter::COUNT == Helium::FILTER_COUNT );
REFLECT_DEFINE_ENUMERATION( RunTimeFilter );

HELIUM_COMPILE_ASSERT( MipGenFilterType::COUNT == Helium::MIP_FILTER_COUNT );
REFLECT_DEFINE_ENUMERATION( MipGenFilterType );

HELIUM_COMPILE_ASSERT( PostMipFilterType::COUNT == Helium::IMAGE_FILTER_COUNT );
REFLECT_DEFINE_ENUMERATION( PostMipFilterType );

REFLECT_DEFINE_ENUMERATION( ReductionRatio );