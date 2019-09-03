#pragma once

#include <Line.hpp>  // Point
#include <cassert>   // assert
#include <cmath>     // std::abs

/** 
 * @brief   Class for outlier rejection filters. Returns the input value if the 
 *          if the difference from the previous valid input is small enough.
 */
class OutlierRejection {
  public:
    /** 
     * @brief   Construct a new Outlier Rejection Filter.
     * 
     * @param   maxJump
     *          If the difference between the previous valid input and the new 
     *          input is greater than this value, the new measurement is 
     *          considered an outlier, and it's discarded.
     * @param   consecutiveOutliers
     *          If this number of outliers are detected in a row, the average of
     *          the last `consecutiveOutliers` inputs will be the new output 
     *          value. Should be greater than zero.
     * @param   initialValue
     *          The initial value that the first input will be compared against.
     */
    OutlierRejection(float maxJump, unsigned int consecutiveOutliers,
                     Point initialValue = {})
        : maxJump(maxJump), consecutiveOutliers(consecutiveOutliers),
          previousValid(initialValue) {
        assert(consecutiveOutliers);
    }

    /** 
     * @brief   Calculate the output @f$ y[n] @f$ for a given input 
     *          @f$ x[n] @f$.
     * @return  The input @f$ x[n] @f$, if the difference between the new input
     *          and the last valid input is less than or equal to the maximum
     *          jump threshold.  
     *          @f$ \big|x[n]-x[n-i]\big| \le \text{maxJump} @f$, where 
     *          @f$ i @f$ is the smallest index that satisfies: 
     *          @f$ \big|x[n-i]-y[n-i-1]\big| \le \text{maxJump} @f$.
     * @return  The last output @f$ y[n-1] @f$,
     *          if @f$ \big|x[n]-x[n-i]\big| \gt \text{maxJump} @f$ and 
     *          @f$ i \lt \text{consecutiveOutliers} @f$.
     * @return  The average of the last @f$ \text{consecutiveOutliers} @f$ 
     *          inputs, if @f$ \big|x[n]-x[n-i]\big| \gt \text{maxJump} @f$ and 
     *          @f$ i \ge \text{consecutiveOutliers} @f$.
     */
    Point operator()(Point p) {
        float jump = std::max(std::abs(p.x - previousValid.x),
                              std::abs(p.y - previousValid.y));
        if (jump <= maxJump) {
            previousValid              = p;
            outlierSum                 = {};
            consecutiveOutliersCounter = 0;
        } else {
            ++consecutiveOutliersCounter;
            outlierSum += p;
            if (consecutiveOutliersCounter >= consecutiveOutliers) {
                previousValid              = outlierSum / consecutiveOutliers;
                outlierSum                 = {};
                consecutiveOutliersCounter = 0;
            }
        }
        return previousValid;
    }

    Point getPreviousValid() const { return previousValid; }

  private:
    /// Jumps that are greater than this value are considered outliers.
    float maxJump;
    /// After how many consecutive outliers do we accept the new value as valid.
    unsigned int consecutiveOutliers;

    /// The previous input that didn't jump.
    Point previousValid;
    /// How many outliers we've encountered in a row.
    unsigned int consecutiveOutliersCounter = 0;
    /// The sum of the last streak of consecutive outliers
    /// (to calculate the average).
    TColVector<float, 2> outlierSum = {};
};