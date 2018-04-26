
namespace Helium
{
	uint32_t Helium::Texture2d::GetWidth() const
	{
		return m_persistentResourceData.m_baseLevelWidth;
	}

	uint32_t Helium::Texture2d::GetHeight() const
	{
		return m_persistentResourceData.m_baseLevelHeight;
	}
}
