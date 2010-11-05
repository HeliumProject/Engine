#pragma once

#include "Pipeline/SceneGraph/Viewport.h"
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
            uint64_t m_Previous;

        public:

            //
            // Render
            //

            uint32_t m_FrameNumber;
            uint32_t m_FrameCount;
            uint32_t m_FramesPerSecondResult;
            uint32_t m_FrameRateResult;

            float32_t m_RenderTimeResult;
            float32_t m_RenderTime;

            float32_t m_RenderWalkTimeResult;
            float32_t m_RenderWalkTime;

            float32_t m_RenderSortTimeResult;
            float32_t m_RenderSortTime;

            float32_t m_RenderCompareTimeResult;
            float32_t m_RenderCompareTime;

            float32_t m_RenderDrawTimeResult;
            float32_t m_RenderDrawTime;

            uint32_t m_EntryCountResult;
            uint32_t m_EntryCount;

            uint32_t m_TriangleCountResult;
            uint32_t m_TriangleCount;

            uint32_t m_LineCountResult;
            uint32_t m_LineCount;


            //
            // Evaluate
            //

            float32_t m_EvaluateTimeResult;
            float32_t m_EvaluateTime;

            int32_t m_NodeCountResult;
            int32_t m_NodeCount;

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