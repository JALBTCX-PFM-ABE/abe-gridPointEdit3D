
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
*   Module Name:        set_area                                            *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       November 1995                                       *
*                                                                           *
*   Purpose:            Invalidates data in a rectangle or polygon.         *
*                                                                           *
*   Arguments:          x_bounds        -   array of x vertices (in pixels) *
*                       y_bounds        -   array of y vertices (in pixels) *
*                       num_vertices    -   number of vertices              *
*                       rect_flag       -   1 = rectangle, 0 = polygon      *
*                                                                           *
*   Return Value:       None                                                *
*                                                                           *
*   Calling Routines:   ButtonReleaseEH                                     *
*                                                                           * 
\***************************************************************************/

void set_area (nvMapGL *map, OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t *x_bounds, int32_t *y_bounds, int32_t num_vertices, int32_t rect_flag)
{
  double *mx = NULL, *my = NULL;
  uint8_t undo_on = NVFalse;
  uint8_t init2D = NVTrue;


  //  If it's a polygon we have to convert to double so that the "inside" function will work.

  if (!rect_flag)
    {
      mx = (double *) malloc (num_vertices * sizeof (double));

      if (mx == NULL)
        {
          perror ("Allocating mx array in set_area");
          exit (-1);
        }

      my = (double *) malloc (num_vertices * sizeof (double));

      if (my == NULL)
        {
          perror ("Allocating my array in set_area");
          exit (-1);
        }

      for (int32_t i = 0 ; i < num_vertices ; i++)
        {
          mx[i] = (double) x_bounds[i];
          my[i] = (double) y_bounds[i];
        }
    }


  //  Get the minimum bounding rectangle (X and Y in pixels).

  int32_t min_y = 9999999;
  int32_t max_y = -1;
  int32_t min_x = 9999999;
  int32_t max_x = -1;
  for (int32_t j = 0 ; j < num_vertices ; j++)
    {
      min_y = qMin (min_y, y_bounds[j]);
      max_y = qMax (max_y, y_bounds[j]);
      min_x = qMin (min_x, x_bounds[j]);
      max_x = qMax (max_x, x_bounds[j]);
    }


  int32_t px = -1, py = -1;


  //  CLEAR_POLYGON is a special case because we have to search the highlight list instead of all of the points.

  if (options->function == CLEAR_POLYGON)
    {
      for (int32_t i = 0 ; i < misc->highlight_count ; i++)
        {
          int32_t j = misc->highlight[i];


          //  Convert the X, Y, and Z value to a projected pixel position

          map->get2DCoords (data->x[j], data->y[j], -data->z[j], &px, &py, &init2D);


          //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
          //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

          if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
            {
              //  Mark the points that fall inside the polygon.

              if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py)) misc->highlight[i] = -1;
            }
        }

      free (mx);
      free (my);


      //  Now reorder and pack the highlight array.

      int32_t new_count = 0;
      for (int32_t i = 0 ; i < misc->highlight_count ; i++)
        {
          if (misc->highlight[i] >= 0)
            {
              misc->highlight[new_count] = misc->highlight[i];
              new_count++;
            }
        }

        misc->highlight_count = new_count;

        try
          {
            misc->highlight.resize (misc->highlight_count);
          }
        catch (std::bad_alloc&)
          {
            perror ("Allocating misc->highlight in set_area.cpp");
            exit (-1);
          }

        return;
    }


  //  Whether we're making an undo block.

  switch (options->function)
    {
    case DELETE_RECTANGLE:
    case DELETE_POLYGON:
      undo_on = NVTrue;
      break;

    default:
      undo_on = NVFalse;
    }


  for (int32_t i = 0 ; i < data->count ; i++)
    {
      //  Check against the displayed minimum bounding rectangle.  This is all that is needed for 
      //  a rectangle and it cuts down on the computations for a polygon.

      if (!check_bounds (options, misc, data, i, misc->slice, NVTrue))
        {

          //  Convert the X, Y, and Z value to a projected pixel position

          map->get2DCoords (data->x[i], data->y[i], -data->z[i], &px, &py, &init2D);


          //  Rectangle.

          if (rect_flag)
            {
              if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                {
                  switch (options->function)
                    {
                    case DELETE_RECTANGLE:
                      store_undo (misc, options->undo_levels, data->val[i], i);
                      data->val[i] = CHRTR2_NULL;
                      break;

                    case MASK_INSIDE_RECTANGLE:
                      data->mask[i] = NVTrue;
                      break;
                    }
                }
              else
                {
                  switch (options->function)
                    {
                    case MASK_OUTSIDE_RECTANGLE:
                      data->mask[i] = NVTrue;
                      break;
                    }
                }
            }

          //  Polygon.
                    
          else
            {
              if (options->function == MASK_OUTSIDE_POLYGON)
                {
                  //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
                  //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

                  if (px < min_x && px > max_x && py < min_y && py > max_y)
                    {
                      //  If it's outside the MBR it's automatically outside the polygon so we set the flag.

                      data->mask[i] = NVTrue;
                    }
                  else
                    {
                      //  If it's inside the MBR we have to check to make sure it's outside the polygon.

                      if (!inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                        {
                          data->mask[i] = NVTrue;
                        }
                    }
                }
              else
                {
                  //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
                  //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

                  if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                    {
                      if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                        {
                          switch (options->function)
                            {
                            case DELETE_POLYGON:
                              store_undo (misc, options->undo_levels, data->val[i], i);
                              data->val[i] = CHRTR2_NULL;
                              break;

                            case MASK_INSIDE_POLYGON:
                              data->mask[i] = NVTrue;
                              break;

                            case HIGHLIGHT_POLYGON:
                              try
                                {
                                  misc->highlight.resize (misc->highlight_count + 1);
                                }
                              catch (std::bad_alloc&)
                                {
                                  perror ("Allocating misc->highlight in set_area.cpp");
                                  exit (-1);
                                }

                              misc->highlight[misc->highlight_count] = i;
                              misc->highlight_count++;
                              break;
                            }
                        }
                    }
                }
            }
        }
    }

  if (!rect_flag)
    {
      free (mx);
      free (my);
    }


  //  Close the undo block

  if (undo_on) end_undo_block (misc);
}
