
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

int32_t pseudo_dist_from_viewer (MISC *misc, double x, double y);

/***************************************************************************\
*                                                                           *
*   Module Name:        check_bounds                                        *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       July 2000 (On vacation in Disney World, no less)    *
*                                                                           *
*   Purpose:            Check all of the conditions for displaying a beam.  *
*                                                                           *
*   Arguments:          slice    -   NVFalse - no slice check               *
*                       lock     -   NVFalse - no lock check                *
*                                                                           *
*   Return Value:       2 - don't display, 1 - display translucent,         *
*                       0 - display in full color                           *
*                                                                           *
\***************************************************************************/

int32_t check_bounds (OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t index, uint8_t slice, uint8_t lock)
{
  double           x = data->x[index];
  double           y = data->y[index];
  uint32_t         v = data->val[index];
  int16_t          chrtr2 = data->chrtr2[index];
  uint8_t          m = data->mask[index];

  return (check_bounds (options, misc, x, y, v, m, chrtr2, slice, lock));
}
  


int32_t check_bounds (OPTIONS *options, MISC *misc, double x, double y, uint32_t v, uint8_t mask, int16_t chrtr2, uint8_t slice, uint8_t lock)
{
  int32_t          iz;


  //  Easy checks first, that way, if we can eliminate this point we don't have to do 
  //  the edge or inside check.  Check for NULL.

  if (!v) return (2);


  //  If this point is masked...

  if (mask) return (2);


  //  If we're not displaying the CHRTR2 that this point is in.

  if (!misc->chrtr2_share->display_chrtr2[chrtr2]) return (2);


  //  If we don't want to get data from locked layers.

  if (lock && misc->chrtr2_share->lock_chrtr2[chrtr2]) return (2);


  //  If we're not displaying interpolated and this point is interpolated.

  if (!options->display_interpolated && (v & CHRTR2_INTERPOLATED)) return (2);


  //  Now check the bounds.  Simple MBR first, then polygon if needed.

  if (x < misc->displayed_area.min_x || x > misc->displayed_area.max_x || y < misc->displayed_area.min_y || y > misc->displayed_area.max_y) return (2);


  if (misc->poly_count && !inside_polygon2 (misc->polygon_x, misc->polygon_y, misc->poly_count, x, y)) return (2);


  //  Check for slicing if it is enabled.  Semi-transparent values return a 1.

  if (slice)
    {
      iz = pseudo_dist_from_viewer (misc, x, y);
      if (iz < misc->slice_min || iz > misc->slice_max) return (1);
    }


  return (0);
}
