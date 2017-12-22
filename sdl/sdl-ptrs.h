#ifndef SDL_PTRS_H
#define SDL_PTRS_H

#include <memory>

#include "SDL2/SDL.h"

// Managed RAII pointers to SDL objects.

namespace qp {
namespace detail {

struct SdlWindowDeleter {
  void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
};

struct SdlSurfaceDeleter {
  void operator()(SDL_Surface* surface) const { SDL_FreeSurface(surface); }
};

}  // namespace detail

using SdlWindowPtr = std::unique_ptr<SDL_Window, detail::SdlWindowDeleter>;
using SdlSurfacePtr = std::unique_ptr<SDL_Surface, detail::SdlSurfaceDeleter>;

}  // namespace qp

#endif /* SDL_PTRS_H */
