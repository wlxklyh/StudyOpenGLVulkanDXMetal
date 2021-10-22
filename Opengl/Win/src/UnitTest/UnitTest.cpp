#include "gtest/gtest.h"
#include "../RMath/Vec3.h"
#include "../RMath/Ray.h"

using namespace RMath;
TEST(Vec3, Operater
) {
FVec3 vec1(1.2, 1.2, 1.2);
FVec3 vec2(2.4, 2.4, 2.4);

FVec3 vec3(2, 2, 2);

ASSERT_EQ(vec1
+ vec1, vec2);
ASSERT_EQ(vec2
- vec1, vec1);
ASSERT_EQ(vec1
* vec3, vec2);
ASSERT_EQ(vec2
/ vec3, vec1);
ASSERT_EQ(vec2
/ 2, vec1);
ASSERT_EQ(vec1
* 2, vec2);
ASSERT_EQ(2 * vec1, vec2);
}

TEST(Ray, Point
) {
FVec3 vecA(1, 1, 1);
FVec3 vecB(2, 2, 2);
FVec3 vecC(3, 3, 3);
ASSERT_EQ(Ray(vecA, vecB)
.PointAtParameter(1), vecC);
}

TEST(Ray, Dir
) {
FVec3 vecA(1, 1, 1);
FVec3 vecB(2, 2, 2);
FVec3 vecC(3, 3, 3);
ASSERT_EQ(Ray(vecA, vecB)
.

Direction(), vecB

);
}