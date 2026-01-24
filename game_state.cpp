// ============================================
// Name: Jiri Uhlir
// Student ID: D00260335
// Module: Multiplayer Distributed Programming
// CA: CA1
// File: game_state.cpp
// Description: 2 local players moving on screen. ESC pauses.
// ============================================

#include "game_state.hpp"
#include "stateid.hpp"
#include <algorithm>

GameState::GameState(StateStack& stack, Context context)
    : State(stack, context)
    , m_window(*context.window)
{
    // Player 1: circle
    m_p1.setRadius(25.f);
    m_p1.setOrigin({ 25.f, 25.f });
    m_p1.setPosition({ 200.f, 300.f });

    // Player 2: circle
    m_p2.setRadius(25.f);
    m_p2.setOrigin({ 25.f, 25.f });
    m_p2.setPosition({ 800.f, 300.f });
}

void GameState::Draw()
{
    m_window.draw(m_p1);
    m_window.draw(m_p2);
}

bool GameState::Update(sf::Time dt)
{
    const float s = m_speed * dt.asSeconds();

    // Player 1 movement (WASD)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) m_p1.move({ 0.f, -s });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) m_p1.move({ 0.f,  s });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) m_p1.move({ -s, 0.f });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) m_p1.move({ s, 0.f });

    // Player 2 movement (Arrow keys)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up))    m_p2.move({ 0.f, -s });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down))  m_p2.move({ 0.f,  s });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))  m_p2.move({ -s, 0.f });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) m_p2.move({ s, 0.f });

    clamp_to_window(m_p1);
    clamp_to_window(m_p2);

    return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
    // ESC -> pause
    const auto* keypress = event.getIf<sf::Event::KeyPressed>();
    if (keypress && keypress->scancode == sf::Keyboard::Scancode::Escape)
    {
        RequestStackPush(StateID::kPause);
    }
    return true;
}

void GameState::clamp_to_window(sf::CircleShape& shape)
{
    const float r = shape.getRadius();
    sf::Vector2f p = shape.getPosition();
    const sf::Vector2u ws = m_window.getSize();

    p.x = std::clamp(p.x, r, ws.x - r);
    p.y = std::clamp(p.y, r, ws.y - r);

    shape.setPosition(p);
}
