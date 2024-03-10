#include "canyon.h"
#include "layers/layer.h"
#include "layers/layer_stack.h"

Layer::Layer() {
}

Layer::~Layer() {
}

bool Layer::OnEvent(Event const& event) {
    return false;
}

void Layer::Update(uint32_t ticks) {
}

void Layer::Draw() {
}

void Layer::DebugDraw() {
}

void Layer::OnAddedToStack(LayerStack* layerStack) {
    m_layerStack = layerStack;
}

void Layer::OnRemovedFromStack() {
    m_layerStack = nullptr;
}

int Layer::GetWidth() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return UseRenderSize() ? m_layerStack->GetRenderWidth() : m_layerStack->GetWindowWidth();
}

int Layer::GetHeight() const {
    if (nullptr == m_layerStack) {
        return 0;
    }
    return UseRenderSize() ? m_layerStack->GetRenderHeight() : m_layerStack->GetWindowHeight();
}
