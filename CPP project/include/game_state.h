#ifndef GAME_STATE
#define GAME_STATE

#include <SFML/Graphics.hpp>

#include "level.h"
#include "button.h"
#include "state.h"

class Game_State : public State
{
public:
    Game_State(sf::RenderWindow &, bool, bool, bool, int, int);
    ~Game_State() override;
    Game_State &operator=(const Game_State &) = delete;
    Game_State(const Game_State &) = delete;

    void handle(sf::Event &, sf::RenderWindow &, State *&,
                std::string *) override;
    void update(sf::Time, std::string *);
    void render(sf::RenderWindow &) override;

    double get_scale() const;

private:
    void read_config_file() override;
    void initialize_game_over_text();

    Button *game_over_button{};
    sf::Text game_over{};
    double scale{};
    Level *current_level{};
};

#endif