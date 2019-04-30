#pragma once

#include <Line.hpp>

class LocationTracker {
  public:
    Point update(Point newLocation) {
#if 0
        Point diff       = newLocation.vec() - location.vec();
        location = newLocation;
        if (diff.x > 0.5)
            x_square--;
        else if (diff.x < -0.5)
            x_square++;
        if (diff.y > 0.5)
            y_square--;
        else if (diff.y < -0.5)
            y_square++;
#else
        if (newLocation)
            location = newLocation.vec() + round(location.vec() - newLocation);
#endif
        return location;
    }

  private:
    Point location = {0.5, 0.5};
};