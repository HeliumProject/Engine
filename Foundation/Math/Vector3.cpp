#include "FoundationPch.h"
#include "Vector3.h"
#include "Scale.h"

using namespace Helium;

const Vector3 Vector3::Zero;
const Vector3 Vector3::Unit   (1.0, 1.0, 1.0);
const Vector3 Vector3::BasisX (1.0, 0.0, 0.0);
const Vector3 Vector3::BasisY (0.0, 1.0, 0.0);
const Vector3 Vector3::BasisZ (0.0, 0.0, 1.0);

Vector3  Vector3::operator* (const Scale& v) const { return Vector3 (x * v.x, y * v.y, z * v.z); }

using namespace Helium;

int32_t Helium::LookupPosInArray( const Vector3& pos, int32_t min_key, int32_t max_key, V_Vector3& pos_array, MM_i32& pos_lookup , float32_t threshold)
{
    for (int32_t key = min_key; key <= max_key; ++key)
    {
        // lookup the first element with this key in the multi-map
        MM_i32::iterator ipos = pos_lookup.lower_bound( key );

        // continue if nothing matched this key
        if (ipos == pos_lookup.end())
            continue;

        // lookup the last element with this key in the multi-map
        MM_i32::iterator ipos_end = pos_lookup.upper_bound( key );

        // search through the elements with this key and explicitly test for a match
        for ( ; ipos != ipos_end; ++ipos)
        {
            // compare positions
            if ( pos.Equal(pos_array[ ipos->second ], threshold ) )
            {
                // return index of matched position vector
                return ipos->second;
            }
        }
    }

    // no match was found
    return -1;
}
