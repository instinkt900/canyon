#include "platform/glfw/glfw_events.h"
#include <GLFW/glfw3.h>
#include "events/event_key.h"
#include "events/event_mouse.h"
#include "events/event.h"

namespace {
    Key FromGLFWKey(int key) {
        switch (key) {
        case GLFW_KEY_ENTER:
            return Key::Return;
        case GLFW_KEY_ESCAPE:
            return Key::Escape;
        case GLFW_KEY_BACKSPACE:
            return Key::Backspace;
        case GLFW_KEY_TAB:
            return Key::Tab;
        case GLFW_KEY_SPACE:
            return Key::Space;
        case GLFW_KEY_COMMA:
            return Key::Comma;
        case GLFW_KEY_MINUS:
            return Key::Minus;
        case GLFW_KEY_PERIOD:
            return Key::Period;
        case GLFW_KEY_SLASH:
            return Key::Slash;
        case GLFW_KEY_0:
            return Key::N0;
        case GLFW_KEY_1:
            return Key::N1;
        case GLFW_KEY_2:
            return Key::N2;
        case GLFW_KEY_3:
            return Key::N3;
        case GLFW_KEY_4:
            return Key::N4;
        case GLFW_KEY_5:
            return Key::N5;
        case GLFW_KEY_6:
            return Key::N6;
        case GLFW_KEY_7:
            return Key::N7;
        case GLFW_KEY_8:
            return Key::N8;
        case GLFW_KEY_9:
            return Key::N9;
        case GLFW_KEY_SEMICOLON:
            return Key::Semicolon;

        case GLFW_KEY_LEFT_BRACKET:
            return Key::Leftbracket;
        case GLFW_KEY_BACKSLASH:
            return Key::Backslash;
        case GLFW_KEY_RIGHT_BRACKET:
            return Key::Rightbracket;
        case GLFW_KEY_GRAVE_ACCENT:
            return Key::Backquote;
        case GLFW_KEY_A:
            return Key::A;
        case GLFW_KEY_B:
            return Key::B;
        case GLFW_KEY_C:
            return Key::C;
        case GLFW_KEY_D:
            return Key::D;
        case GLFW_KEY_E:
            return Key::E;
        case GLFW_KEY_F:
            return Key::F;
        case GLFW_KEY_G:
            return Key::G;
        case GLFW_KEY_H:
            return Key::H;
        case GLFW_KEY_I:
            return Key::I;
        case GLFW_KEY_J:
            return Key::J;
        case GLFW_KEY_K:
            return Key::K;
        case GLFW_KEY_L:
            return Key::L;
        case GLFW_KEY_M:
            return Key::M;
        case GLFW_KEY_N:
            return Key::N;
        case GLFW_KEY_O:
            return Key::O;
        case GLFW_KEY_P:
            return Key::P;
        case GLFW_KEY_Q:
            return Key::Q;
        case GLFW_KEY_R:
            return Key::R;
        case GLFW_KEY_S:
            return Key::S;
        case GLFW_KEY_T:
            return Key::T;
        case GLFW_KEY_U:
            return Key::U;
        case GLFW_KEY_V:
            return Key::V;
        case GLFW_KEY_W:
            return Key::W;
        case GLFW_KEY_X:
            return Key::X;
        case GLFW_KEY_Y:
            return Key::Y;
        case GLFW_KEY_Z:
            return Key::Z;

        case GLFW_KEY_CAPS_LOCK:
            return Key::Capslock;

        case GLFW_KEY_F1:
            return Key::F1;
        case GLFW_KEY_F2:
            return Key::F2;
        case GLFW_KEY_F3:
            return Key::F3;
        case GLFW_KEY_F4:
            return Key::F4;
        case GLFW_KEY_F5:
            return Key::F5;
        case GLFW_KEY_F6:
            return Key::F6;
        case GLFW_KEY_F7:
            return Key::F7;
        case GLFW_KEY_F8:
            return Key::F8;
        case GLFW_KEY_F9:
            return Key::F9;
        case GLFW_KEY_F10:
            return Key::F10;
        case GLFW_KEY_F11:
            return Key::F11;
        case GLFW_KEY_F12:
            return Key::F12;

        case GLFW_KEY_PRINT_SCREEN:
            return Key::Printscreen;
        case GLFW_KEY_SCROLL_LOCK:
            return Key::Scrolllock;
        case GLFW_KEY_PAUSE:
            return Key::Pause;
        case GLFW_KEY_INSERT:
            return Key::Insert;
        case GLFW_KEY_HOME:
            return Key::Home;
        case GLFW_KEY_PAGE_UP:
            return Key::Pageup;
        case GLFW_KEY_DELETE:
            return Key::Delete;
        case GLFW_KEY_END:
            return Key::End;
        case GLFW_KEY_PAGE_DOWN:
            return Key::Pagedown;
        case GLFW_KEY_RIGHT:
            return Key::Right;
        case GLFW_KEY_LEFT:
            return Key::Left;
        case GLFW_KEY_DOWN:
            return Key::Down;
        case GLFW_KEY_UP:
            return Key::Up;

        case GLFW_KEY_KP_DIVIDE:
            return Key::KP_Divide;
        case GLFW_KEY_KP_MULTIPLY:
            return Key::KP_Multiply;
        case GLFW_KEY_KP_SUBTRACT:
            return Key::KP_Minus;
        case GLFW_KEY_KP_ADD:
            return Key::KP_Plus;
        case GLFW_KEY_KP_ENTER:
            return Key::KP_Enter;
        case GLFW_KEY_KP_1:
            return Key::KP_1;
        case GLFW_KEY_KP_2:
            return Key::KP_2;
        case GLFW_KEY_KP_3:
            return Key::KP_3;
        case GLFW_KEY_KP_4:
            return Key::KP_4;
        case GLFW_KEY_KP_5:
            return Key::KP_5;
        case GLFW_KEY_KP_6:
            return Key::KP_6;
        case GLFW_KEY_KP_7:
            return Key::KP_7;
        case GLFW_KEY_KP_8:
            return Key::KP_8;
        case GLFW_KEY_KP_9:
            return Key::KP_9;
        case GLFW_KEY_KP_0:
            return Key::KP_0;
        case GLFW_KEY_KP_DECIMAL:
            return Key::KP_Period;

        case GLFW_KEY_LEFT_CONTROL:
            return Key::Lctrl;
        case GLFW_KEY_LEFT_SHIFT:
            return Key::Lshift;
        case GLFW_KEY_LEFT_ALT:
            return Key::Lalt;
        case GLFW_KEY_RIGHT_CONTROL:
            return Key::Rctrl;
        case GLFW_KEY_RIGHT_SHIFT:
            return Key::Rshift;
        case GLFW_KEY_RIGHT_ALT:
            return Key::Ralt;

        default:
            return Key::Unknown;
        }
    }

    MouseButton FromGLFWButton(int button) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            return MouseButton::Left;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return MouseButton::Middle;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return MouseButton::Right;
        default:
            return MouseButton::Unknown;
        }
    }
}

std::unique_ptr<Event> FromGLFW(int key, int scancode, int action, int mods) {
    KeyAction keyAction = action == GLFW_RELEASE ? KeyAction::Up : KeyAction::Down;
    int keyMods = 0;
    if ((mods & GLFW_MOD_SHIFT) != 0)
        keyMods |= KeyMod_LeftShift;
    if ((mods & GLFW_MOD_ALT) != 0)
        keyMods |= KeyMod_LeftAlt;
    if ((mods & GLFW_MOD_CONTROL) != 0)
        keyMods |= KeyMod_LeftCtrl;
    return std::make_unique<EventKey>(keyAction, FromGLFWKey(key), keyMods);
}

std::unique_ptr<Event> FromGLFW(int button, int action, int mods, IntVec2 const& pos) {
    if (action == GLFW_PRESS) {
        return std::make_unique<EventMouseDown>(FromGLFWButton(button), pos);
    } else if (action == GLFW_RELEASE) {
        return std::make_unique<EventMouseUp>(FromGLFWButton(button), pos);
    }
    return nullptr;
}
