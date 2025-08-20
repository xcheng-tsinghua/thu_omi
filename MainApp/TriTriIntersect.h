/* Triangle/triangle intersection test routine,
 * by Tomas Moller, 1997.
 * See article "A Fast Triangle-Triangle Intersection Test",
 * Journal of Graphics Tools, 2(2), 1997
 * updated: 2001-06-20 (added line of intersection)
 *
 * int tri_tri_intersect(float V0[3],float V1[3],float V2[3],
 *                       float U0[3],float U1[3],float U2[3])
 *
 * parameters: vertices of triangle 1: V0,V1,V2
 *             vertices of triangle 2: U0,U1,U2
 * result    : returns 1 if the triangles intersect, otherwise 0
 *
 * Here is a version withouts divisions (a little faster)
 * int NoDivTriTriIsect(float V0[3],float V1[3],float V2[3],
 *                      float U0[3],float U1[3],float U2[3]);
 * 
 * This version computes the line of intersection as well (if they are not coplanar):
 * int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3], 
 *				        float U0[3],float U1[3],float U2[3],int *coplanar,
 *				        float isectpt1[3],float isectpt2[3]);
 * coplanar returns whether the tris are coplanar
 * isectpt1, isectpt2 are the endpoints of the line of intersection
 */

#include <math.h>
/*
 * parameters: vertices of triangle 1: V0,V1,V2
 *             vertices of triangle 2: U0,U1,U2
 * result    : returns 1 if the triangles intersect, otherwise 0
 */
int tri_tri_intersect(float V0[3],float V1[3],float V2[3],
                       float U0[3],float U1[3],float U2[3]);

/*
 * Here is a version withouts divisions (a little faster)
 */
int NoDivTriTriIsect(float V0[3],float V1[3],float V2[3],
                      float U0[3],float U1[3],float U2[3]);

/*
 * This version computes the line of intersection as well (if they are not coplanar):
 * coplanar returns whether the tris are coplanar
 * isectpt1, isectpt2 are the endpoints of the line of intersection
 */
int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3], 
				        float U0[3],float U1[3],float U2[3],int *coplanar,
				        float isectpt1[3],float isectpt2[3]);

int tri_tri_intersect_with_isectline_dbl(double V0[3],double V1[3],double V2[3], 
                                     double U0[3],double U1[3],double U2[3],int *coplanar,
                                     double isectpt1[3],double isectpt2[3]);
/*!
	\brief	Check whether the ray is intersected within the given triangle.
	\param  [i]_pos		triangle's vertices.
	\param  [i]orig		ray's start point.
	\param  [i]dir		ray's direction.
	\param	[i]vert0	triangle's first vertex.
	\param	[i]vert1	triangle's second vertex.
	\param	[i]vert2	triangle's third vertex.
	\param	[o]t		parameter value of the ray.
	\param	[o]u,v		parameter value in the triangle.
	\return return 1 if successful, otherwise 0.		*/  
int intersect_triangle3(double orig[3], double dir[3],
			double vert0[3], double vert1[3], double vert2[3],
			double *t, double *u, double *v);

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3]);