#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics.hpp>

class State
{
public:
    State(bool, bool, bool, int, int, unsigned int);
    State(State const &) = delete;
    State &operator=(State const &) = delete;
    virtual ~State() = default;

    virtual void handle(sf::Event &, sf::RenderWindow &, State *&,
                        std::string *) = 0;
    virtual void render(sf::RenderWindow &) = 0;

    void initialize_fps_counter(sf::RenderWindow &, int, int);
    void update_fps_counter(sf::Time);
    void load_cursor();

    unsigned int get_state_id() const;
    bool get_music_enabled() const;
    bool get_sound_effects_enabled() const;
    
    void set_background(unsigned int, unsigned int);

private:
    virtual void read_config_file() = 0;

protected:
    unsigned int game_screen_width{};
    unsigned int game_screen_height{};

    double fps_font_size{};

    bool display_fps;
    bool music_enabled;
    bool sound_effects_enabled;
    int level_id;
    int player_count;
    const unsigned int state_id;

    sf::Text fps_counter{};
    double elapsed_time{};
    sf::Font font{};
    sf::RectangleShape background{};
    sf::Color background_color{};
    sf::Cursor cursor{};
    sf::Mouse mouse{};
    bool is_paused{false};
};

#endif
