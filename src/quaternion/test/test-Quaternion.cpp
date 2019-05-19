#include <gtest/gtest.h>

#include <AlmostEqual.hpp>
#include <EulerAngles.hpp>

static constexpr real_t eps = 1e2 * std::numeric_limits<real_t>::epsilon();

TEST(Quaternion, quatmultiply) {
    Quaternion q        = {1, 2, 5, 7};
    Quaternion r        = {11, 13, 17, 19};
    Quaternion result   = q + r;
    Quaternion expected = {-233, 11, 125, 65};
    ASSERT_EQ(result, expected);
}

TEST(Quaternion, quatrotate) {
    Quaternion q          = {1, 2, 5, 7};
    q                     = q.normalize();
    Matrix<3, 2> v        = {{
        {1, 2},
        {3, 4},
        {5, 6},
    }};
    Matrix<3, 2> result   = q.rotate(v);
    Matrix<3, 2> expected = {{
        {1.556962025316456, 1.341772151898735},
        {3.734177215189874, 4.405063291139241},
        {4.316455696202532, 5.898734177215190},
    }};
    std::cout << result << std::endl;
    ASSERT_TRUE(isAlmostEqual(result, expected, eps));
}

TEST(Quaternion, conjugate) {
    Quaternion q        = {1, 2, 5, 7};
    Quaternion result   = -q;
    Quaternion expected = {1, -2, -5, -7};
    ASSERT_EQ(result, expected);
}

#include <AnglePrinter.hpp>
#include <Degrees.hpp>

TEST(Quaternion, fromDirectionSimple) {
    ColVector<3> vector = {0.0, 1.0, sqrt(3.0)};
    Quaternion result   = Quaternion::fromDirection(vector);
    Quaternion expected = EulerAngles{0, 0, 30_deg};
    ASSERT_TRUE(isAlmostEqual(result.asColVector(), expected.asColVector(),
                              std::numeric_limits<real_t>::epsilon()));
}

TEST(Quaternion, fromDirectionHard) {
    ColVector<3> vector = {
        -0.414578098794425,
        0.829156197588850,
        0.375000000000000,
    };
    Quaternion result   = Quaternion::fromDirection(vector);
    Quaternion expected = {0.829156197588850, 0.5, 0.25, 0.0};
    ASSERT_TRUE(isAlmostEqual(result.asColVector(), expected.asColVector(),
                              std::numeric_limits<real_t>::epsilon()));
    ColVector<3> reference = {0, 0, 1};
    ColVector<3> rotated   = result.rotate(reference);
    ASSERT_TRUE(
        isAlmostEqual(rotated, vector, std::numeric_limits<real_t>::epsilon()));
}

// -------------------------------------------------------------------------- //

TEST(Quaternion, eul2quat) {
    EulerAngles e       = {2, 3, 5};
    Quaternion result   = e;
    Quaternion expected = {0.471716140466081, 0.695323669708306,
                           -0.396152384736371, -0.370232582621850};

    ASSERT_TRUE(
        isAlmostEqual(result.asColVector(), expected.asColVector(), eps));
}

TEST(Quaternion, eul2quatAndBack) {
    EulerAngles e         = {0.3, -0.7, 0.9};
    Quaternion q          = e;
    EulerAngles result    = q;
    EulerAngles expected  = e;
    Quaternion q_expected = {0.814068885161671, 0.450147392727527,
                             -0.244234643193019, 0.273877005417802};
    ASSERT_TRUE(isAlmostEqual(q.asColVector(), q_expected.asColVector(), eps));
    ASSERT_TRUE(
        isAlmostEqual(result.asColVector(), expected.asColVector(), eps));
}

TEST(Quaternion, quat2eulAndBack) {
    Quaternion q        = {1, 2, 5, 11};
    q                   = q.normalize();
    EulerAngles e       = q;
    Quaternion result   = e;
    Quaternion expected = q;
    ASSERT_TRUE(
        isAlmostEqual(result.asColVector(), expected.asColVector(), eps));
}