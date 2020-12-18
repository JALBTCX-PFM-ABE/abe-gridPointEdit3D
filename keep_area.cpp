
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.
*********************************************************************************************/


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#include "gridPointEdit3D.hpp"

/***************************************************************************\
*                                                                           *
*   Module Name:        keep_area                                           *
*                                                                           *
*   Programmer(s):      Rebecca Martinolich                                 *
*                                                                           *
*   Date Written:       October 1998                                        *
*                                                                           *
*   Purpose:            Invalidates data outside a polygon.                 *
*                                                                           *
*   Arguments:          x_bounds        -   array of x vertices             *
*                       y_bounds        -   array of y vertices             *
*                       num_vertices    -   number of vertices              *
*                                                                           *
*   Return Value:       None                                                *
*                                                                           *
*   Calling Routines:   ButtonReleaseEH                                     *
*                                                                           * 
\***************************************************************************/

void keep_area (nvMapGL *map, OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t *x_bounds, int32_t *y_bounds, int32_t num_vertices)
{
  uint8_t init2D = NVTrue;


  //  We have to convert to double so that the "inside" function will work.

  double *mx = (double *) malloc (num_vertices * sizeof (double));

  if (mx == NULL)
    {
      perror ("Allocating mx array in keep_area");
      exit (-1);
    }

  double *my = (double *) malloc (num_vertices * sizeof (double));

  if (my == NULL)
    {
      perror ("Allocating my array in keep_area");
      exit (-1);
    }

  for (int32_t i = 0 ; i < num_vertices ; i++)
    {
      mx[i] = (double) x_bounds[i];
      my[i] = (double) y_bounds[i];
    }


  int32_t px = -1, py = -1;

  for (int32_t i = 0 ; i < data->count ; i++)
    {
      //  Check against the rectangular or polygonal area that is displayed.

      if (!check_bounds (options, misc, data, i, misc->slice, NVTrue))
        {
          //  Convert the X, Y, and Z value to a projected pixel position

          map->get2DCoords (data->x[i], data->y[i], -data->z[i], &px, &py, &init2D);


          //  Now check the point against the polygon.

          if (!inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
            {
              //  Save the undo information.

              store_undo (misc, options->undo_levels, data->val[i], i);


              data->val[i] = CHRTR2_NULL;
            }
        }
    }

  free (mx);
  free (my);


  //  Close the undo block.

  end_undo_block (misc);
}
