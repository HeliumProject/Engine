#pragma once

#include "Engine/Asset.h"

namespace Helium
{
	class PackageLoader;
	class Package;
	typedef Helium::StrongPtr< Package > PackagePtr;
	typedef Helium::StrongPtr< const Package > ConstPackagePtr;

	/// Asset package.
	class HELIUM_ENGINE_API Package : public Asset
	{
		HELIUM_DECLARE_OBJECT( Package, Asset );

	public:
		/// @name Construction/Destruction
		//@{
		Package();
		virtual ~Package();
		//@}

		/// @name Loader Information
		//@{
		inline PackageLoader* GetLoader() const;
		void SetLoader( PackageLoader* pLoader );
		//@}

		/// @name Package Serialization
		//@{
		void SavePackage();
		//@}

	private:
		/// Package loader.
		PackageLoader* m_pLoader;
	};
}

#include "Engine/Package.inl"
