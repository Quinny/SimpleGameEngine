#include <SDL2/SDL.h>
#include <chrono>
#include <cmath>
#include <thread>

#include "asteroids.h"
#include "engine/component.h"
#include "engine/engine.h"
#include "graphics.h"
#include "input.h"

// A component which automatically sends a draw message on frame start.
class AutoDrawComponent
    : public engine::Component<qp::MessageType, qp::AsteroidsMessage> {
 public:
  AutoDrawComponent(
      engine::MessageBus<qp::MessageType, qp::AsteroidsMessage>* message_bus)
      : engine::Component<qp::MessageType, qp::AsteroidsMessage>(message_bus) {
    OnMessage(qp::MessageType::FRAME_END,
              std::bind(&AutoDrawComponent::Draw, this));
  }

  virtual int x() = 0;
  virtual int y() = 0;
  virtual int z() = 0;
  virtual SDL_Surface* surface() = 0;

 private:
  void Draw() {
    qp::AsteroidsMessage m;
    m.x = x();
    m.y = y();
    m.z = z();
    m.surface = surface();
    SendMessage(qp::MessageType::DRAW_SURFACE, m);
  }
};

// A ship which responds to controls.
struct Ship : public AutoDrawComponent {
 public:
  Ship(engine::MessageBus<qp::MessageType, qp::AsteroidsMessage>* bus)
      : AutoDrawComponent(bus),
        x_(200),
        y_(450),
        move_amount_(5),
        surface_(SDL_LoadBMP("./ship.bmp")) {
    OnMessage(qp::MessageType::KEY_PRESS,
              [this](const qp::AsteroidsMessage& message) {
                KeyPressed(message.sdl_key_code);
              });
  }

  int x() override { return x_; }
  int y() override { return y_; }
  int z() override { return 1; }
  SDL_Surface* surface() override { return surface_.get(); }

 private:
  void Translate(int dx, int dy) {
    x_ += dx;
    y_ += dy;
  }

  void KeyPressed(int sdlk_keycode) {
    switch (sdlk_keycode) {
      case SDLK_UP:
        Translate(0, -move_amount_);
        break;
      case SDLK_DOWN:
        Translate(0, move_amount_);
        break;
      case SDLK_RIGHT:
        Translate(move_amount_, 0);
        break;
      case SDLK_LEFT:
        Translate(-move_amount_, 0);
        break;
      default:
        break;
    }
  }

 private:
  int x_, y_;
  int move_amount_;
  qp::SdlSurfacePtr surface_;
};

// A ship which automatically adjusts its position each frame according to a sin
// wave.
struct BadGuy : public AutoDrawComponent {
 public:
  BadGuy(engine::MessageBus<qp::MessageType, qp::AsteroidsMessage>* bus)
      : AutoDrawComponent(bus),
        x_(0),
        y_(0),
        move_amount_(2),
        surface_(SDL_LoadBMP("./ship.bmp")) {
    OnMessage(qp::MessageType::FRAME_START,
              std::bind(&BadGuy::UpdatePosition, this));
  }

  int x() override { return x_; }
  int y() override { return y_; }
  int z() override { return 1; }
  SDL_Surface* surface() override { return surface_.get(); }

 private:
  void UpdatePosition() {
    x_ += coef_ * move_amount_;
    y_ = 20 * std::sin(x_ / 30) + offset_;

    if (x_ < 0 || x_ > 400) {
      coef_ *= -1;
      offset_ += 20;
      move_amount_ += 0.5;
    }
  }

  double x_;
  int y_;
  double move_amount_;
  int offset_ = 20;
  int coef_ = 1;
  qp::SdlSurfacePtr surface_;
};

int main() {
  engine::MessageBus<qp::MessageType, qp::AsteroidsMessage> message_bus;

  // Create each of the components.
  auto graphics = std::make_unique<qp::Graphics>(
      &message_bus, "Asteroids", /* x */ 100,
      /* y */ 100, /* width */ 500, /* height */ 500);
  auto input = std::make_unique<qp::InputComponent>(&message_bus);
  auto ship = std::make_unique<Ship>(&message_bus);
  auto bad_guy = std::make_unique<BadGuy>(&message_bus);

  engine::Engine<qp::MessageType, qp::AsteroidsMessage> engine(
      &message_bus, {qp::MessageType::GAME_START, {}},
      {qp::MessageType::FRAME_START, {}}, {qp::MessageType::FRAME_END, {}},
      /* fps */ 60);

  // Register them within the engine.
  engine.AddComponent(std::move(graphics));
  engine.AddComponent(std::move(input));
  engine.AddComponent(std::move(ship));
  engine.AddComponent(std::move(bad_guy));

  // Add one more subscriber which flips the run bit to false when a quit
  // message is recieved.
  bool run = true;
  message_bus.AddIgnoringSubscriber(qp::MessageType::GAME_END,
                                    [&run]() { run = false; });

  // Run the blocking loop.
  engine.BlockingGameLoop(&run);
}
