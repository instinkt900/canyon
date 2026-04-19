#pragma once

#include <moth_ui/events/event.h>

namespace moth_graphics {
    enum MothGraphicsEventType : int {
        EVENTTYPE_RENDERDEVICERESET = moth_ui::EVENTTYPE_USER0,
        EVENTTYPE_RENDERTARGETRESET,
        EVENTTYPE_WINDOWSIZE,
        EVENTTYPE_REQUEST_QUIT,
        EVENTTYPE_QUIT,

        EVENTTYPE_GRAPHICSUSER0 = moth_ui::EVENTTYPE_USER0 + 100
    };
}
