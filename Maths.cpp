#include "Maths.h"

float dot(xyz* a,xyz* b){
	return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

xyz cross(xyz* a, xyz* b ){
	xyz c;
	c.x = a->y*b->z - a->z*b->y;
	c.y = a->z*b->x - a->x*b->z;
	c.z = a->x*b->y - a->y*b->x;
	return c;
}

xyz normal(xyz* triangle0, xyz* triangle1, xyz* triangle2){
	xyz v1, v2, n;
	v1.x = triangle1->x - triangle0->x;
	v1.y = triangle1->y - triangle0->y;
	v1.z = triangle1->z - triangle0->z;
	v2.x = triangle2->x - triangle0->x;
	v2.y = triangle2->y - triangle0->y;
	v2.z = triangle2->z - triangle0->z;
	n = cross(&v1, &v2);
	float div = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
	n.x = n.x / div;
	n.y = n.y / div;
	n.z = n.z / div;
	return n; 	
}

plane CalcPlaneEq(xyz* triangle0, xyz* triangle1, xyz* triangle2)
{
	plane p;
	p.normal=normal(triangle0, triangle1, triangle2);
	p.d=-dot(triangle0, &p.normal);
	return p;
}


float CalcPlaneEqValue(plane* p, xyz* a)
{
	return p->normal.x*a->x + p->normal.y*a->y + p->normal.z*a->z + p->d;
}
   
int sign (float number)
{
    return (number < 0.0f ? -1 : (number > 0.0f ? 1 : 0));
}

xyz planeIntersection(plane* p, xyz* p1, xyz* p2)
{
	xyz r, inter;
	float t;
	r.x = p2->x - p1->x;
	r.y = p2->y - p1->y;
	r.z = p2->z - p1->z;
	t = (-(p->d) - dot (&(p->normal), p1))  / dot(&(p->normal), &r);
	inter.x = p1->x + (r.x * t);
	inter.y = p1->y + (r.y * t);
	inter.z = p1->z + (r.z * t);

	return inter;
}

bool in_triangle(xyz* triangle0, xyz* triangle1, xyz* triangle2, xyz* p){
	xyz tri_vec[3];
	xyz tri_to_point_vec[3];
	xyz cp[3];

	tri_vec[0].x = triangle1->x - triangle0->x;
	tri_vec[0].y = triangle1->y - triangle0->y;
	tri_vec[0].z = triangle1->z - triangle0->z;

	tri_vec[1].x = triangle2->x - triangle1->x;
	tri_vec[1].y = triangle2->y - triangle1->y;
	tri_vec[1].z = triangle2->z - triangle1->z;

	tri_vec[2].x = triangle0->x - triangle2->x;
	tri_vec[2].y = triangle0->y - triangle2->y;
	tri_vec[2].z = triangle0->z - triangle2->z;

	tri_to_point_vec[0].x = p->x - triangle0->x;
	tri_to_point_vec[0].y = p->y - triangle0->y;
	tri_to_point_vec[0].z = p->z - triangle0->z;

	tri_to_point_vec[1].x = p->x - triangle1->x;
	tri_to_point_vec[1].y = p->y - triangle1->y;
	tri_to_point_vec[1].z = p->z - triangle1->z;

	tri_to_point_vec[2].x = p->x - triangle2->x;
	tri_to_point_vec[2].y = p->y - triangle2->y;
	tri_to_point_vec[2].z = p->z - triangle2->z;

	cp[0] = cross(&tri_vec[0], &tri_to_point_vec[0]);
	cp[1] = cross(&tri_vec[1], &tri_to_point_vec[1]);
	cp[2] = cross(&tri_vec[2], &tri_to_point_vec[2]);

	float d0, d1, d2;

	d0 = ( dot (&cp[0], &cp[1]));
	d1 = ( dot (&cp[1], &cp[2]));
	d2 = ( dot (&cp[2], &cp[0]));

	if ((d0 >=0 ) && (d1 >=0 )&& (d2 >=0 )) return true;
	return false;
}