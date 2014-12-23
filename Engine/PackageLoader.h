#pragma once

#include "Engine/AssetLoader.h"

namespace Helium
{
	/// Package loader interface.
	class HELIUM_ENGINE_API PackageLoader : NonCopyable
	{
	public:
		/// @name Construction/Destruction
		//@{
		virtual ~PackageLoader();
		//@}

		/// @name Loading
		//@{
		virtual bool TryFinishPreload() = 0;

		virtual size_t BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver, bool forceReload = false ) = 0;
		virtual bool TryFinishLoadObject( size_t requestId, AssetPtr& rspObject ) = 0;

		virtual void Tick() = 0;
		//@}

		/// @name Data Access
		//@{
		virtual size_t GetAssetCount() const = 0;
		virtual AssetPath GetAssetPath( size_t index ) const = 0;
		//@}
		
#if HELIUM_TOOLS
		/// @name Package File Information
		//@{
		virtual bool HasAssetFileState() const;
		virtual size_t GetAssetIndex( const AssetPath &path ) const;
		virtual const FilePath &GetAssetFileSystemPath( size_t index ) const;
		virtual int64_t GetAssetFileSystemTimestamp( size_t index ) const;
		virtual Name GetAssetTypeName( size_t index ) const;
		virtual AssetPath GetAssetTemplatePath( size_t index ) const;
		//@}
		
		virtual void EnumerateChildPackages( DynamicArray< AssetPath > &children ) const;

		virtual bool SaveAsset( Asset *pAsset ) const;
#endif // #if HELIUM_TOOLS
	};
}
