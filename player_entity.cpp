// ============================================
// Name: Jiri Uhlir
// Student ID: D00260335
// ============================================

#include "player_entity.hpp"
#include <algorithm>
#include <cmath>
#include <SFML/Window/Keyboard.hpp>

PlayerEntity::PlayerEntity()
{
    m_body.setRadius(18.f);
    m_body.setOrigin({ 18.f, 18.f });
    m_body.setPosition({ 100.f, 100.f });

    // default controls (can be overridden)
    set_controls_wasd_backtick();
}

void PlayerEntity::set_position(sf::Vector2f p) { m_body.setPosition(p); }
sf::Vector2f PlayerEntity::position() const { return m_body.getPosition(); }
void PlayerEntity::set_color(const sf::Color& c) { m_body.setFillColor(c); }

void PlayerEntity::set_controls_arrows_j()
{
    m_up = sf::Keyboard::Scancode::Up;
    m_down = sf::Keyboard::Scancode::Down;
    m_left = sf::Keyboard::Scancode::Left;
    m_right = sf::Keyboard::Scancode::Right;
    m_shoot = sf::Keyboard::Scancode::J;
}

void PlayerEntity::set_controls_wasd_backtick()
{
    m_up = sf::Keyboard::Scancode::W;
    m_down = sf::Keyboard::Scancode::S;
    m_left = sf::Keyboard::Scancode::A;
    m_right = sf::Keyboard::Scancode::D;
    m_shoot = sf::Keyboard::Scancode::Grave; // ` key
}

void PlayerEntity::handle_input(sf::Vector2f& dir) const
{
    dir = { 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(m_up)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(m_down)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(m_left)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(m_right)) dir.x += 1.f;

    // normalize diagonal
    const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f)
    {
        dir.x /= len;
        dir.y /= len;
    }
}

void PlayerEntity::update(sf::Time dt, const std::vector<sf::RectangleShape>& walls)
{
    m_shoot_timer += dt;

    if(m_invulnerable)
    {
        m_invulnerable_time += dt;
        if(m_invulnerable_time >= m_invulnerable_duration)
        {
            m_invulnerable = false;
        }
	}

    sf::Vector2f dir;
    handle_input(dir);

    if (dir.x != 0.f || dir.y != 0.f)
        m_last_dir = dir;

    m_velocity = dir * m_speed;

    // move + collide
    m_body.move(m_velocity * dt.asSeconds());
    resolve_walls(walls);
}

void PlayerEntity::draw(sf::RenderTarget& target) const
{
    target.draw(m_body);
}

sf::Vector2f PlayerEntity::facing_dir() const
{
    return m_last_dir;
}

bool PlayerEntity::can_shoot() const
{
    return m_shoot_timer >= m_shoot_cd && sf::Keyboard::isKeyPressed(m_shoot);
}

void PlayerEntity::on_shot_fired()
{
    m_shoot_timer = sf::Time::Zero;
}

void PlayerEntity::respawn(sf::Vector2f p)
{
    m_body.setPosition(p);
    m_velocity = { 0.f, 0.f };

    m_invulnerable = true;
    m_invulnerable_time = sf::Time::Zero;
}

bool PlayerEntity::circle_rect_intersect(const sf::CircleShape& c, const sf::RectangleShape& r)
{
    const sf::Vector2f cc = c.getPosition();
    const float radius = c.getRadius();

    const sf::FloatRect rb = r.getGlobalBounds();
    const float closest_x = std::clamp(cc.x, rb.position.x, rb.position.x + rb.size.x);
    const float closest_y = std::clamp(cc.y, rb.position.y, rb.position.y + rb.size.y);

    const float dx = cc.x - closest_x;
    const float dy = cc.y - closest_y;

    return (dx * dx + dy * dy) <= (radius * radius);
}

void PlayerEntity::resolve_walls(const std::vector<sf::RectangleShape>& walls)
{
    // Simple resolution: if overlapping, push player out along smallest axis
    for (const auto& w : walls)
    {
        if (!circle_rect_intersect(m_body, w))
            continue;

        const sf::FloatRect rb = w.getGlobalBounds();
        const sf::Vector2f p = m_body.getPosition();
        const float r = m_body.getRadius();

        // compute overlap distances in x/y against rect expanded by circle radius
        const float left = (rb.position.x - r);
        const float right = (rb.position.x + rb.size.x + r);
        const float top = (rb.position.y - r);
        const float bottom = (rb.position.y + rb.size.y + r);

        const float dx_left = std::abs(p.x - left);
        const float dx_right = std::abs(right - p.x);
        const float dy_top = std::abs(p.y - top);
        const float dy_bottom = std::abs(bottom - p.y);

        // push out to nearest edge
        const float minx = std::min(dx_left, dx_right);
        const float miny = std::min(dy_top, dy_bottom);

        sf::Vector2f newp = p;
        if (minx < miny)
        {
            // push horizontally
            newp.x = (dx_left < dx_right) ? left : right;
        }
        else
        {
            // push vertically
            newp.y = (dy_top < dy_bottom) ? top : bottom;
        }
        m_body.setPosition(newp);
    }
}
