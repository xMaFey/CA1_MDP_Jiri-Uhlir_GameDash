// ============================================
// Name: Jiri Uhlir
// Student ID: D00260335
// Module: Multiplayer Distributed Programming
// CA: CA1
// File: game_state.hpp
// Description: Main gameplay state (2 local players, no World/Aircraft dependencies)
// ============================================

#pragma once

#include "state.hpp"
#include <SFML/Graphics/CircleShape.hpp>

class GameState : public State
{
public:
    GameState(StateStack& stack, Context context);

    void Draw() override;
    bool Update(sf::Time dt) override;
    bool HandleEvent(const sf::Event& event) override;

private:
    void clamp_to_window(sf::CircleShape& shape);

private:
    sf::RenderWindow& m_window;

    sf::CircleShape m_p1;
    sf::CircleShape m_p2;

    float m_speed = 250.f; // pixels per second
};
