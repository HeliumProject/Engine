
#include "Game/GameLibrary/GameLibrary.h"

#include "Engine/Asset.h"
#include "Framework/Action.h"
#include "Framework/Predicate.h"

namespace GameLibrary
{
	class EnemyWaveManagerComponentDefinition;
	struct ParameterSet_ActionSpawnEnemyWave;

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API EnemyWaveFormation : public Helium::Reflect::Object
	{
		HELIUM_DECLARE_ABSTRACT( EnemyWaveFormation, Helium::Reflect::Object )

		virtual Helium::Simd::Vector3 GetSpawnLocation( ParameterSet_ActionSpawnEnemyWave *parameters, int index ) = 0;
	};
	typedef Helium::StrongPtr< EnemyWaveFormation > EnemyWaveFormationPtr;

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API EnemyWaveFormation_Circle : public EnemyWaveFormation
	{
		HELIUM_DECLARE_CLASS( EnemyWaveFormation_Circle, EnemyWaveFormation )

		EnemyWaveFormation_Circle();

		virtual Helium::Simd::Vector3 GetSpawnLocation( ParameterSet_ActionSpawnEnemyWave *parameters, int index );
	};

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API EnemyWaveDefinition : public Helium::Asset
	{
		HELIUM_DECLARE_ASSET( EnemyWaveDefinition, Asset );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		EnemyWaveDefinition();

		EnemyWaveFormationPtr m_Formation;
		Helium::EntityDefinitionPtr m_Entity;
	};
	typedef Helium::StrongPtr< EnemyWaveDefinition > EnemyWaveDefinitionPtr;

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API ParameterSet_ActionSpawnEnemyWave : public Helium::ParameterSet
	{
		HELIUM_DECLARE_CLASS( ParameterSet_ActionSpawnEnemyWave, ParameterSet );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		ParameterSet_ActionSpawnEnemyWave();

		Helium::Simd::Vector3 m_Location;
		int m_Count;
		float m_Distance;
	};

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API ActionSpawnEnemyWave : public Helium::Action
	{
		HELIUM_DECLARE_ASSET( ActionSpawnEnemyWave, Action );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		virtual void PerformAction( Helium::World &pWorld, Helium::ParameterSet *pParamSet );

		EnemyWaveDefinitionPtr m_WaveDefinition;
		Helium::StrongPtr< ParameterSet_ActionSpawnEnemyWave > m_DefaultParameters;
	};

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API PredicateEnemyWaveAlive : public Helium::Predicate
	{
		PredicateEnemyWaveAlive() : m_FractionAlive(HELIUM_EPSILON) { }

		HELIUM_DECLARE_ASSET( PredicateEnemyWaveAlive, Predicate );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		virtual bool Evaluate( Helium::World &pWorld, Helium::ParameterSet *pParamSet );

		EnemyWaveDefinitionPtr m_WaveDefinition;
		float m_FractionAlive;
	};

	//////////////////////////////////////////////////////////////////////////
	class GAME_LIBRARY_API EnemyWaveManager
	{
	public:
		void Initialize(Helium::World *pWorld);
		void Update(float dt);

		void SpawnWave( EnemyWaveDefinition *pWave, ParameterSet_ActionSpawnEnemyWave *pParameters );
		float GetPercentAlive( EnemyWaveDefinition *pDefinition );

	private:
		struct WaveEntityState
		{
			Helium::WeakPtr< Helium::Entity > m_Entity;
		};

		struct WaveState
		{
			EnemyWaveDefinitionPtr m_WaveDefinition;
			Helium::DynamicArray< WaveEntityState > m_Entities;
		};

		Helium::World *m_pWorld;
		Helium::DynamicArray< WaveState > m_ActiveWaves;
	};

	//////////////////////////////////////////////////////////////////////////
	class GAME_LIBRARY_API EnemyWaveManagerComponent : public Helium::Component
	{
	public:
		HELIUM_DECLARE_COMPONENT( EnemyWaveManagerComponent, Component );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		EnemyWaveManager &GetWaveManager() { return m_WaveManager; }

		void Initialize(const EnemyWaveManagerComponentDefinition &pDefinition);

		EnemyWaveManager &GetEnemyWaveManager() { return m_WaveManager; }

	private:
		EnemyWaveManager m_WaveManager;
	};

	//////////////////////////////////////////////////////////////////////////
	class GAME_LIBRARY_API EnemyWaveManagerComponentDefinition : public Helium::ComponentDefinitionHelper<EnemyWaveManagerComponent, EnemyWaveManagerComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( GameLibrary::EnemyWaveManagerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

		//DynamicArray< EnemyWaveDefinitionPtr > m_Waves;
	};

	//////////////////////////////////////////////////////////////////////////
	struct GAME_LIBRARY_API TaskUpdateEnemyWaveManager : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(TaskUpdateEnemyWaveManager)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
