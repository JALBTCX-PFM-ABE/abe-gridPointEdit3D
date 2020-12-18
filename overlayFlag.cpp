
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



#include "gridPointEdit3DDef.hpp"

/***************************************************************************\
*                                                                           *
*   Module Name:        overlay_flag                                        *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       November 1995                                       *
*                                                                           *
*   Purpose:            Draw flag locations in the GC and the pixmap.       *
*                                                                           *
*   Arguments:          None                                                *
*                                                                           *
*   Return Value:       None                                                *
*                                                                           *
*   Calling Routines:   utility routine                                     *
*                                                                           * 
\***************************************************************************/

void overlayFlag (nvMapGL *map, OPTIONS *options, MISC *misc, POINT_DATA *data)
{
  //  Clear the marker points.

  map->clearMarkerPoints ();


  //  We can only display one group of markers (it gets too confusing to look at otherwise).  These are the data type or CHRTR2_USER flagged points.

  if (options->flag_index)
    {
      //  Turn off highlighted points.

      if (misc->highlight_count)
        {
          misc->highlight.clear ();
          misc->highlight_count = 0;
        }


      uint32_t check = 0;


      switch (options->flag_index)
        {
        case PRE_USER + 0:
          check = CHRTR2_USER_01;
          break;

        case PRE_USER + 1:
          check = CHRTR2_USER_02;
          break;

        case PRE_USER + 2:
          check = CHRTR2_USER_03;
          break;

        case PRE_USER + 3:
          check = CHRTR2_USER_04;
          break;

        case PRE_USER + 4:
          check = CHRTR2_LAND_MASK;
          break;
        }

      for (int32_t i = 0 ; i < data->count ; i++)
        {
          if (!check_bounds (options, misc, data, i, misc->slice, NVTrue))
            {
              if (data->val[i] & check) map->setMarkerPoints (data->x[i], data->y[i], -data->z[i], options->edit_color, 0.01, NVFalse);
            }
        }
      map->setMarkerPoints (0.0, 0.0, 0.0, Qt::black, 0, NVTrue);
    }


  //  These are highlight points

  else
    {
      for (int32_t i = 0 ; i < misc->highlight_count ; i++)
        {
          int32_t j = misc->highlight[i];

          if (!check_bounds (options, misc, data, j, misc->slice, NVTrue))
            map->setMarkerPoints (data->x[j], data->y[j], -data->z[j], options->edit_color, 0.01, NVFalse);
        }
      map->setMarkerPoints (0.0, 0.0, 0.0, Qt::black, 0, NVTrue);
    }
}
