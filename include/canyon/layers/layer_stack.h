#pragma once

#include "canyon/utils/vector.h"

#include <moth_ui/event_listener.h>
#include <moth_ui/events/event.h>

#include <memory>
#include <vector>
#include <cstdint>

namespace canyon {
    class Layer;

    class LayerStack : public moth_ui::EventListener {
    public:
        LayerStack(int renderWidth, int renderHeight, int windowWidth, int windowHeight);
        virtual ~LayerStack();

        void PushLayer(std::unique_ptr<Layer>&& layer);
        std::unique_ptr<Layer> PopLayer();
        void RemoveLayer(Layer* layer);

        virtual bool OnEvent(moth_ui::Event const& event) override;
        virtual void Update(uint32_t ticks);
        virtual void Draw();
        virtual void DebugDraw();

        void SetWindowSize(IntVec2 const& dimensions);
        void SetRenderSize(IntVec2 const& dimensions);

        int GetRenderWidth() const { return m_renderWidth; }
        int GetRenderHeight() const { return m_renderHeight; }
        int GetWindowWidth() const { return m_windowWidth; }
        int GetWindowHeight() const { return m_windowHeight; }

    protected:
        virtual void SetLayerLogicalSize(IntVec2 const& size);

        std::vector<std::unique_ptr<Layer>> m_layers;

        int m_renderWidth = 0;
        int m_renderHeight = 0;

        int m_windowWidth = 0;
        int m_windowHeight = 0;
    };
}
