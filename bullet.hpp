// ============================================
// Name: Jiri Uhlir
// Student ID: D00260335
// ============================================

#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include "SFML/Graphics/Drawable.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <vector>

class Bullet
{
public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir, int owner_id);

    void update(sf::Time dt);
    void draw(sf::RenderTarget& target) const;

    bool is_dead() const;
    void kill();

    int owner() const;

    const sf::CircleShape& shape() const;

private:
    sf::CircleShape m_shape;
    sf::Vector2f m_velocity;
    int m_owner_id = 0;
    bool m_dead = false;
};
