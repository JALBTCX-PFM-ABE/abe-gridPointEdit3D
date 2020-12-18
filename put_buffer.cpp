
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
*   Module Name:        put_buffer                                          *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       May 1999                                            *
*                                                                           *
*   Purpose:            Write the modified portion of the rectangle back to *
*                       the file and recompute the bin sets.                *
*                                                                           *
*   Inputs:             None                                                *
*                                                                           *
*   Return Value:       0 if nothing modified, else 1                       *
*                                                                           *
*   Calling Routines:   ExitCB                                              *
*                                                                           * 
\***************************************************************************/

int32_t put_buffer (POINT_DATA *data, MISC *misc)
{
  NV_I32_COORD2   coord;
  int32_t         mod_flag;
  CHRTR2_RECORD   chrtr2_record;



  mod_flag = 0;


  misc->statusProg->setRange (0, data->count);
  misc->statusProgLabel->setText (gridPointEdit3D::tr ("Saving edits"));
  misc->statusProgLabel->setVisible (true);
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      QString progText = gridPointEdit3D::tr ("Saving edits for ") + QFileInfo (QString (misc->chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2");

      misc->statusProgLabel->setText (progText);
      misc->statusProg->reset ();


      //  This seems a bit silly but it takes a long time to spin through the data
      //  with a QProgressDialog running so we're only updating it at 10% intervals.

      int32_t inc = data->count / 10;
      if (!inc) inc = 1;


      for (int32_t i = 0 ; i < data->count ; i++)
        {
          if (!(i % inc))
            {
              misc->statusProg->setValue (i);
              qApp->processEvents();
            }


          //  Only deal with points that are in the current CHRTR2 (i.e. misc->chrtr2_share->open_args[chrtr2])

          if (data->chrtr2[i] == chrtr2)
            {
              //  We only need to check for data points that have been set to NULL since that's the only possible change.

              if (!data->val[i])
                {
                  //  Set the coordinate x and y value

                  coord.x = data->xcoord[i];
                  coord.y = data->ycoord[i];


                  memset (&chrtr2_record, 0, sizeof (CHRTR2_RECORD));

                  chrtr2_write_record (misc->chrtr2_handle[chrtr2], coord, chrtr2_record);

                  mod_flag++;
                }
            }
        }

      misc->statusProg->reset ();
      misc->statusProgLabel->setVisible (false);
      misc->statusProg->setTextVisible (false);
      qApp->processEvents();
    }


  data->x.clear ();
  data->y.clear ();
  data->z.clear ();
  data->herr.clear ();
  data->verr.clear ();
  data->val.clear ();
  data->chrtr2.clear ();
  data->xcoord.clear ();
  data->ycoord.clear ();
  data->mask.clear ();

  return (mod_flag);
}
