// Standard library includes-
#include <vector>

// Header-file includes-
#include "../include/static_object.h"

using namespace std;

// Constructor --- initializes object id, scale and position.
StaticObject::StaticObject(double x_pos, double y_pos, double scale, int id): 
                            id{id}, scale{scale}
{
    position.push_back(x_pos);
    position.push_back(y_pos);
}

// Returns the position of the object.
vector<double> StaticObject::get_position() const
{
    return position;
}

// Return the id of the object.
int StaticObject::get_object_id() const
{
    return id;
}