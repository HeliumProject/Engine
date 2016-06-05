#pragma once

#include "EditorScene/PivotTransform.h"

namespace Helium
{
	namespace Editor
	{
		class Instance;

		class HELIUM_EDITOR_SCENE_API Instance : public PivotTransform
		{
		public:
			HELIUM_DECLARE_ABSTRACT( Instance, PivotTransform );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			Instance();
			~Instance();

		protected:
			bool  m_Solid;
			bool  m_SolidOverride;
			bool  m_Transparent;
			bool  m_TransparentOverride;
		};

		typedef std::vector< Instance* > V_InstanceDumbPtr;
	}
}