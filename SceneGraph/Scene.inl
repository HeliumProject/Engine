namespace Helium
{
    namespace SceneGraph
    {
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


        struct ResolveSceneArgs
        {
            SceneGraph::Viewport* m_Viewport;
            Helium::FilePath m_Path;
            mutable Scene* m_Scene;

            ResolveSceneArgs( SceneGraph::Viewport* viewport, const Helium::FilePath& path )
                : m_Viewport( viewport )
                , m_Path( path )
                , m_Scene( NULL )
            {
            }
        };

        struct ReleaseSceneArgs
        {
            mutable Scene* m_Scene;

            ReleaseSceneArgs( Scene* scene )
                : m_Scene( scene )
            {
            }
        };

        struct SceneEditingArgs
        {
            Scene* m_Scene;
            mutable bool m_Veto;

            SceneEditingArgs( Scene* scene )
                : m_Scene( scene )
                , m_Veto( false )
            {
            }
        };

        // update the status bar of the frame of this instance of the scene editor
        struct SceneStatusChangeArgs
        {
            const tstring& m_Status;

            SceneStatusChangeArgs( const tstring& status )
                : m_Status (status)
            {
            }
        };

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
            SceneGraph::Scene* m_Scene;
            UndoCommandPtr m_Command;

            UndoCommandArgs( SceneGraph::Scene* scene, UndoCommandPtr command )
                : m_Scene( scene )
                , m_Command( command )
            {
            }
        };
    }
}