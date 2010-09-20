#pragma once

#include "Core/SceneGraph/Viewport.h"
#include "Resource.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Statistics
        {
        private:
            IDirect3DDevice9* m_Device;
            ID3DXFont* m_Font;
            u64 m_Previous;

        public:

            //
            // Render
            //

            u32 m_FrameNumber;
            u32 m_FrameCount;
            u32 m_FramesPerSecondResult;
            u32 m_FrameRateResult;

            f32 m_RenderTimeResult;
            f32 m_RenderTime;

            f32 m_RenderWalkTimeResult;
            f32 m_RenderWalkTime;

            f32 m_RenderSortTimeResult;
            f32 m_RenderSortTime;

            f32 m_RenderCompareTimeResult;
            f32 m_RenderCompareTime;

            f32 m_RenderDrawTimeResult;
            f32 m_RenderDrawTime;

            u32 m_EntryCountResult;
            u32 m_EntryCount;

            u32 m_TriangleCountResult;
            u32 m_TriangleCount;

            u32 m_LineCountResult;
            u32 m_LineCount;


            //
            // Evaluate
            //

            f32 m_EvaluateTimeResult;
            f32 m_EvaluateTime;

            i32 m_NodeCountResult;
            i32 m_NodeCount;

        public:
            Statistics(IDirect3DDevice9* device);

            ~Statistics();

            ID3DXFont* GetFont() { return m_Font; }

            void Reset();
            void Update();

            void Create();
            void Delete();

            void Draw( DrawArgs* args );
        };
    }
}