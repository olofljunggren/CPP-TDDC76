#include "catch.hpp"
#include "../include/tank.h"
#include "../include/level.h"
#include "../include/projectile.h"
#include "../include/powerup.h"
#include <SFML/Graphics.hpp>

using namespace std;

TEST_CASE ("Collision and other functions within the game")
{
    int game_screen_width{1600};
    int game_screen_height{900};
    sf::RenderWindow window{};
    Tank* tank = new Tank{100, 100, 90, 3, 1, game_screen_width, game_screen_height};
    Bullet* bullet = new Bullet{105, 105, 45, 4, 1, 20};
    BouncingBullet* bouncing_bullet = new BouncingBullet{105, 105, 45, 3, 1, 20};
    Explosive* explosive = new Explosive{105, 105, 45, 2, 1, 20};
    Powerup* powerup = new Powerup{4, 100, 100, 1};

  SECTION("Getters")
    {
      CHECK( tank -> get_object_id() == 1 );
      CHECK( tank -> get_tank_id() == 3 );
      CHECK( tank -> get_powerup() ==  0);
      CHECK( tank -> get_shield() ==  50);
 
      CHECK( bouncing_bullet -> get_object_id() == 5 );
      CHECK( bouncing_bullet -> get_damage() == 40 );
      CHECK( bouncing_bullet -> get_remaining_bounces() == 5 );
      CHECK( bouncing_bullet -> get_shooter_id() == 3 );

      CHECK( bullet -> get_object_id() == 2 );
      CHECK( bullet -> get_damage() == 40 );
      CHECK( bullet -> get_shooter_id() == 4 );

      CHECK( explosive -> get_object_id() == 0 );
      CHECK( explosive -> get_damage() == 70 );
      CHECK( explosive -> get_shooter_id() == 2 );

      CHECK( powerup -> get_object_id() == 3 );
      CHECK( powerup -> get_powerup() == 4 );     
   
      Level level{1, 1, game_screen_width, game_screen_height};
      level.render_objects(window);
      // Game over expected as only one player was spawned.
      CHECK( level.get_game_over() );

      Level level2{1, 3, game_screen_width, game_screen_height};
      CHECK( !level2.get_game_over() );
    }
  SECTION("Collision")
    {
      CHECK( ! tank -> get_invulnerable() );
      tank -> collides(bullet);
      CHECK( tank -> get_invulnerable() );
      CHECK( tank -> get_shield() == 100 - 90 );

      tank -> collides(explosive);
      CHECK( ! explosive -> get_has_exploded() );
      // No further tank damage expected as tank still invulnerable.
      CHECK( tank -> get_shield() == 0 );

      tank -> collides(bouncing_bullet);
      // No further tank damage expected as tank hit by own shot.
      CHECK( tank -> get_tank_id() == bouncing_bullet -> get_shooter_id() );
      CHECK( tank -> get_shield() == 0 );

      tank -> collides(powerup);
      CHECK( tank -> get_powerup() == 4 );
    }

  SECTION("Other functions")
    {
      CHECK( tank -> get_shield() ==  50);
    }

   delete tank;
   delete bullet;
   delete explosive;
   delete bouncing_bullet;
   delete powerup;
}