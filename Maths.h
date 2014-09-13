#pragma once

#include <math.h>
#define PI 3.1415926536

struct xyz{
	float x,y,z;
};

struct plane{
	xyz normal;
	float d;
};

float dot(xyz* a,xyz* b);
xyz cross(xyz* a,xyz* b);
xyz normal(xyz* trianlge0,xyz* triangle1,xyz* triangle2);
plane CalcPlaneEq(xyz* trianlge0,xyz* triangle1,xyz* triangle2);
float CalcPlaneEqValue(plane* p, xyz* a);
int sign(float number);
bool in_triangle(xyz* triangle0, xyz* triangle1, xyz* triangle2, xyz* p);
xyz planeIntersection(plane* p, xyz* p1, xyz* p2);