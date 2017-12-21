#ifndef COMPONENT_H
#define COMPONENT_H

#include "message_bus.h"

namespace engine {

// A game engine component. This is the main source of logic within the game.
// Each component must override a message handling function which will be
// automatically subscribed to via the base class. Components should be careful
// not to create loops where they indefinitely respond to their own messages.
template <typename Message>
class Component {
 public:
  Component(MessageBus<Message>* message_bus) {
    message_bus->AddSubscriber(
        [this](const Message& message) { OnMessage(message); });
    message_bus_ = message_bus;
  }

  MessageBus<Message>* message_bus() { return message_bus_; }

  virtual ~Component() = default;

  virtual void OnMessage(const Message& message) = 0;

 private:
  MessageBus<Message>* message_bus_;
};

}  // namespace

#endif /* COMPONENT_H */
