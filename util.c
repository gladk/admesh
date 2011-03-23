/*  ADMesh -- process triangulated solid meshes
 *  Copyright (C) 1995  Anthony D. Martin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *  
 *  Questions, comments, suggestions, etc to <amartin@engr.csulb.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stl.h"

static void stl_rotate(float *x, float *y, float angle);
static void stl_get_size(stl_file *stl);


void
stl_verify_neighbors(stl_file *stl)
{
  int i;
  int j;
  stl_edge edge_a;
  stl_edge edge_b;
  int neighbor;
  int vnot;

  stl->stats.backwards_edges = 0;

  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  edge_a.p1 = stl->facet_start[i].vertex[j];
	  edge_a.p2 = stl->facet_start[i].vertex[(j + 1) % 3];
	  neighbor = stl->neighbors_start[i].neighbor[j];
	  vnot = stl->neighbors_start[i].which_vertex_not[j];
	  
	  if(neighbor == -1)
	    continue;		/* this edge has no neighbor... Continue. */
	  if(vnot < 3)
	    {
	      edge_b.p1 = stl->facet_start[neighbor].vertex[(vnot + 2) % 3];
	      edge_b.p2 = stl->facet_start[neighbor].vertex[(vnot + 1) % 3];
	    }
	  else
	    {
	      stl->stats.backwards_edges += 1;
	      edge_b.p1 = stl->facet_start[neighbor].vertex[(vnot + 1) % 3];
	      edge_b.p2 = stl->facet_start[neighbor].vertex[(vnot + 2) % 3];
	    }
	  if(memcmp(&edge_a, &edge_b, SIZEOF_EDGE_SORT) != 0)
	    {
	      /* These edges should match but they don't.  Print results. */
	      printf("edge %d of facet %d doesn't match edge %d of facet %d\n",
		     j, i, vnot + 1, neighbor);
	      stl_write_facet(stl, "first facet", i);
	      stl_write_facet(stl, "second facet", neighbor);
	    }
	}
    }
}

void
stl_translate(stl_file *stl, float x, float y, float z)
{
  int i;
  int j;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl->facet_start[i].vertex[j].x -= (stl->stats.min.x - x);
	  stl->facet_start[i].vertex[j].y -= (stl->stats.min.y - y);
	  stl->facet_start[i].vertex[j].z -= (stl->stats.min.z - z);
	}
    }
  stl->stats.max.x -= (stl->stats.min.x - x);
  stl->stats.max.y -= (stl->stats.min.y - y);
  stl->stats.max.z -= (stl->stats.min.z - z);
  stl->stats.min.x = x;
  stl->stats.min.y = y;
  stl->stats.min.z = z;
}

void
stl_scale(stl_file *stl, float factor)
{
  int i;
  int j;
  
  stl->stats.min.x *= factor;
  stl->stats.min.y *= factor;
  stl->stats.min.z *= factor;
  stl->stats.max.x *= factor;
  stl->stats.max.y *= factor;
  stl->stats.max.z *= factor;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl->facet_start[i].vertex[j].x *= factor;
	  stl->facet_start[i].vertex[j].y *= factor;
	  stl->facet_start[i].vertex[j].z *= factor;
	}
    }
}

void
stl_rotate_x(stl_file *stl, float angle)
{
  int i;
  int j;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl_rotate(&stl->facet_start[i].vertex[j].y,
		     &stl->facet_start[i].vertex[j].z, angle);
	}
    }
  stl_get_size(stl);
}

void
stl_rotate_y(stl_file *stl, float angle)
{
  int i;
  int j;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl_rotate(&stl->facet_start[i].vertex[j].z,
		     &stl->facet_start[i].vertex[j].x, angle);
	}
    }
  stl_get_size(stl);
}

void
stl_rotate_z(stl_file *stl, float angle)
{
  int i;
  int j;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl_rotate(&stl->facet_start[i].vertex[j].x,
		     &stl->facet_start[i].vertex[j].y, angle);
	}
    }
  stl_get_size(stl);
}

static void
stl_rotate(float *x, float *y, float angle)
{
  double r;
  double theta;
  double radian_angle;
  
  radian_angle = (angle / 180.0) * M_PI;
  
  r = sqrt((*x * *x) + (*y * *y));
  theta = atan2(*y, *x);
  *x = r * cos(theta + radian_angle);
  *y = r * sin(theta + radian_angle);
}

static void
stl_get_size(stl_file *stl)
{
  int i;
  int j;

  stl->stats.min.x = stl->facet_start[0].vertex[0].x;
  stl->stats.min.y = stl->facet_start[0].vertex[0].y;
  stl->stats.min.z = stl->facet_start[0].vertex[0].z;
  stl->stats.max.x = stl->facet_start[0].vertex[0].x;
  stl->stats.max.y = stl->facet_start[0].vertex[0].y;
  stl->stats.max.z = stl->facet_start[0].vertex[0].z;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl->stats.min.x = STL_MIN(stl->stats.min.x,
				     stl->facet_start[i].vertex[j].x);
	  stl->stats.min.y = STL_MIN(stl->stats.min.y,
				     stl->facet_start[i].vertex[j].y);
	  stl->stats.min.z = STL_MIN(stl->stats.min.z,
				     stl->facet_start[i].vertex[j].z);
	  stl->stats.max.x = STL_MAX(stl->stats.max.x,
				     stl->facet_start[i].vertex[j].x);
	  stl->stats.max.y = STL_MAX(stl->stats.max.y,
				     stl->facet_start[i].vertex[j].y);
	  stl->stats.max.z = STL_MAX(stl->stats.max.z,
				     stl->facet_start[i].vertex[j].z);
	}
    }
}

void
stl_mirror_xy(stl_file *stl)
{
  int i;
  int j;
  float temp_size;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl->facet_start[i].vertex[j].z *= -1.0;
    	}
    }
  temp_size = stl->stats.min.z;
  stl->stats.min.z = stl->stats.max.z;
  stl->stats.max.z = temp_size;
  stl->stats.min.z *= -1.0;
  stl->stats.max.z *= -1.0;
}

void
stl_mirror_yz(stl_file *stl)
{
  int i;
  int j;
  float temp_size;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl->facet_start[i].vertex[j].x *= -1.0;
    	}
    }
  temp_size = stl->stats.min.x;
  stl->stats.min.x = stl->stats.max.x;
  stl->stats.max.x = temp_size;
  stl->stats.min.x *= -1.0;
  stl->stats.max.x *= -1.0;
}

void
stl_mirror_xz(stl_file *stl)
{
  int i;
  int j;
  float temp_size;
  
  for(i = 0; i < stl->stats.number_of_facets; i++)
    {
      for(j = 0; j < 3; j++)
	{
	  stl->facet_start[i].vertex[j].y *= -1.0;
    	}
    }
  temp_size = stl->stats.min.y;
  stl->stats.min.y = stl->stats.max.y;
  stl->stats.max.y = temp_size;
  stl->stats.min.y *= -1.0;
  stl->stats.max.y *= -1.0;
}
