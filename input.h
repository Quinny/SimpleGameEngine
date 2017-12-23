#ifndef SDL_EVENT_SOURCE_H
#define SDL_EVENT_SOURCE_H

#include <SDL2/SDL.h>

#include "asteroids.h"
#include "engine/component.h"
#include "engine/message_bus.h"

namespace qp {

// A component which reads input from SDL (key presses and window events) and
// re-sends them into the message bus.
class InputComponent
    : public engine::Component<qp::MessageType, qp::AsteroidsMessage> {
 public:
  InputComponent(
      engine::MessageBus<qp::MessageType, qp::AsteroidsMessage>* message_bus)
      : engine::Component<qp::MessageType, qp::AsteroidsMessage>(message_bus) {
    OnMessage(qp::MessageType::FRAME_START,
              std::bind(&InputComponent::PollSdlEvent, this));
  }

 private:
  void PollSdlEvent() {
    SDL_Event sdl_event;
    const bool got_event = SDL_PollEvent(&sdl_event);

    if (got_event) {
      switch (sdl_event.type) {
        case SDL_KEYDOWN:
          qp::AsteroidsMessage m;
          m.sdl_key_code = sdl_event.key.keysym.sym;
          SendMessage(qp::MessageType::KEY_PRESS, m);
          break;
        case SDL_QUIT:
          SendMessage(qp::MessageType::GAME_END, {});
          break;
      }
    }
  }
};

}  // namespace qp

#endif /* SDL_EVENT_SOURCE_H */
