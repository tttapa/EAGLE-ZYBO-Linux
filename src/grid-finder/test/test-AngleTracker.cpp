#include <AngleTracker.hpp>
#include <gtest/gtest.h>

TEST(AngleTracker, AngleTracker) {
    AngleTracker at;
    std::vector<angle_t> input = {
        0_deg, 60_deg, 0_deg, 30_deg, 60_deg, 0_deg, 30_deg, 0_deg, 60_deg,
    };
    std::vector<angle_t> expected = {
        0_deg, 330_deg, 0_deg, 30_deg, 60_deg, 90_deg, 120_deg, 90_deg, 60_deg,
    };
    std::vector<angle_t> result(input.size());
    std::transform(input.begin(), input.end(), result.begin(),
                   [&at](angle_t a) { return at.update(a); });
    ASSERT_EQ(result, expected);
}