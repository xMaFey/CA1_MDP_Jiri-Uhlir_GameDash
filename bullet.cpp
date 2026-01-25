// ============================================
// Name: Jiri Uhlir
// Student ID: D00260335
// ============================================

#include "bullet.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir, int owner_id)
    : m_owner_id(owner_id)
{
    m_shape.setRadius(6.f);
    m_shape.setOrigin({ 6.f, 6.f });
    m_shape.setPosition(pos);

    // normalize dir
    const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f)
    {
        dir.x /= len;
        dir.y /= len;
    }
    else
    {
        dir = { 1.f, 0.f };
    }

    const float speed = 620.f;
    m_velocity = dir * speed;
}

void Bullet::update(sf::Time dt)
{
    m_shape.move(m_velocity * dt.asSeconds());
}

void Bullet::draw(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}

bool Bullet::is_dead() const { return m_dead; }
void Bullet::kill() { m_dead = true; }

int Bullet::owner() const { return m_owner_id; }
const sf::CircleShape& Bullet::shape() const { return m_shape; }
