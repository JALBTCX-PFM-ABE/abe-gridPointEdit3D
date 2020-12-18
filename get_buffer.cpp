
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
*   Module Name:        get_buffer                                          *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       May 1999                                            *
*                                                                           *
*   Purpose:            Get the rectangle of data from the file.            *
*                                                                           *
*   Return Value:       None                                                *
*                                                                           *
\***************************************************************************/

void get_buffer (POINT_DATA *data, MISC *misc)
{
  NV_I32_COORD2   coord;
  int32_t         total;
  NV_F64_COORD2   xy;
  CHRTR2_RECORD   chrtr2_record;


  //  Clear some important variables

  total = 0;
  data->count = 0;

  misc->min_z = 99999999.0;
  misc->max_z = -99999999.0;



  //  Compute average bin size for all of the CHRTR2 files.

  misc->x_grid_size = 0.0;
  misc->y_grid_size = 0.0;

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      misc->x_grid_size += misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
      misc->y_grid_size += misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
    }

  misc->x_grid_size /= (double) misc->chrtr2_share->chrtr2_count;
  misc->y_grid_size /= (double) misc->chrtr2_share->chrtr2_count;


  //  Build a lookup table for the file types and get the unique data types.

  for (int32_t chrtr2 = misc->chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      //  Read the buffer.

      xy.y = misc->chrtr2_share->edit_area.min_y;
      xy.x = misc->chrtr2_share->edit_area.min_x;


      misc->ll[chrtr2].x = (int32_t) ((xy.x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                                       misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
      misc->ll[chrtr2].y = (int32_t) ((xy.y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                                       misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);

      if (misc->ll[chrtr2].x < 0) misc->ll[chrtr2].x = 0;
      if (misc->ll[chrtr2].y < 0) misc->ll[chrtr2].y = 0;


      xy.y = misc->chrtr2_share->edit_area.max_y;
      xy.x = misc->chrtr2_share->edit_area.max_x;

      misc->ur[chrtr2].x = (int32_t) ((xy.x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                                       misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
      misc->ur[chrtr2].y = (int32_t) ((xy.y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                                       misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);

      if (misc->ur[chrtr2].x > misc->chrtr2_share->chrtr2_header[chrtr2].width - 1) misc->ur[chrtr2].x = misc->chrtr2_share->chrtr2_header[chrtr2].width - 1;
      if (misc->ur[chrtr2].y > misc->chrtr2_share->chrtr2_header[chrtr2].height - 1) misc->ur[chrtr2].y = misc->chrtr2_share->chrtr2_header[chrtr2].height - 1;


      for (int32_t i = misc->ll[chrtr2].y ; i <= misc->ur[chrtr2].y ; i++)
        {
          coord.y = i;
          for (int32_t j = misc->ll[chrtr2].x ; j <= misc->ur[chrtr2].x ; j++)
            {
              coord.x = j;

              chrtr2_read_record (misc->chrtr2_handle[chrtr2], coord, &chrtr2_record);

              //  Don't use NULL points.

              if (chrtr2_record.status)
                {
                  //  If the polygon count is zero we are loading a rectangle so we want to check against that.
                  //  Otherwise it's a polygon so we need to check against that.

                  uint8_t in = NVFalse;
                  if (misc->chrtr2_share->polygon_count)
                    {
                      chrtr2_get_lat_lon (misc->chrtr2_handle[chrtr2], &xy.y, &xy.x, coord);


                      if (inside_polygon2 (misc->chrtr2_share->polygon_x, misc->chrtr2_share->polygon_y, misc->chrtr2_share->polygon_count, xy.x, xy.y)) 
                        in = NVTrue;
                    }
                  else
                    {
                      if (xy.x >= misc->chrtr2_share->edit_area.min_x && xy.x <= misc->chrtr2_share->edit_area.max_x &&
                          xy.y >= misc->chrtr2_share->edit_area.min_y && xy.y <= misc->chrtr2_share->edit_area.max_y) in = NVTrue;
                    }


                  if (in)
                    {
                      total ++;

                      try
                        {
                          data->x.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.x, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->y.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.y, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->z.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.z, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->herr.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.herr, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->verr.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.verr, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->val.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.val, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->chrtr2.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.chrtr2, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->xcoord.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.xcoord, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->ycoord.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.ycoord, exiting\n");
                          exit (-1);
                        }

                      try
                        {
                          data->mask.resize (total);
                        }
                      catch (std::bad_alloc&)
                        {
                          fprintf (stderr, "Error allocating memory for data.mask, exiting\n");
                          exit (-1);
                        }


                      double lat, lon;
                      chrtr2_get_lat_lon (misc->chrtr2_handle[chrtr2], &lat, &lon, coord);
                      data->x[data->count] = lon;
                      data->y[data->count] = lat;
                      data->z[data->count] = chrtr2_record.z;


                      //  Compute min and max Z values for displayed data (valid or not).

                      misc->min_z = qMin (data->z[data->count], misc->min_z);
                      misc->max_z = qMax (data->z[data->count], misc->max_z);


                      data->herr[data->count] = chrtr2_record.horizontal_uncertainty;
                      data->verr[data->count] = chrtr2_record.vertical_uncertainty;
                      data->val[data->count] = chrtr2_record.status;
                      data->chrtr2[data->count] = chrtr2;
                      data->xcoord[data->count] = coord.x;
                      data->ycoord[data->count] = coord.y;
                      data->mask[data->count] = NVFalse;


                      data->count++;
                    }
                }
            }
        }
    }


  //  Define the bounds + 2%

  double width_2 = (misc->chrtr2_share->edit_area.max_x - misc->chrtr2_share->edit_area.min_x) * 0.02;
  double height_2 = (misc->chrtr2_share->edit_area.max_y - misc->chrtr2_share->edit_area.min_y) * 0.02;
  misc->chrtr2_share->edit_area.min_y -= height_2;
  misc->chrtr2_share->edit_area.max_y += height_2;
  misc->chrtr2_share->edit_area.min_x -= width_2;
  misc->chrtr2_share->edit_area.max_x += width_2;
    
    
  misc->displayed_area.min_x = misc->chrtr2_share->edit_area.min_x;
  misc->displayed_area.max_x = misc->chrtr2_share->edit_area.max_x;
  misc->displayed_area.min_y = misc->chrtr2_share->edit_area.min_y;
  misc->displayed_area.max_y = misc->chrtr2_share->edit_area.max_y;
}
