#include <SDL2/SDL.h>
#include <chrono>
#include <cmath>
#include <thread>

#include "asteroids.h"
#include "engine/component.h"
#include "engine/engine.h"
#include "input.h"
#include "screen.h"

struct Ship : public engine::Component<qp::AsteroidsMessage> {
 public:
  Ship(engine::MessageBus<qp::AsteroidsMessage>* bus)
      : engine::Component<qp::AsteroidsMessage>(bus),
        x_(200),
        y_(450),
        move_amount_(2),
        surface_(SDL_LoadBMP("./ship.bmp")) {}

  void OnMessage(const qp::AsteroidsMessage& message) override {
    switch (message.type) {
      case qp::MessageType::KEY_PRESS:
        KeyPressed(message.sdl_key_code);
        break;
      case qp::MessageType::FRAME_START:
        Draw();
        break;
      default:
        break;
    }
  }

 private:
  void Translate(int dx, int dy) {
    x_ += dx;
    y_ += dy;
  }

  void Draw() {
    qp::AsteroidsMessage m(qp::MessageType::DRAW_SURFACE);
    m.x = x_;
    m.y = y_;
    m.z = 1;
    m.surface = surface_.get();
    message_bus()->SendMessage(m);
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

struct BadGuy : public engine::Component<qp::AsteroidsMessage> {
 public:
  BadGuy(engine::MessageBus<qp::AsteroidsMessage>* bus)
      : engine::Component<qp::AsteroidsMessage>(bus),
        x_(0),
        y_(0),
        move_amount_(0.5),
        surface_(SDL_LoadBMP("./ship.bmp")) {}

  void OnMessage(const qp::AsteroidsMessage& message) override {
    if (message.type == qp::MessageType::FRAME_START) {
      x_ += coef_ * move_amount_;
      y_ = 20 * std::sin(x_ / 30) + offset_;

      if (x_ < 0 || x_ > 400) {
        coef_ *= -1;
        offset_ += 10;
      }

      qp::AsteroidsMessage m(qp::MessageType::DRAW_SURFACE);
      m.x = x_;
      m.y = y_;
      m.z = 1;
      m.surface = surface_.get();
      message_bus()->SendMessage(m);
    }
  }

 private:
  double x_;
  int y_;
  double move_amount_;
  int offset_ = 20;
  int coef_ = 1;
  qp::SdlSurfacePtr surface_;
};

int main() {
  engine::MessageBus<qp::AsteroidsMessage> message_bus;

  auto screen = std::make_unique<qp::Screen>(
      &message_bus, "Asteroids", /* x */ 100,
      /* y */ 100, /* width */ 500, /* height */ 500);
  auto input = std::make_unique<qp::InputComponent>(&message_bus);
  auto ship = std::make_unique<Ship>(&message_bus);
  auto bad_guy = std::make_unique<BadGuy>(&message_bus);

  engine::Engine<qp::AsteroidsMessage> engine(
      &message_bus, qp::AsteroidsMessage(qp::MessageType::GAME_START),
      qp::AsteroidsMessage(qp::MessageType::FRAME_START),
      qp::AsteroidsMessage(qp::MessageType::FRAME_END));

  engine.AddComponent(std::move(screen));
  engine.AddComponent(std::move(input));
  engine.AddComponent(std::move(ship));
  engine.AddComponent(std::move(bad_guy));

  bool run = true;
  message_bus.AddSubscriber([&run](const qp::AsteroidsMessage& message) {
    if (message.type == qp::MessageType::GAME_END) {
      run = false;
    }
  });

  engine.BlockingGameLoop(&run);
}