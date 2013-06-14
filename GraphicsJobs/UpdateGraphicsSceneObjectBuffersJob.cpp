#include "GraphicsJobsPch.h"
#include "GraphicsJobs/GraphicsJobsInterface.h"

#include "Engine/JobManager.h"
#include "GraphicsTypes/VertexTypes.h"

namespace Helium
{
    /// Update the instance buffer data for a set of graphics scene objects.
    ///
    /// @param[in] pContext  Context in which this job is running.
    void UpdateGraphicsSceneObjectBuffersJob::Run( JobContext* /*pContext*/ )
    {
        const GraphicsSceneObject* pSceneObjects = m_parameters.pSceneObjects;
        HELIUM_ASSERT( pSceneObjects );

        float32_t* const* ppConstantBufferData = m_parameters.ppConstantBufferData;
        HELIUM_ASSERT( ppConstantBufferData );

        uint_fast32_t sceneObjectCount = m_parameters.sceneObjectCount;
        for( uint_fast32_t sceneObjectIndex = 0;
             sceneObjectIndex < sceneObjectCount;
             ++sceneObjectIndex, ++pSceneObjects, ++ppConstantBufferData )
        {
            float32_t* pConstantBuffer = *ppConstantBufferData;
            if( !pConstantBuffer )
            {
                continue;
            }

            const GraphicsSceneObject& rSceneObject = *pSceneObjects;

            const Simd::Matrix44& rTransform = rSceneObject.GetTransform();

            // Transpose the matrix when loading into the constant buffer for proper interpretation by the shader.
            *( pConstantBuffer++ ) = rTransform.GetElement( 0 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 4 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 8 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 12 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 1 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 5 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 9 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 13 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 2 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 6 );
            *( pConstantBuffer++ ) = rTransform.GetElement( 10 );
            *pConstantBuffer       = rTransform.GetElement( 14 );
        }

        JobManager& rJobManager = JobManager::GetStaticInstance();
        rJobManager.ReleaseJob( this );
    }
}
