#pragma once

#include "Framework/Framework.h"

#include "MathSimd/Quat.h"
#include "MathSimd/Vector3.h"

#include "Engine/Asset.h"

namespace Helium
{
	class EntityDefinition;
	typedef Helium::StrongPtr< EntityDefinition > EntityDefinitionPtr;
	typedef Helium::StrongPtr< const EntityDefinition > ConstEntityDefinitionPtr;
	
	class WorldDefinition;
	typedef Helium::StrongPtr< WorldDefinition > WorldDefinitionPtr;
	typedef Helium::StrongPtr< const WorldDefinition > ConstWorldDefinitionPtr;
	
	class SceneDefinition;
	typedef Helium::StrongPtr< SceneDefinition > SceneDefinitionPtr;
	typedef Helium::StrongPtr< const SceneDefinition > ConstSceneDefinitionPtr;

	/// Container for entities.
	///
	/// Slices typically fall under one of two categories.
	/// - Level slices.  The Level instance is either created from the package information and cached (in editor mode)
	///   or loaded from the cache file (in runtime mode).  When in runtime mode, the loaded level is typically linked
	///   directly to a world, with all of its entities attached when linked.
	/// - Dynamic slices.  These can be constructed in editor mode for use with special-case worlds (such as for a
	///   preview window).  In runtime mode, dynamic slices can be created as necessary for various runtime-created
	///   entities.
	class HELIUM_FRAMEWORK_API SceneDefinition : public Asset
	{
		HELIUM_DECLARE_ASSET( SceneDefinition, Asset );
		static void PopulateStructure( Reflect::Structure& comp );

	public:
		/// @name Construction/Destruction
		//@{
		SceneDefinition();
		virtual ~SceneDefinition();
		//@}
		
		/// @name EntityDefinition Creation
		//@{
		virtual EntityDefinition* AddEntityDefinition(
			const AssetType* pType, 
			//const Simd::Vector3& rPosition = Simd::Vector3( 0.0f ),
			//const Simd::Quat& rRotation = Simd::Quat::IDENTITY, 
			//const Simd::Vector3& rScale = Simd::Vector3( 1.0f ),
			EntityDefinition* pTemplate = NULL, 
			Name name = NULL_NAME, 
			bool bAssignInstanceIndex = true );
		virtual bool DestroyEntityDefinition( EntityDefinition* pEntity );
		//@}

		/// @name Serialization
		//@{
		//virtual void Serialize( Serializer& s );
		//@}

		/// @name Dynamic Package Binding
		//@{
		void BindPackage( Package* pPackage );
		inline Package* GetPackage() const;
		//@}

		/// @name EntityDefinition Access
		//@{
		inline size_t GetEntityDefinitionCount() const;
		inline EntityDefinition* GetEntityDefinition( size_t index ) const;
		//@}

		inline const Helium::WorldDefinition *GetWorldDefinition() const;
		inline void SetWorldDefinition(Helium::WorldDefinition *);

	private:
		/// Bound package.
		PackagePtr m_spPackage;
		/// Entities.
		DynamicArray< EntityDefinitionPtr > m_Entities;

		/// Definition for world (i.e. world-level components) which is optional if this scene is never created as a world
		WorldDefinitionPtr m_WorldDefinition;

		/// @name Private Utility Functions
		//@{
		void AddPackageEntities();
		void StripNonPackageEntities();
		//@}
	};
}

#include "Framework/WorldDefinition.h"
#include "Framework/EntityDefinition.h"

#include "Framework/SceneDefinition.inl"
