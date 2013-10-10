
namespace Helium
{
	bool LooseAssetFileWatcher::IsThreadRunning()
	{
		return ( m_Thread.IsValid() );
	}
}