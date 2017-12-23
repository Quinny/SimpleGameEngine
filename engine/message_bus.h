#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <functional>
#include <unordered_map>
#include <vector>

namespace engine {

// A simple message bus which delivers message payloads to all subscribers of
// the specified ID. A subscriber may send a message from its subscribed
// function, but should excercise caution as to not create a deadlock. E.g.
//
// MessageBus<int, int> bus;
// auto f = [&](int x) { bus.SendMessage(1, x); }
// bus.AddSubscriber(1, f);
// bus.SendMessage(1, 2); // Deadlocked!
template <typename MessageId, typename MessagePayload>
class MessageBus {
 public:
  using Subscriber = std::function<void(const MessagePayload&)>;

  void AddSubscriber(const MessageId& id, Subscriber subscriber) {
    id_to_subscribers_[id].push_back(std::move(subscriber));
  }

  // A helper for subscribers who don't actually care about the payload.
  void AddIgnoringSubscriber(const MessageId& id,
                             std::function<void()> subscriber) {
    id_to_subscribers_[id].push_back(
        [subscriber](const MessagePayload& /* payload */) { subscriber(); });
  }

  void SendMessage(const MessageId& id, const MessagePayload& payload) {
    auto it = id_to_subscribers_.find(id);
    if (it != id_to_subscribers_.end()) {
      for (const auto& subscriber : it->second) {
        subscriber(payload);
      }
    }
  }

 private:
  std::unordered_map<MessageId, std::vector<Subscriber>> id_to_subscribers_;
};

}  // namespace engine

#endif /* MESSAGE_BUS_H */
