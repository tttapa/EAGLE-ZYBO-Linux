#pragma once

#include <Line.hpp>
#include <OutlierRejection.hpp>

#if 0
#define GLOBAL_POSITION

class LocationTracker {
  public:
    Point update(Point newLocation) {
#ifdef GLOBAL_POSITION
        if (newLocation)
            outRej(newLocation.vec() +
                   round(outRej.getPreviousValid().vec() - newLocation));
        return outRej.getPreviousValid();
#else
        return newLocation;
#endif
    }

  private:
    OutlierRejection outRej = {0.2, 5, {0.5, 0.5}};
};

#endif