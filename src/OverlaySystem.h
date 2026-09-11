#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <umbrellas/common.hpp>

class Overlay {
    expose
    virtual ~Overlay() = default;

    virtual auto Draw() -> bool = 0;

    [[nodiscard]] virtual auto BlocksControls() const -> bool { return true; }
    [[nodiscard]] virtual auto ReleasesCursor() const -> bool { return true; }
    [[nodiscard]] virtual auto FreezesSim() const -> bool { return false; }
    [[nodiscard]] virtual auto DimsBehind() const -> bool { return false; }
    [[nodiscard]] virtual auto Id() const -> const char* = 0;
};

class OverlaySystem {
    hide
    std::vector<std::unique_ptr<Overlay>> _stack;
    bool _dirty = false;

    expose
    auto Push(std::unique_ptr<Overlay> overlay) -> void;
    auto CloseTop() -> void;
    auto Close(std::string_view id) -> void;
    auto Clear() -> void;
    auto Draw() -> void;

    [[nodiscard]] auto IsOpen(std::string_view id) const -> bool;
    [[nodiscard]] auto Empty() const -> bool { return _stack.empty(); }
    [[nodiscard]] auto BlocksControls() const -> bool;
    [[nodiscard]] auto ReleasesCursor() const -> bool;
    [[nodiscard]] auto FreezesSim() const -> bool;

    hide
    auto DrawDim() -> void;
};

class MessageOverlay : public Overlay {
    expose
    struct Button {
        std::string Label;
        std::function<void()> OnClick;
    };

    hide
    std::string _id;
    std::string _title;
    std::string _body;
    std::vector<Button> _buttons;

    expose
    MessageOverlay(
        std::string id,
        std::string title,
        std::string body,
        std::vector<Button> buttons
    );

    expose
    auto Draw() -> bool override;
    [[nodiscard]] auto DimsBehind() const -> bool override { return true; }
    [[nodiscard]] auto Id() const -> const char* override { return _id.c_str(); }
};
