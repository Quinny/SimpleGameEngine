#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "message_bus.h"

namespace engine {

// The main game engine. The only responsibilites here are to create the game
// loop and send boilerplate messages which kick off the registered components.
template <typename MessageId, typename MessagePayload>
class Engine {
 public:
  using MessageIdAndPayload = std::pair<MessageId, MessagePayload>;
  Engine(MessageBus<MessageId, MessagePayload>* message_bus,
         const MessageIdAndPayload& start_message,
         const MessageIdAndPayload& start_frame_message,
         const MessageIdAndPayload& end_frame_message, const int fps)
      : message_bus_(message_bus),
        start_message_(start_message),
        start_frame_message_(start_frame_message),
        end_frame_message_(end_frame_message),
        seconds_per_frame_(1.0 / fps) {}

  void AddComponent(
      std::unique_ptr<Component<MessageId, MessagePayload>> component) {
    components_.push_back(std::move(component));
  }

  // Runs a blocking game loop until the value of |run| is false.
  void BlockingGameLoop(bool* run) {
    using Clock = std::chrono::high_resolution_clock;
    message_bus_->SendMessage(start_message_.first, start_message_.second);

    while (*run) {
      const auto frame_start = Clock::now();
      message_bus_->SendMessage(start_frame_message_.first,
                                start_frame_message_.second);
      message_bus_->SendMessage(end_frame_message_.first,
                                end_frame_message_.second);
      const auto frame_duration = Clock::now() - frame_start;
      if (frame_duration < seconds_per_frame_) {
        std::this_thread::sleep_for(seconds_per_frame_ - frame_duration);
      } else {
        std::cout << "Frame lasted longer than allocated time of "
                  << seconds_per_frame_.count() << "s" << std::endl;
      }
    }
  }

 private:
  MessageBus<MessageId, MessagePayload>* message_bus_;
  MessageIdAndPayload start_message_;
  MessageIdAndPayload start_frame_message_;
  MessageIdAndPayload end_frame_message_;
  std::chrono::duration<double> seconds_per_frame_;

  // This technically does not need to be here, but it makes it clear that the
  // engine owns the components and prevents users of the API from modifying
  // them from outside the engine.
  std::vector<std::unique_ptr<Component<MessageId, MessagePayload>>>
      components_;
};

}  // namespace engine

#endif /* ENGINE_H */
