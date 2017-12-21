#ifndef SCREEN_H
#define SCREEN_H

#include <algorithm>
#include <iostream>

#include "asteroids.h"
#include "engine/component.h"
#include "sdl-ptrs.h"

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
class Screen : public engine::Component<qp::AsteroidsMessage> {
 public:
  Screen(engine::MessageBus<qp::AsteroidsMessage>* message_bus,
         const std::string& title, const int x, const int y, const int w,
         const int h)
      : engine::Component<qp::AsteroidsMessage>(message_bus) {
    SDL_Init(SDL_INIT_VIDEO);
    window_.reset(
        SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_SHOWN));
    window_surface_.reset(SDL_GetWindowSurface(window_.get()));
  }

  void OnMessage(const qp::AsteroidsMessage& message) override {
    switch (message.type) {
      case qp::MessageType::GAME_START:
        Update();
        break;
      case qp::MessageType::FRAME_END:
        Update();
        break;
      case qp::MessageType::DRAW_SURFACE:
        surfaces_to_draw_.push_back(
            {message.surface, message.x, message.y, message.z});
        break;
      default:
        break;
    }
  }

  void Update() {
    if (!surfaces_to_draw_.empty()) {
      Clear();
      std::sort(surfaces_to_draw_.begin(), surfaces_to_draw_.end());
      for (const auto& blit_request : surfaces_to_draw_) {
        Blit(blit_request.surface, blit_request.x, blit_request.y);
      }
      SDL_UpdateWindowSurface(window_.get());
      surfaces_to_draw_.clear();
    }
  }

  void Blit(SDL_Surface* surface, const int x, const int y) {
    SDL_Rect dst_rect;
    dst_rect.x = x;
    dst_rect.y = y;
    SDL_BlitSurface(surface, /* src_rect */ NULL, window_surface_.get(),
                    &dst_rect);
  }

  void Clear() {
    SDL_FillRect(window_surface_.get(), /* destination_rect */ NULL,
                 /* color */ 0);
  }

  ~Screen() {
    window_surface_.reset();
    window_.reset();
    SDL_Quit();
  }

 private:
  qp::SdlSurfacePtr window_surface_;
  qp::SdlWindowPtr window_;
  std::vector<BlitRequest> surfaces_to_draw_;
};

}  // namespace qp

#endif /* SCREEN_H */
