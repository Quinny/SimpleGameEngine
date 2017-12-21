#ifndef ENGINE_H
#define ENGINE_H

#include <memory>

#include "component.h"
#include "message_bus.h"

namespace engine {

// The main game engine. The only responsibilites here are to create the game
// loop and send boilerplate messages which kick off the registered components.
template <typename Message>
class Engine {
 public:
  Engine(MessageBus<Message>* message_bus, const Message& start_message,
         const Message& start_frame_message, const Message& end_frame_message)
      : message_bus_(message_bus),
        start_message_(start_message),
        start_frame_message_(start_frame_message),
        end_frame_message_(end_frame_message) {}

  void AddComponent(std::unique_ptr<Component<Message>> component) {
    components_.push_back(std::move(component));
  }

  // Runs a blocking game loop until the value of |run| is false.
  void BlockingGameLoop(bool* run) {
    message_bus_->SendMessage(start_message_);
    while (*run) {
      message_bus_->SendMessage(start_frame_message_);
      message_bus_->SendMessage(end_frame_message_);
    }
  }

 private:
  MessageBus<Message>* message_bus_;
  Message start_message_;
  Message start_frame_message_;
  Message end_frame_message_;

  // This technically does not need to be here, but it makes it clear that the
  // engine owns the components and prevents users of the API from modifying
  // them from outside the engine.
  std::vector<std::unique_ptr<Component<Message>>> components_;
};

}  // namespace engine

#endif /* ENGINE_H */
