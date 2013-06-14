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
		virtual ~PackageLoader() = 0;
		//@}

		/// @name Loading
		//@{
		virtual bool TryFinishPreload() = 0;

		virtual size_t BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver ) = 0;
		virtual bool TryFinishLoadObject( size_t requestId, AssetPtr& rspObject ) = 0;

		virtual void Tick() = 0;
		//@}

		/// @name Data Access
		//@{
		virtual size_t GetObjectCount() const = 0;
		virtual AssetPath GetAssetPath( size_t index ) const = 0;
		//@}
		
#if HELIUM_TOOLS
		/// @name Package File Information
		//@{
		virtual bool HasAssetFileState() const;
		virtual const FilePath &GetAssetFileSystemPath( const AssetPath &path ) const;
		virtual int64_t GetAssetFileSystemTimestamp( const AssetPath &path ) const;
		//@}
#endif // #if HELIUM_TOOLS
	};
}
