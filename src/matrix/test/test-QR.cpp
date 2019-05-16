#include <gtest/gtest.h>

#include <AlmostEqual.hpp>
#include <HouseholderQR.hpp>

using Matrices::T;
using std::cout;
using std::endl;

TEST(QR, QR) {
    TMatrix<double, 3, 2> A            = {{
        {11, 12},
        {21, 22},
        {31, 32},
    }};
    QR<double, 3, 2> qtr               = householderQR(A);
    TMatrix<double, 3, 2> QTA          = qtr.applyTranspose(A);
    TMatrix<double, 3, 3> Q            = qtr.Q();
    TMatrix<double, 3, 2> U_expected   = {{
        {1.132195224629404, 0},
        {0.475278341931121, -1.074832100035896},
        {0.701601361898321, -0.919095183717348},
    }};
    TMatrix<double, 3, 2> R_expected   = {{
        {-39.025632602175733, -40.639956209488311},
        {0, 0.627661764705549},
        {0, 0},
    }};
    TMatrix<double, 3, 3> Q_expected   = {{
        {-0.281866026673626, 0.868265441176001, 0.408248290463862},
        {-0.538107869104196, 0.209220588235181, -0.816496580927726},
        {-0.794349711534765, -0.449824264705639, 0.408248290463863},
    }};
    TMatrix<double, 3, 2> QTA_expected = qtr.R;

    ASSERT_TRUE(isAlmostEqual(qtr.R, R_expected, 1e-13));
    ASSERT_TRUE(isAlmostEqual(qtr.U, U_expected, 1e-13));
    ASSERT_TRUE(isAlmostEqual(Q, Q_expected, 1e-13));
    ASSERT_TRUE(isAlmostEqual(QTA, QTA_expected, 1e-13));
    ASSERT_TRUE(isAlmostEqual(Q * qtr.R, A, 1e-13));
}