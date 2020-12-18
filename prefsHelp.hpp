
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



QString bGrpText = 
  Prefs::tr ("Select the format in which you want all geographic positions to be displayed.");

QString contourIntText = 
  Prefs::tr ("Set the contour interval to be used by pfmView and also pfmEdit3D.  Setting this "
             "to 0.0 will cause the user defined contour levels to be used.  No contours will "
             "be plotted unless the plot contour button <img source=\":/icons/contour.png\"> has been "
             "toggled on.");

QString contourSmText = 
  Prefs::tr ("Set the contour smoothing factor by pfmView and also pfmEdit3D.  This value goes "
             "from 0 (no smoothing) to 10 (max smoothing).");

QString locBoxText = 
  Prefs::tr ("Select where you would like contours to be displayed.  Checking <b>Top</b> will cause the contours to be displayed at the "
             "top of the 3D display space.  Checking <b>Point cloud</b> will cause the contours to be drawn within the point cloud.");

QString topCheckText = 
  Prefs::tr ("Checking this box will cause contours to be displayed at the top of the 3D display space.");

QString cloudCheckText = 
  Prefs::tr ("Checking this box will cause contours to be drawn within the point cloud.");

QString ZFactorText = 
  Prefs::tr ("Set the scaling factor for contours <b><i>and</i></b> all displayed depth values "
             "in gridEdit and gridPointEdit3D.  The actual values aren't changed just the displayed values.  "
             "This is useful for comparing depths to GeoTIFFs of scanned charts from NOAA (feet) "
             "or the US Navy (fathoms).");

QString offsetText = 
  Prefs::tr ("Set the offset value for contours <b><i>and</i></b> all displayed depth values "
             "in gridEdit and gridPointEdit3D.  The actual values aren't changed just the displayed values.  "
             "This is useful for applying estimated ellipsoid offsets prior to the final datum "
             "shift being applied to the data.<br><br>"
             "<b>IMPORTANT NOTE: The offset value is <i>added</i> to the Z value and is applied <i>after</i> "
             "scaling so the units are in scaled units.</b>");

QString WidthText = 
  Prefs::tr ("Set the contour line width/thickness in pixels.  Index contours (if set) will be twice this size.");

QString pointSizeText = 
  Prefs::tr ("Set the size of each displayed point in pixels.");

QString sparseText = 
  Prefs::tr ("Set the point limit at which we will plot sparse points for rotation, zoom, and slice.  When we have more than "
             "this number of points loaded the program will only display the min and max points from each CHRTR2 bin when "
             "the image is rotated or zoomed.  In addition, when slicing, areas outside of the active slice will only display "
             "the sparse points.  The default is 500,000.");

QString scaleText = 
  Prefs::tr ("Check this box to draw the scale on the screen.");

QString iconSizeText = 
  Prefs::tr ("Set the size of the main window buttons.  The options are 16, 20, 24, 28, or 32 pixels.<br><br>"
             "<b>IMPORTANT NOTE: The icons size will change when you click the <i>Apply</i> button but "
             "the locations will not be entirely correct until you have exited and restarted the program.</b>");

QString rotText = 
  Prefs::tr ("Set the view rotation increment in degrees.  This value is the amount the alternate views will rotate "
             "when the left or right arrow keys are pressed.");

QString zoomPercentText = 
  Prefs::tr ("Set the percentage value for zooming the data view in or out.");

QString exagText = 
  Prefs::tr ("Set the Z exaggeration value (1.0 - 100.0).  This can be modified more easily by pressing <b>Ctrl-Page UP</b> "
             "or <b>Ctrl-Page Down</b> when viewing the main window.");


QString prefsUndoText = 
  Prefs::tr ("Set the maximum number of undo levels.  This is the number of edit operations that can be <b><i>undone</i></b>. "
             "The value ranges from a reasonable 100 to a ridiculous 1,000,000.  The higher this value is set, the more memory "
             "you use to save the undo information.");

QString contourLevelsText = 
  Prefs::tr ("Set individual contour levels.  Pressing this button will set the standard contour "
             "interval to 0.0 and bring up a table dialog to allow the user to insert contour "
             "levels (one per line, ascending).  The default levels are IHO standard contour "
             "levels.");

QString contoursTableText = 
  Prefs::tr ("Use this table to set individual contour levels.  Levels should be set in ascending "
             "order.");

QString hotKeysText = 
  Prefs::tr ("Click this button to change the accelerator key sequences associated with some of "
             "the function buttons.");

QString hotKeyTableText = 
  Prefs::tr ("Accelerator key sequences (hot keys) for some of the buttons in the main window may be changed "
             "in this dialog.  Up to four comma separated key values with modifiers may be used.  The format is "
             "illustrated by the following examples:<br><br>"
             "<ul>"
             "<li>q</li>"
             "<li>Shift+q</li>"
             "<li>Ctrl+q</li>"
             "<li>Meta+q</li>"
             "<li>Ctrl+Alt+q</li>");

QString closeHotKeyText = 
  Prefs::tr ("Click this button to close the hot key dialog.");

QString closeContoursText = 
  Prefs::tr ("This button closes the contour level dialog.");

QString colorText = 
  Prefs::tr ("Set color options in this tab.");

QString conText = 
  Prefs::tr ("Set depth display options in this tab");

QString kboxText = 
  Prefs::tr ("Set the hot keys in this tab.");

QString miscText = 
  Prefs::tr ("This tab contains settings for a number of miscellaneous options including the number of undo levels "
             "and the rotation increment.");

QString markerColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot the point marker.  "
             "After selecting a color the map will be redrawn.");

QString contourColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot contours.  "
             "After selecting a color the map will be redrawn.");

QString trackerColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot the <i>other CHRTR2 window</i> tracker.  "
             "After selecting a color the map will be redrawn.");

QString backgroundColorText = 
  Prefs::tr ("Click this button to change the color that is used as background.  After "
             "selecting a color the map will be redrawn.");

QString scaleColorText = 
  Prefs::tr ("Click this button to change the color that is used for the scale.  After "
             "selecting a color the map will be redrawn.");

QString restoreDefaultsText = 
  Prefs::tr ("Click this button to restore all of the preferences to the original default values.  "
             "This includes <b><i>all</i></b> of the system preferences, not just those in the "
             "preferences dialog.");

QString closePrefsText = 
  Prefs::tr ("Click this button to discard all preference changes and close the dialog.  The main map and "
             "coverage map will not be redrawn.");

QString applyPrefsText = 
  Prefs::tr ("Click this button to apply all changes and close the dialog.  The main map and coverage map will "
             "be redrawn.");
