
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




#ifndef VERSION

#define     VERSION     "PFM Software - gridPointEdit3D V3.13 - 09/02/16"

#endif

/*! <pre>

    Version 1.00
    Jan C. Depner
    08/09/10

    First working version.


    Version 1.01
    Jan C. Depner
    09/08/10

    Changes to handle Qt 4.6.3.


    Version 2.00
    Jan C. Depner
    11/05/10

    Finally fixed auto Z scaling in nvMapGL (nvutility library).  Also added minimum Z extents to preferences.


    Version 2.01
    Jan C. Depner
    11/22/10

    Added ability to toggle display of interpolated data (since it makes no sense to edit interpolated data).


    Version 2.02
    Jan C. Depner
    12/08/10

    Cleaned up button hotkey handling.


    Version 2.03
    Jan C. Depner
    12/09/10

    Fixed the slice and size bar buttons (finally).


    Version 2.04
    Jan C. Depner
    12/14/10

    We can finally mark data points in a somewhat proper way.  They don't disappear when you rotate or zoom.


    Version 2.05
    Jan C. Depner
    12/16/10

    Can now highlight and un-highlight in polygons.  Also, pressing the Enter key when data is highlighted will
    invalidate the highlighted points.


    Version 2.06
    Jan C. Depner
    12/22/10

    Optional contours in the 2D tracker map.  Maintain aspect ratio when displaying data.


    Version 2.07
    Jan C. Depner
    01/05/11

    Added hotkey check in the preferences dialog to prevent conflicts.  I really should have done this a long time
    ago but it was way down on the priority list ;-)


    Version 2.08
    Jan C. Depner
    01/06/11

    Correct problem with the way I was instantiating the main widget.  This caused an intermittent error on Windows7/XP.


    Version 2.09
    Jan C. Depner
    01/07/11

    Added the cursor distance, azimuth, and delta Z to the measure tool output (in the status bar).


    Version 2.10
    Jan C. Depner
    01/15/11

    Added an exaggeration scrollbar to the left side of the window.


    Version 2.11
    Jan C. Depner
    01/16/11

    Positioned non-modal popup dialogs in the center of the screen.


    Version 2.12
    Jan C. Depner
    01/24/11

    First shot at replacing CHRTR2_USER_05 with CHRTR2_LAND_MASK.


    Version 2.13
    Jan C. Depner
    03/28/11

    Try to save the user's hot key definitions even across setting version changes.


    Version 2.14
    Jan C. Depner
    11/30/11

    Converted .xpm icons to .png icons.


    Version 2.15
    Jan C. Depner
    02/03/12

    Now displays "locked" layer data in refernce color.  Won't perform edit operations on the locked data.


    Version 3.0
    Stacy Johnson
    06/15/2012

    Removed half node shift becuase we are moving chrtr2 to grid registration    


    Version 3.01
    Stacy Johnson
    08/16/2012

    Continued chrtr2 refinments


    Version 3.02
    Jan C. Depner (PFM Software)
    12/09/13

    Switched to using .ini file in $HOME (Linux) or $USERPROFILE (Windows) in the ABE.config directory.  Now
    the applications qsettings will not end up in unknown places like ~/.config/navo.navy.mil/blah_blah_blah on
    Linux or, in the registry (shudder) on Windows.


    Version 3.04
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 3.05
    Jan C. Depner (PFM Software)
    03/19/14

    - Straightened up the Open Source acknowledgments.


    Version 3.06
    Jan C. Depner (PFM Software)
    05/27/14

    - Added the new LGPL licensed GSF library to the acknowledgments.


    Version 3.07
    Jan C. Depner (PFM Software)
    06/25/14

    - Replaced all printf statements with fprintf (stdout, and then added fflush (stdout) after them.


    Version 3.08
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).


    Version 3.09
    Jan C. Depner (PFM Software)
    12/09/14

    - Added minimum bounding rectangle checks to set_area in an attempt to speed up point inside polygon checking.


    Version 3.10
    Jan C. Depner (PFM Software)
    12/24/14

    - Replaced realloc with std::vector to address Windows speed issues.


    Version 3.11
    Jan C. Depner (PFM Software)
    12/28/14

    - Taking advantage of a change to get2DCoords in nvMapGL.cpp in the nvutility library.  Basically, it no longer makes calls to
      get the viewport, mvmatrix, and projmatrix except the first time it's called for the loop of points.  This really shows up
      in Windows but it actually makes the Linux version faster as well.


    Version 3.12
    Jan C. Depner (PFM Software)
    08/09/16

    - Now gets font from globalABE.ini as set in Preferences by pfmView.
    - To avoid possible confusion, removed translation setup from QSettings in env_in_out.cpp.


    Version 3.13
    Jan C. Depner (PFM Software)
    09/02/16

    - Added contours to the 3D map, either at the top of the map or in the point cloud.

</pre>*/
