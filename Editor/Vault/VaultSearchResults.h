#pragma once

#include <vector>

#include "Editor/API.h"

#include "Foundation/FilePath.h"
#include "Foundation/SmartPtr.h"


namespace Helium
{
	namespace Editor
	{
		struct TrackedFile
		{
		};
		bool operator<( const TrackedFile& lhs, const TrackedFile& rhs );

		class VaultSearchResults : public Helium::RefCountBase< VaultSearchResults >
		{
		public:
			VaultSearchResults( uint32_t vaultSearchID = 0 );
			VaultSearchResults( const VaultSearchResults* results );
			virtual ~VaultSearchResults();

			void Clear();
			bool HasResults() const;

			void SetResults( const std::set< TrackedFile >& results );
			const std::set< TrackedFile >& GetResults() const;

			int32_t GetSearchID() { return m_VaultSearchID; }

			bool Add( const TrackedFile& file );
			bool Remove( const TrackedFile& file );

		private:
			// This is the ID of the VaultSearch that created these results, for easy of debugging
			int32_t m_VaultSearchID;

			std::set< TrackedFile > m_Results;
		};
		typedef Helium::SmartPtr< VaultSearchResults > VaultSearchResultsPtr;
	}
}
