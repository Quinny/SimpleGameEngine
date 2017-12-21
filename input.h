#ifndef SDL_EVENT_SOURCE_H
#define SDL_EVENT_SOURCE_H

#include <SDL2/SDL.h>

#include "asteroids.h"
#include "engine/component.h"

namespace qp {

// A component which reads input from SDL (key presses and window events) and
// re-sends them into the message bus.
class InputComponent : public engine::Component<qp::AsteroidsMessage> {
 public:
  InputComponent(engine::MessageBus<qp::AsteroidsMessage>* message_bus)
      : engine::Component<qp::AsteroidsMessage>(message_bus) {}

  void OnMessage(const qp::AsteroidsMessage& message) override {
    if (message.type == qp::MessageType::FRAME_START) {
      SDL_Event sdl_event;
      const bool got_event = SDL_PollEvent(&sdl_event);

      if (got_event) {
        if (sdl_event.type == SDL_KEYDOWN) {
          qp::AsteroidsMessage m(qp::MessageType::KEY_PRESS);
          m.sdl_key_code = sdl_event.key.keysym.sym;
          message_bus()->SendMessage(m);
        } else if (sdl_event.type == SDL_QUIT) {
          message_bus()->SendMessage(
              qp::AsteroidsMessage(qp::MessageType::GAME_END));
        }
      }
    }
  }
};

}  // namespace qp

#endif /* SDL_EVENT_SOURCE_H */
