#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <functional>
#include <iostream>
#include <vector>

namespace engine {

// A simple message bus which delivers messages to all subscribers. A subscriber
// may send a message from its subscribed function, but should excercise caution
// as to not create a deadlock. E.g.
//
// MessageBus<int> bus;
// auto f = [&](int x) { bus.SendMessage(x); }
// bus.AddSubscriber(f);
// bus.SendMessage(2); // Deadlocked!
template <typename Message>
class MessageBus {
 public:
  using Subscriber = std::function<void(const Message&)>;

  void AddSubscriber(Subscriber subscriber) {
    subscribers_.push_back(std::move(subscriber));
  }

  void SendMessage(const Message& message) {
    for (auto& subscriber : subscribers_) {
      subscriber(message);
    }
  }

 private:
  std::vector<Subscriber> subscribers_;
};

}  // namespace engine

#endif /* MESSAGE_BUS_H */
