#include "common.h"
#include "canyon/layers/layer_stack.h"
#include "canyon/layers/layer.h"

namespace canyon {
    LayerStack::LayerStack(int renderWidth, int renderHeight, int windowWidth, int windowHeight)
        : m_renderWidth(renderWidth)
        , m_renderHeight(renderHeight)
        , m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight) {
    }

    LayerStack::~LayerStack() {
    }

    void LayerStack::PushLayer(std::unique_ptr<Layer>&& layer) {
        m_layers.push_back(std::move(layer));
        m_layers.back()->OnAddedToStack(this);
    }

    std::unique_ptr<Layer> LayerStack::PopLayer() {
        auto oldLayer = std::move(m_layers.back());
        m_layers.pop_back();
        oldLayer->OnRemovedFromStack();
        return oldLayer;
    }

    void LayerStack::RemoveLayer(Layer* layer) {
        auto it = std::find_if(std::begin(m_layers), std::end(m_layers), [layer](auto& candidate) {
            return candidate.get() == layer;
        });
        if (std::end(m_layers) != it) {
            (*it)->OnRemovedFromStack();
            m_layers.erase(it);
        }
    }

    bool LayerStack::OnEvent(moth_ui::Event const& event) {
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
            if ((*it)->OnEvent(event)) {
                return true;
            }
        }
        return false;
    }

    void LayerStack::Update(uint32_t ticks) {
        for (auto&& layer : m_layers) {
            layer->Update(ticks);
        }
    }

    void LayerStack::Draw() {
        for (auto&& layer : m_layers) {
            if (layer->UseRenderSize()) {
                SetLayerLogicalSize(IntVec2{ m_renderWidth, m_renderHeight });
            } else {
                SetLayerLogicalSize(IntVec2{ m_windowWidth, m_windowHeight });
            }
            layer->Draw();
        }
    }

    void LayerStack::DebugDraw() {
    }

    void LayerStack::SetWindowSize(IntVec2 const& dimensions) {
        m_windowWidth = dimensions.x;
        m_windowHeight = dimensions.y;
    }

    void LayerStack::SetRenderSize(IntVec2 const& dimensions) {
        m_renderWidth = dimensions.x;
        m_renderHeight = dimensions.y;
    }

    void LayerStack::SetLayerLogicalSize(IntVec2 const& size) {
    }
}
