#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>

#include "sdl-ptrs.h"

namespace qp {

// A nice wrapper around an SDL screen.
class Screen {
 public:
  Screen(const std::string& title, const int x, const int y, const int w,
         const int h) {
    SDL_Init(SDL_INIT_VIDEO);
    window_.reset(
        SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_SHOWN));
    window_surface_.reset(SDL_GetWindowSurface(window_.get()));
  }

  void Update() { SDL_UpdateWindowSurface(window_.get()); }

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
};

}  // namespace qp

#endif /* SCREEN_H */
