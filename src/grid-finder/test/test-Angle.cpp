#include <Angle.hpp>
#include <Degrees.hpp>
#include <gtest/gtest.h>
#include <limits>

TEST(Angle, sincos) {
    angle_t angle = 60_deg;
    EXPECT_EQ(angle.cos(),
              static_cast<int>(std::round(
                  std::cos(M_PI / 3) * (std::numeric_limits<int>::max() / 2))));
    EXPECT_EQ(angle.sin(),
              static_cast<int>(std::round(
                  std::sin(M_PI / 3) * (std::numeric_limits<int>::max() / 2))));
}

TEST(Angle, average) {
    double eps = 2 * M_PI * std::numeric_limits<double>::epsilon();
    EXPECT_NEAR(angle_t::average(350_deg, 10_deg).rad(), 0_deg, eps);
    EXPECT_NEAR(angle_t::average(10_deg, 350_deg).rad(), 180_deg, eps);
    EXPECT_NEAR(angle_t::average(270_deg, 90_deg).rad(), 0_deg, eps);
    EXPECT_NEAR(angle_t::average(181_deg, 179_deg).rad(), 0_deg, eps);
    EXPECT_NEAR(angle_t::average(179_deg, 181_deg).rad(), 180_deg, eps);
    EXPECT_NEAR(angle_t::average(45_deg, 135_deg).rad(), 90_deg, eps);
    EXPECT_NEAR(angle_t::average(135_deg, 45_deg).rad(), 270_deg, eps);
    EXPECT_NEAR(angle_t::average(357_deg, 359_deg).rad(), 358_deg, eps);
    EXPECT_NEAR(angle_t::average(359_deg, 357_deg).rad(), 178_deg, eps);
    EXPECT_NEAR(angle_t::average(0_deg, 2_deg).rad(), 1_deg, eps);
    EXPECT_NEAR(angle_t::average(2_deg, 0_deg).rad(), 181_deg, eps);
    EXPECT_NEAR(angle_t::average(0_deg, 0_deg).rad(), 0_deg, eps);
    EXPECT_NEAR(angle_t::average(359_deg, 359_deg).rad(), 359_deg, eps);
    EXPECT_NEAR(angle_t::average(180_deg, 180_deg).rad(), 180_deg, eps);
    EXPECT_NEAR(angle_t::average(269_deg, 271_deg).rad(), 270_deg, eps);
    EXPECT_NEAR(angle_t::average(181_deg, 179_deg).rad(), 0_deg, eps);
}

TEST(Angle, averageOfSquare) {
    std::vector<std::array<angle_t, 4>> angles = {{
        {45_deg, 135_deg, 135_deg, 225_deg},
        {225_deg, 135_deg, 135_deg, 45_deg},
        {135_deg, 225_deg, 135_deg, 45_deg},
        {44_deg, 134_deg, 134_deg, 224_deg},
        {1_deg, 91_deg, 91_deg, 181_deg},
        {359_deg, 91_deg, 91_deg, 179_deg},
        {44_deg, 136_deg, 134_deg, 226_deg},
        {200_deg, 110_deg, 340_deg, 70_deg},
    }};

    const int _45  = angle_t(45_deg).getIndex();
    const int _90  = angle_t(90_deg).getIndex();
    const int _360 = angle_t::resolution();

    for (auto &anglelist : angles) {
        int sum         = 0;
        int first_angle = anglelist[0].getIndex();
        for (auto &angle : anglelist) {
            int v = ((angle.getIndex() - first_angle + _45 + _360) % _90) +
                    (first_angle % _90) - _45;
            std::cout << v << ' ';
            sum += v;
        }
        std::cout << "→ " << ((sum / 4) % _90) << std::endl;
    }
}

TEST(Angle, difference) {
    std::vector<angle_t> a        = {180_deg, 1_deg};
    std::vector<angle_t> b        = {1_deg, 180_deg};
    std::vector<angle_t> expected = {179_deg, 181_deg};
    std::vector<angle_t> result(a.size());
    std::transform(a.begin(), a.end(), b.begin(), result.begin(),
                   [](angle_t a, angle_t b) { return a - b; });
    ASSERT_EQ(expected, result);
}