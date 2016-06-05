#pragma once

#include "Instance.h"

namespace Helium
{
	namespace Editor
	{
		class LocatorType;

		class LocatorShape
		{
		public:
			enum Enum
			{
				Cross,
				Cube,
			};

			HELIUM_DECLARE_ENUM( LocatorShape );

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement(Cross,  TXT( "Cross" ) );
				info.AddElement(Cube,   TXT( "Cube" ) );
			}
		};

		class Locator : public Instance
		{
		public:
			HELIUM_DECLARE_CLASS( Locator, Instance );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			Locator();
			~Locator();

			LocatorShape GetShape() const;
			void SetShape( LocatorShape shape );

			virtual void Evaluate(GraphDirection direction) override;
			virtual void Render( RenderVisitor* render ) override;
			virtual bool Pick( PickVisitor* pick ) override;

		protected:
			LocatorShape m_Shape;
		};

		typedef Helium::SmartPtr<Locator> LLocatorPtr;
	}
}