#include "Precompile.h"
#include "Engine/PackageLoader.h"

using namespace Helium;

/// Destructor.
PackageLoader::~PackageLoader()
{
}

#if HELIUM_TOOLS

bool PackageLoader::HasAssetFileState() const
{
	return false;
}

const FilePath &PackageLoader::GetAssetFileSystemPath( const AssetPath &path ) const
{
	static const FilePath nullPath;
	return nullPath;
}

int64_t PackageLoader::GetAssetFileSystemTimestamp( const AssetPath &path ) const
{
	return INT64_MIN;
}

void PackageLoader::EnumerateChildren( DynamicArray< AssetPath > &children ) const
{
	HELIUM_BREAK_MSG("We tried to enumerate children with a package loader that doesn't support doing that!");
}

bool PackageLoader::SaveAsset( Asset *pAsset ) const
{
	HELIUM_BREAK_MSG("We tried to save an asset with a package loader that doesn't support doing that!");
	return false;
}

#endif

/// @fn size_t PackageLoader::BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver )
/// Begin asynchronous preloading of an object's properties from the cache.
///
/// Note: Do NOT call this multiple times for the same object (access to this function should actually be abstracted
/// by the AssetLoader interface).
///
/// @param[in] path  FilePath of the object to attempt to load.
///
/// @return  Load request ID.
///
/// @see TryFinishLoadObject()

/// @fn bool TryFinishLoadObject( size_t requestId, AssetPtr& rspObject )
/// Test for and finalize an asynchronous object preload request in a non-blocking fashion.
///
/// @param[in]  requestId   ID of the preload request returned by BeginLoadObject() (NOT the object index).
/// @param[out] rspObject   Smart pointer to the object if preloading has completed (linking still needs to occur).
///
/// @return  True if the preload has finished, false if not.
///
/// @see BeginLoadObject()

/// @fn size_t PackageLoader::GetObjectCount() const
/// Get the number of objects in the associated package file.
///
/// @return  Asset count.
///
/// @see GetObjectPath(), GetObjectType(), GetObject()

/// @fn AssetPath PackageLoader::GetObjectPath( size_t index ) const
/// Get the path name of the object with the given index.
///
/// @param[in] index  Asset index.
///
/// @return  Asset path.
///
/// @see GetObjectType(), GetObject(), GetObjectCount()

