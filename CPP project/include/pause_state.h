#ifndef PAUSE_STATE
#define PAUSE_STATE

#include <SFML/Graphics.hpp>

#include "button.h"
#include "state.h"

class Pause_State : public State
{
public:
    Pause_State(sf::RenderWindow &, State *&, bool, bool, bool, int, int);
    Pause_State &operator=(const Pause_State &) = delete;
    Pause_State(const Pause_State &) = delete;
    ~Pause_State() override;

    void handle(sf::Event &, sf::RenderWindow &, State *&, std::string *) override;
    void update(sf::Time );
    void render(sf::RenderWindow &) override;


private:
    void read_config_file() override;
    std::vector<Button *> buttons{};
    State *previous_state;
    double scale{};
};

#endif