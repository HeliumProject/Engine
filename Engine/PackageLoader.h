//----------------------------------------------------------------------------------------------------------------------
// PackageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_PACKAGE_LOADER_H
#define HELIUM_ENGINE_PACKAGE_LOADER_H

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

		virtual size_t BeginLoadObject( AssetPath path ) = 0;
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
		virtual bool CanResolveLooseAssetFilePaths() const;
		virtual const FilePath &GetLooseAssetFileSystemPath( const AssetPath &path ) const;
		virtual int64_t GetLooseAssetFileSystemTimestamp( const AssetPath &path ) const;
		//@}
#endif // #if HELIUM_TOOLS
	};
}

#endif  // HELIUM_ENGINE_PACKAGE_LOADER_H
