#pragma once

#include <Angle.hpp>
#include <Degrees.hpp>

class AngleTracker {
  public:
    angle_t update(angle_t newAngle) {
        int diff      = newAngle.getIndex() - previousAngle.getIndex();
        previousAngle = newAngle;
        int _45 = angle_t::indexof(45_deg);
        if (diff > _45)
            quadrant--;
        else if (diff < -_45)
            quadrant++;
        quadrant = quadrant % 4;
        return newAngle + 90_deg * quadrant;
    }

  private:
    uint8_t quadrant      = 0;
    angle_t previousAngle = 0_deg;
};