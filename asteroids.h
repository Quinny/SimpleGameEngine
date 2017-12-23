#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <SDL2/SDL.h>

// Defines the asteroids message which all components will read from the message
// bus.

namespace qp {

// The type of the message, will define what additional fields are set.
enum class MessageType {
  GAME_START,
  GAME_END,
  FRAME_START,
  FRAME_END,
  DRAW_SURFACE,
  KEY_PRESS
};

struct AsteroidsMessage {
  // Coordinates of the message. Will be set for:
  // - DRAW_SURFACE
  int x, y, z;
  // Will be set for:
  // - DRAW_SURFACE
  SDL_Surface* surface;
  // The SDL key code, will be set for:
  // - KEY_PRESS
  int sdl_key_code;
};

}  // namespace qp

#endif /* ASTEROIDS_H */
