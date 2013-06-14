#pragma once

#include "GraphicsJobs/GraphicsJobs.h"
#include "Platform/Assert.h"
#include "GraphicsTypes/GraphicsSceneObject.h"

namespace Helium
{
    class JobContext;
}

namespace Helium
{

/// Spawn jobs to update all instance constant buffers for graphics scene objects and sub-meshes.
class HELIUM_GRAPHICS_JOBS_API UpdateGraphicsSceneConstantBuffersJobSpawner : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Number of elements in the scene object array.
        uint32_t sceneObjectCount;
        /// [in] Number of elements in the sub-mesh array.
        uint32_t subMeshCount;
        /// [in] Array of scene objects to update.
        const GraphicsSceneObject* pSceneObjects;
        /// [in] Array of buffers in which to store the constant buffer data for each scene object.
        float32_t* const* ppSceneObjectConstantBufferData;
        /// [in] Array of sub-meshes to update.
        const GraphicsSceneObject::SubMeshData* pSubMeshes;
        /// [in] Array of buffers in which to store the constant buffer data for each sub-mesh.
        float32_t* const* ppSubMeshConstantBufferData;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline UpdateGraphicsSceneConstantBuffersJobSpawner();
    inline ~UpdateGraphicsSceneConstantBuffersJobSpawner();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

/// Spawn jobs to update the constant buffer data for all graphics scene objects.
class HELIUM_GRAPHICS_JOBS_API UpdateGraphicsSceneObjectBuffersJobSpawner : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Number of elements in the scene object array.
        uint32_t sceneObjectCount;
        /// [in] Array of scene objects to update.
        const GraphicsSceneObject* pSceneObjects;
        /// [out] Array of buffers in which to store the constant buffer data for each scene object.
        float32_t* const* ppConstantBufferData;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline UpdateGraphicsSceneObjectBuffersJobSpawner();
    inline ~UpdateGraphicsSceneObjectBuffersJobSpawner();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

/// Spawn jobs to update the constant buffer data for all graphics scene object sub-meshes.
class HELIUM_GRAPHICS_JOBS_API UpdateGraphicsSceneSubMeshBuffersJobSpawner : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Number of elements in the sub-mesh array.
        uint32_t subMeshCount;
        /// [in] Array of sub-meshes to update.
        const GraphicsSceneObject::SubMeshData* pSubMeshes;
        /// [in] Array of graphics scene objects.
        const GraphicsSceneObject* pSceneObjects;
        /// [out] Array of buffers in which to store the constant buffer data for each sub-mesh.
        float32_t* const* ppConstantBufferData;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline UpdateGraphicsSceneSubMeshBuffersJobSpawner();
    inline ~UpdateGraphicsSceneSubMeshBuffersJobSpawner();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

/// Update the constant buffer data for a set of graphics scene objects.
class HELIUM_GRAPHICS_JOBS_API UpdateGraphicsSceneObjectBuffersJob : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Number of elements in the scene object array.
        uint32_t sceneObjectCount;
        /// [in] Array of scene objects to update.
        const GraphicsSceneObject* pSceneObjects;
        /// [out] Array of buffers in which to store the constant buffer data for each scene object.
        float32_t* const* ppConstantBufferData;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline UpdateGraphicsSceneObjectBuffersJob();
    inline ~UpdateGraphicsSceneObjectBuffersJob();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

/// Update the constant buffer data for a set of graphics scene object sub-meshes.
class HELIUM_GRAPHICS_JOBS_API UpdateGraphicsSceneSubMeshBuffersJob : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Number of elements in the sub-mesh array.
        uint32_t subMeshCount;
        /// [in] Array of sub-meshes to update.
        const GraphicsSceneObject::SubMeshData* pSubMeshes;
        /// [in] Array of graphics scene objects.
        const GraphicsSceneObject* pSceneObjects;
        /// [out] Array of buffers in which to store the constant buffer data for each sub-mesh.
        float32_t* const* ppConstantBufferData;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline UpdateGraphicsSceneSubMeshBuffersJob();
    inline ~UpdateGraphicsSceneSubMeshBuffersJob();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

}  // namespace Helium

#include "GraphicsJobs/GraphicsJobsInterface.inl"
