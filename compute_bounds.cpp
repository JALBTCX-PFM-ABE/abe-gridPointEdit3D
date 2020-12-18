
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

uint8_t compute_bounds (OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t *viewable_count, float *attr_min, float *attr_max)
{
  static NV_F64_XYMBC prev_bounds = {-999.0, -999.0, -999.0, -999.0, CHRTRNULL, -CHRTRNULL};


  //  Compute the Z range of the data.

  *viewable_count = 0;
  *attr_min = CHRTRNULL;
  *attr_max = -CHRTRNULL;


  //  Compute the min and max values for defining the 3D space and optionally for coloring.

  for (int32_t i = 0 ; i < data->count ;  i++)
    {
      //  Note that we NEVER check for slicing here because we want the range to be the full range when we are slicing.
      //  This option to check_bounds -------------\/   turns off slice check

      if (!check_bounds (options, misc, data, i, NVFalse, NVFalse))
        {
          (*viewable_count)++;


          misc->bounds.min_x = qMin (misc->bounds.min_x, data->x[i]);
          misc->bounds.max_x = qMax (misc->bounds.max_x, data->x[i]);

          misc->bounds.min_y = qMin (misc->bounds.min_y, data->y[i]);
          misc->bounds.max_y = qMax (misc->bounds.max_y, data->y[i]);


          //  Don't use null data for max and min Z computation.

          if (data->val[i])
            {
              misc->bounds.min_z = qMin (misc->bounds.min_z, (double) -data->z[i]);
              misc->bounds.max_z = qMax (misc->bounds.max_z, (double) -data->z[i]);
            }



          //  Compute the attribute min/max if we are coloring by horizontal error, vertical error, or attribute.

          if (options->color_index == 2)
            {
              if (data->herr[i] != 0.0)
                {
                  *attr_min = qMin (data->herr[i], *attr_min);
                  *attr_max = qMax (data->herr[i], *attr_max);
                }
            }
          else if (options->color_index == 3)
            {
              if (data->verr[i] != 0.0)
                {
                  *attr_min = qMin (data->verr[i], *attr_min);
                  *attr_max = qMax (data->verr[i], *attr_max);
                }
            }
        }
    }


  if (misc->bounds.min_x != prev_bounds.min_x || misc->bounds.min_y != prev_bounds.min_y || misc->bounds.min_z != prev_bounds.min_z ||
      misc->bounds.max_x != prev_bounds.max_x || misc->bounds.max_y != prev_bounds.max_y || misc->bounds.max_z != prev_bounds.max_z)
    {
      prev_bounds = misc->aspect_bounds = misc->bounds;


      //  Assuming that the bins are approximately square in terms of distance we want to try to maintain aspect ratio
      //  when we edit a long narrow area.

      double x_bins = (misc->aspect_bounds.max_x - misc->aspect_bounds.min_x) / misc->chrtr2_share->chrtr2_header[0].lon_grid_size_degrees;
      double y_bins = (misc->aspect_bounds.max_y - misc->aspect_bounds.min_y) / misc->chrtr2_share->chrtr2_header[0].lat_grid_size_degrees;

      if (x_bins > y_bins)
        {
          double center = misc->aspect_bounds.min_y + (misc->aspect_bounds.max_y - misc->aspect_bounds.min_y) / 2.0;

          misc->aspect_bounds.min_y = center - (x_bins / 2.0) * misc->chrtr2_share->chrtr2_header[0].lat_grid_size_degrees;
          misc->aspect_bounds.max_y = center + (x_bins / 2.0) * misc->chrtr2_share->chrtr2_header[0].lat_grid_size_degrees;
        }
      else
        {
          double center = misc->aspect_bounds.min_x + (misc->aspect_bounds.max_x - misc->aspect_bounds.min_x) / 2.0;

          misc->aspect_bounds.min_x = center - (y_bins / 2.0) * misc->chrtr2_share->chrtr2_header[0].lon_grid_size_degrees;
          misc->aspect_bounds.max_x = center + (y_bins / 2.0) * misc->chrtr2_share->chrtr2_header[0].lon_grid_size_degrees;
        }

      return (NVTrue);
    }

  return (NVFalse);
}
