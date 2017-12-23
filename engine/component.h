#ifndef COMPONENT_H
#define COMPONENT_H

#include "message_bus.h"

namespace engine {

// A game engine component. This is the main source of logic within the game.
// Each component must override a message handling function which will be
// automatically subscribed to the message bus via the base class. Components
// should be careful not to create loops where they indefinitely respond to
// their own messages. See the class comment in message_bus.h for an example of
// this.
template <typename MessageId, typename MessagePayload>
class Component {
 public:
  Component(engine::MessageBus<MessageId, MessagePayload>* message_bus)
      : message_bus_(message_bus) {}

  virtual ~Component() = default;

  void OnMessage(const MessageId& message_id,
                 std::function<void(const MessagePayload&)> handler) {
    message_bus_->AddSubscriber(message_id, std::move(handler));
  }

  // Since std::function tries to consume anything passed to it, it doesn't
  // work with overloading. This template trick makes this version of the
  // function the "most specialized" in the case which the message handler
  // doesn't accept any arugments, forcing the compiler to pick it.
  template <typename T,
            typename = std::enable_if_t<std::is_same<T, void>::value>>
  void OnMessage(const MessageId& message_id, std::function<T()> handler) {
    message_bus_->AddIgnoringSubscriber(message_id, std::move(handler));
  }

  void SendMessage(const MessageId& message_id, const MessagePayload& payload) {
    message_bus_->SendMessage(message_id, payload);
  }

 private:
  engine::MessageBus<MessageId, MessagePayload>* message_bus_;
};

}  // namespace engine

#endif /* COMPONENT_H */
