#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <SFML/Graphics.hpp>
#include <vector>

class Checkbox
{
public:
    Checkbox(double, double, double, std::string, std::string, bool);
    Checkbox(Checkbox const &) = delete;
    Checkbox &operator=(Checkbox const &) = delete;
    ~Checkbox() = default;

    void render_checkbox(sf::RenderWindow &);
    bool toggle();

    sf::FloatRect get_checkbox_bounds() const;
    std::string get_option_name() const;

private:
    void load_sprite();
    
    std::vector<double> position{};
    double scale;
    std::string checkbox_string;
    std::string option_name;
    bool checked;

    sf::Sprite checkbox{};
    sf::Texture checkbox_texture{};
    sf::Text checkbox_text{};
    sf::FloatRect checkbox_bounds{};
    sf::Font font{};
};

#endif