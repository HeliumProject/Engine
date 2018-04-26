
namespace Helium
{
	GraphicsScene*  GraphicsManagerComponent::GetGraphicsScene() const
	{
		return m_spGraphicsScene.Get();
	}

#if GRAPHICS_SCENE_BUFFERED_DRAWER
	BufferedDrawer& GraphicsManagerComponent::GetBufferedDrawer()
	{
		return m_spGraphicsScene->GetSceneBufferedDrawer();
	}
#endif // GRAPHICS_SCENE_BUFFERED_DRAWER
}
