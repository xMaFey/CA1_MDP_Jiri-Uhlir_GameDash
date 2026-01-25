// ============================================
// Name: Jiri Uhlir
// Student ID: D00260335
// ============================================

#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>

class PlayerEntity
{
public:
    PlayerEntity();

    void set_position(sf::Vector2f p);
    sf::Vector2f position() const;

    void set_color(const sf::Color& c);
    void set_controls_arrows_j();
    void set_controls_wasd_backtick();

    void update(sf::Time dt, const std::vector<sf::RectangleShape>& walls);
    void draw(sf::RenderTarget& target) const;

    bool can_shoot() const;
    void on_shot_fired();
    sf::Vector2f facing_dir() const;

    void respawn(sf::Vector2f p);
    bool is_invulnerable() const { return m_invulnerable; }

private:
    void handle_input(sf::Vector2f& dir) const;
    void resolve_walls(const std::vector<sf::RectangleShape>& walls);
    static bool circle_rect_intersect(const sf::CircleShape& c, const sf::RectangleShape& r);

private:
    sf::CircleShape m_body;
    sf::Vector2f m_velocity{ 0.f, 0.f };
    sf::Vector2f m_last_dir{ 1.f, 0.f };

    float m_speed = 280.f;

    // shooting cooldown
    sf::Time m_shoot_cd = sf::seconds(0.2f);
    sf::Time m_shoot_timer = sf::Time::Zero;

    // controls
    // movement
    sf::Keyboard::Scancode m_up{};
    sf::Keyboard::Scancode m_down{};
    sf::Keyboard::Scancode m_left{};
    sf::Keyboard::Scancode m_right{};
    // actions
    sf::Keyboard::Scancode m_shoot{};
    // spawn protection
	bool m_invulnerable = false;
    sf::Time m_invulnerable_duration = sf::seconds(1.5f);
	sf::Time m_invulnerable_time = sf::Time::Zero;
};
