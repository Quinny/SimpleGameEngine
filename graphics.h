#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <algorithm>
#include <iostream>

#include "asteroids.h"
#include "engine/component.h"
#include "engine/message_bus.h"
#include "sdl/screen.h"

namespace qp {

// A request to blit a surface to the screen. Requests are sorted relative to
// their z values before drawing so the user can enforce ordering. Ties will
// broken non-deterministically.
struct BlitRequest {
  SDL_Surface* surface;
  int x, y, z;
  bool operator<(const BlitRequest& rhs) const { return z < rhs.z; }
};

// A component which processes Blit requests and draws them to an SDL window.
// Note that DRAW_SURFACE messages will not be automatically drawn, but instead
// buffered until the end of the frame and drawn in order of z value.
class Graphics
    : public engine::Component<qp::MessageType, qp::AsteroidsMessage> {
 public:
  Graphics(
      engine::MessageBus<qp::MessageType, qp::AsteroidsMessage>* message_bus,
      const std::string& title, const int x, const int y, const int w,
      const int h)
      : engine::Component<qp::MessageType, qp::AsteroidsMessage>(message_bus),
        screen_(title, x, y, w, h) {
    using std::placeholders::_1;
    OnMessage(qp::MessageType::DRAW_SURFACE,
              std::bind(&Graphics::BufferSurface, this, _1));
    OnMessage(qp::MessageType::FRAME_END, std::bind(&Graphics::Update, this));
  }

 private:
  void BufferSurface(const qp::AsteroidsMessage& message) {
    surfaces_to_draw_.push_back(
        {message.surface, message.x, message.y, message.z});
  }

  void Update() {
    if (!surfaces_to_draw_.empty()) {
      screen_.Clear();
      std::sort(surfaces_to_draw_.begin(), surfaces_to_draw_.end());
      for (const auto& blit_request : surfaces_to_draw_) {
        screen_.Blit(blit_request.surface, blit_request.x, blit_request.y);
      }
      screen_.Update();
      surfaces_to_draw_.clear();
    }
  }

 private:
  qp::Screen screen_;
  std::vector<BlitRequest> surfaces_to_draw_;
};

}  // namespace qp

#endif /* GRAPHICS_H */
