#pragma once

#include "layers/layer_stack.h"

#include <moth_ui/event_listener.h>

class Layer : public moth_ui::EventListener {
public:
    Layer();
    virtual ~Layer();

    bool OnEvent(moth_ui::Event const& event) override;

    virtual void Update(uint32_t ticks);
    virtual void Draw();
    virtual void DebugDraw();

    virtual void OnAddedToStack(LayerStack* layerStack);
    virtual void OnRemovedFromStack();

    int GetWidth() const;
    int GetHeight() const;

    virtual bool UseRenderSize() const { return false; }

protected:
    LayerStack* m_layerStack = nullptr;
};
