namespace Helium
{
    namespace SceneGraph
    {
        struct ResolveSceneArgs
        {
            ResolveSceneArgs( SceneGraph::Viewport* viewport, const Helium::FilePath& path )
                : m_Viewport( viewport )
                , m_Path( path )
                , m_Scene( NULL )
            {
            }

            SceneGraph::Viewport* m_Viewport;
            Helium::FilePath          m_Path;
            mutable Scene*        m_Scene;
        };

        struct ReleaseSceneArgs
        {
            ReleaseSceneArgs( Scene* scene )
                : m_Scene( scene )
            {
            }

            mutable Scene* m_Scene;
        };

        struct SceneEditingArgs
        {
            SceneEditingArgs( Scene* scene )
                : m_Scene( scene )
                , m_Veto( false )
            {
            }

            Scene*          m_Scene;
            mutable bool    m_Veto;
        };

        // update the status bar of the frame of this instance of the scene editor
        struct SceneStatusChangeArgs
        {
            const tstring& m_Status;

            SceneStatusChangeArgs ( const tstring& status )
                : m_Status (status)
            {

            }
        };

        namespace SceneContexts
        {
            enum SceneContext
            {
                None,
                Normal,
                Loading,
                Saving,
                Picking,
            };
        }
        typedef SceneContexts::SceneContext SceneContext;

        struct SceneContextChangeArgs
        {
            SceneContext m_OldContext;
            SceneContext m_NewContext;

            SceneContextChangeArgs( SceneContext oldContext, SceneContext newContext )
                : m_OldContext( oldContext )
                , m_NewContext( newContext )
            {
            }
        };


        //
        // Some scene data directly correlates with UI, and we need to fire events when the UI needs updating
        //

        // arguments and delegates for when a node is changed (in this case, added to or removed from the scene)
        struct NodeChangeArgs
        {
            SceneGraph::SceneNode* m_Node;

            NodeChangeArgs( SceneGraph::SceneNode* node )
                : m_Node( node )
            {

            }
        };

        // event for loading a scene.
        struct LoadArgs
        {
            SceneGraph::Scene* m_Scene;
            bool m_Success; // Only valid for finished loading events

            LoadArgs( SceneGraph::Scene* scene, bool loadedOk = false )
                : m_Scene( scene )
                , m_Success( loadedOk )
            {

            }
        };

        // event for loading a scene.
        struct ExecuteArgs
        {
            SceneGraph::Scene* m_Scene;
            bool m_Interactively;

            ExecuteArgs( SceneGraph::Scene* scene, bool interactively )
                : m_Scene( scene )
                , m_Interactively( interactively )
            {

            }
        };

        struct UndoCommandArgs
        {
            UndoCommandArgs( SceneGraph::Scene* scene, UndoCommandPtr command )
                : m_Scene( scene )
                , m_Command( command )
            {
            }

            SceneGraph::Scene* m_Scene;
            UndoCommandPtr   m_Command;
        };
    }
}