#pragma once

#include <Line.hpp>

class LocationTracker {
  public:
    Point update(Point newLocation) {
        if (newLocation)
            location = newLocation.vec() + round(location.vec() - newLocation);
        return location;
    }

  private:
    Point location = {0.5, 0.5};
};