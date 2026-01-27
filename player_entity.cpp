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
    set_controls_wasd();
}

void PlayerEntity::set_position(sf::Vector2f p) { m_body.setPosition(p); }
sf::Vector2f PlayerEntity::position() const { return m_body.getPosition(); }
void PlayerEntity::set_color(const sf::Color& c) { m_body.setFillColor(c); }

void PlayerEntity::set_controls_arrows()
{
    m_up = sf::Keyboard::Scancode::Up;
    m_down = sf::Keyboard::Scancode::Down;
    m_left = sf::Keyboard::Scancode::Left;
    m_right = sf::Keyboard::Scancode::Right;

    m_shoot = sf::Keyboard::Scancode::J;
	m_dash = sf::Keyboard::Scancode::K;
	m_melee = sf::Keyboard::Scancode::L;
}

void PlayerEntity::set_controls_wasd()
{
    m_up = sf::Keyboard::Scancode::W;
    m_down = sf::Keyboard::Scancode::S;
    m_left = sf::Keyboard::Scancode::A;
    m_right = sf::Keyboard::Scancode::D;

    m_shoot = sf::Keyboard::Scancode::Grave; // ` key
	m_dash = sf::Keyboard::Scancode::Num1;
	m_melee = sf::Keyboard::Scancode::Num2;
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

    // dash + melee cd
	m_dash_cd_timer += dt;
	m_melee_cd_timer += dt;

    // detect single press for dash
	const bool dash_now = sf::Keyboard::isKeyPressed(m_dash);
	const bool dash_pressed = dash_now && !m_dash_pressed_prev;
	m_dash_pressed_prev = dash_now;

	// detect single press for melee
	const bool melee_now = sf::Keyboard::isKeyPressed(m_melee);
	const bool melee_pressed = melee_now && !m_melee_pressed_prev;
	m_melee_pressed_prev = melee_now;

    // read movement direction - for facing + normal movement
    sf::Vector2f dir;
    handle_input(dir);

    if (dir.x != 0.f || dir.y != 0.f)
        m_last_dir = dir;

    // dash logic
    if(dash_pressed && !m_is_dashing && m_dash_cd_timer >= m_dash_cd)
    {
        m_is_dashing = true;
        m_dash_time = sf::Time::Zero;
        m_dash_cd_timer = sf::Time::Zero;
        
        // dash in the direction you are facing
		m_dash_dir = (dir.x != 0.f || dir.y != 0.f) ? dir : m_last_dir;
        if (m_dash_dir.x == 0.f && m_dash_dir.y == 0.f)
            m_dash_dir = { 1.f, 0.f };
	}

    // melee logic
    if (melee_pressed && !m_melee_active && m_melee_cd_timer >= m_melee_cd)
    {
        m_melee_active = true;
        m_melee_active_time = sf::Time::Zero;
		m_melee_cd_timer = sf::Time::Zero;
    }

    // dash overrides normal movement while active (dash is priority)
    if(m_is_dashing)
    {
        m_dash_time += dt;
		m_velocity = m_dash_dir * m_dash_speed;

        if(m_dash_time >= m_dash_duration)
        {
            m_is_dashing = false;
        }
	}
    else
    {
		m_velocity = dir * m_speed;
    }

    // move + collide
    m_body.move(m_velocity * dt.asSeconds());
    resolve_walls(walls);

    // melee active window timing
    if(m_melee_active)
    {
        m_melee_active_time += dt;
        if (m_melee_active_time >= m_melee_active_window)
        {
            m_melee_active = false;
        }
	}
}

void PlayerEntity::draw(sf::RenderTarget& target) const
{
    // visible invulnerability - blink effect
    if (m_invulnerable)
    {
        const int tick = static_cast<int>(m_invulnerable_time.asSeconds() * 10.f);
        if (tick % 2 == 1)
			return;
    }

    target.draw(m_body);

	// show melee hitbox when active - FOR TESTING PURPOSES
    if (m_melee_active)
    {
        sf::RectangleShape r;
		const sf::FloatRect hb = get_melee_hitbox_world();
		r.setPosition({ hb.position.x, hb.position.y });
		r.setSize({ hb.size.x, hb.size.y });
		r.setFillColor(sf::Color(255, 255, 255, 50));
		target.draw(r);
	}
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

	// reset dash / melee states
    m_is_dashing = false;
	m_melee_active = false;
	m_dash_time = sf::Time::Zero;
	m_melee_active_time = sf::Time::Zero;
}

sf::FloatRect PlayerEntity::get_melee_hitbox_world() const
{
    // "punch" = thin rectangle in facing direction
    const float melee_range = 60.f;
    const float melee_width = 18.f;
	const float radius = m_body.getRadius();

	const sf::Vector2f p = m_body.getPosition();
	const sf::Vector2f d = m_last_dir;

	// decide if horizontal or vertical
    if (std::abs(d.x) >= std::abs(d.y))
    {
        // horizontal
		const float sign = (d.x >= 0.f) ? 1.f : -1.f;
		const float cx = p.x + sign * (radius + melee_range * 0.5f);
		const float cy = p.y;

        return sf::FloatRect(
            { cx - melee_range * 0.5f, cy - melee_width * 0.5f },
            { melee_range, melee_width }
        );
    }
    else
    {
        // vertical
		const float sign = (d.y >= 0.f) ? 1.f : -1.f;
		const float cx = p.x;
        const float cy = p.y + sign * (radius + melee_range * 0.5f);

        return sf::FloatRect(
            { cx - melee_width * 0.5f, cy - melee_range * 0.5f },
            { melee_width, melee_range }
		);
    }
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
