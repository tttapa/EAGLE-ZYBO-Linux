#pragma once

#include <Line.hpp>

#define GLOBAL_POSITION

class LocationTracker {
  public:
    Point update(Point newLocation) {
#ifdef GLOBAL_POSITION
        if (newLocation)
            location = newLocation.vec() + round(location.vec() - newLocation);
        return location;
#else
        return newLocation;
#endif
    }

  private:
    Point location = {0.5, 0.5};
};