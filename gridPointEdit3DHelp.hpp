
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



QString exitSaveText = 
  gridPointEdit3D::tr ("<img source=\":/icons/exit_save.png\"> Click this button to save changes to the CHRTR2 file and "
                       "then exit from the editor.");
QString exitMaskText = 
  gridPointEdit3D::tr ("<img source=\":/icons/exit_mask.png\"> Click this button to save changes to the CHRTR2 file and "
                       "then exit from the editor.  In addition, the area that was edited will be marked as a filter "
                       "masked area in gridEdit.  This is handy just to keep track of areas that you have edited in the overall "
                       "CHRTR2 file.");
QString exitNoSaveText = 
  gridPointEdit3D::tr ("<img source=\":/icons/exit_no_save.png\"> Click this button to discard changes and then exit from "
                       "the editor.");

QString resetText = 
  gridPointEdit3D::tr ("<img source=\":/icons/reset_view.png\"> Click this button to reset to the original scale and view.");


QString interpolatedText = 
  gridPointEdit3D::tr ("<img source=\":/icons/display_interpolated.png\"> Click this button to toggle display of the interpolated "
                       "data points.  Editing the interpolated data points doesn't make much sense usually.");

QString highlightPolyText = 
  gridPointEdit3D::tr ("<img source=\":/icons/highlight_polygon.png\"> Click this button to allow you to select a polygonal area "
                       "inside of which you wish to highlight data points.  When selected the cursor will become the highlight polygon cursor "
                       "<img source=\":/icons/highlight_polygon_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                       "highlight the data in the polygon simply left click again.  If, at any time during the operation, "
                       "you wish to discard the polygon and abort the operation simply click the middle mouse "
                       "button or right click and select a menu option.<br><br>"
                       "<b>IMPORTANT NOTE: If you would like to delete highlighted data points just press <i>Enter</i>.</b>");
QString clearPolyText = 
  gridPointEdit3D::tr ("<img source=\":/icons/clear_polygon.png\"> Click this button to allow you to select a polygonal area "
                       "inside of which you wish to un-highlight data points.  When selected the cursor will become the clear polygon cursor "
                       "<img source=\":/icons/clear_polygon_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                       "un-highlight the data in the polygon simply left click again.  If, at any time during the operation, "
                       "you wish to discard the polygon and abort the operation simply click the middle mouse "
                       "button or right click and select a menu option.<br><br>"
                       "<b>IMPORTANT NOTE: If you would like to delete highlighted data points just press <i>Enter</i>.</b>");
QString clearHighlightText = 
  gridPointEdit3D::tr ("<img source=\":/icons/clear_highlight.png\"> Click this button to clear all highlighted points from "
                       "the display.");

QString contourText = 
  gridPointEdit3D::tr ("<img source=\":/icons/contour.png\"> Click this button to toggle drawing of contours.");


QString prefsText = 
  gridPointEdit3D::tr ("<img source=\":/icons/prefs.png\"> Click this button to change program preferences.  This "
                       "includes colors, slice size, position display format, and other options.");


QString stopText = 
  gridPointEdit3D::tr ("<img source=\":/icons/stop.png\"> Click this button to cancel drawing of the data.  A much easier "
                       "way to do this though is to click any mouse button in the display area or press any "
                       "key on the keyboard.  The stop button is really just there so that the interface looks "
                       "similar to the viewer and also to provide a place for help on how to stop the drawing.");


QString deletePointText = 
  gridPointEdit3D::tr ("<img source=\":/icons/delete_point.png\"> Click this button to select delete point "
                       "mode.  In this mode you can place the cursor on a point and delete it.  Press the left mouse button "
                       "to delete the point.  The display will be redrawn after a delete.  Information "
                       "about the current point will be displayed in the status bars at the bottom of the "
                       "window.<br><br>"
                       "You can also highlight a point by using the right-click popup menu.  Note that the <b>Highlight point</b> option may be "
                       "disabled if you have filter kill points up or are flagging data using any of the flag buttons "
                       "<img source=\":/icons/user_flag.png\">");
QString deleteRectText = 
  gridPointEdit3D::tr ("<img source=\":/icons/delete_rect.png\"> Click this button to allow you to select a rectangular area "
                       "to invalidate.  When selected the cursor will become the invalidate rectangle cursor "
                       "<img source=\":/icons/delete_rect_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize the "
                       "rectangle and invalidate the data left click again.  If, at any time during the operation, you wish to "
                       "discard the rectangle and abort the operation simply click the middle mouse button or right click and "
                       "select one of the menu options.");
QString deletePolyText = 
  gridPointEdit3D::tr ("<img source=\":/icons/delete_poly.png\"> Click this button to allow you to select a polygonal area "
                       "to invalidate.  When selected the cursor will become the invalidate polygon cursor "
                       "<img source=\":/icons/delete_poly_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                       "invalidate the data in the polygon simply left click again.  If, at any time during the operation, "
                       "you wish to discard the polygon and abort the operation simply click the middle mouse "
                       "button or right click and select a menu option.");
QString keepPolyText = 
  gridPointEdit3D::tr ("<img source=\":/icons/keep_poly.png\"> Click this button to allow you to select a polygonal area "
                       "and invalidate all data <b>outside</b> of the polygon.  When selected the cursor will "
                       "become the invalidate outside polygon cursor <img source=\":/icons/keep_poly_cursor.png\">.  Left "
                       "clicking on a location will cause that point to be the polygon start point.  To close the polygon and "
                       "invalidate the data outside of the polygon simply left click again.  If, at any time during the "
                       "operation, you wish to discard the polygon and abort the operation simply click the middle mouse "
                       "button or right click and select a menu option.");

QString mapText = 
  gridPointEdit3D::tr ("The gridPointEdit3D program is used to display and edit gridded data points that have been stored "
                       "in a CHRTR2 file.  This program is launched from the gridEdit program and is "
                       "not run from the command line.  The various editing and viewing options are "
                       "initiated from the tool bar buttons.  Point of view is controlled by holding down the <b>Ctrl</b> key "
                       "and clicking and dragging with the left mouse button.  It can also be controlled by holding down the "
                       "<b>Ctrl</b> key and using the left, right, up, and down arrow keys.  Zoom is controlled by holding down "
                       "the <b>Ctrl</b> key and using the mouse wheel or holding down the <b>Ctrl</b> key, pressing the right "
                       "mouse button, and moving the cursor up (zoom in) or down (zoom out).  To reposition the center of the view "
                       "just place the cursor at the desired location, hold down the <b>Ctrl</b> key, and click the middle mouse "
                       "button.  Z exaggeration can be changed by pressing <b>Ctrl-Page Up</b> or <b>Ctrl-Page Down</b> or by editing "
                       "the exaggeration value in the Preferences dialog <img source=\":/icons/prefs.png\"><br><br><br>"
                       "Slicing of the data is initiated by using the mouse wheel, dragging the slider "
                       "in the scroll bar on the right of the display, by clicking the up or down arrow keys (without "
                       "holding down the <b>Ctrl</b> key), or pressing the up and down arrow buttons in the slice scroll bar.  "
                       "<br><br>"
                       "Help is available for each button by clicking on the What's This button <img source=\":/icons/contextHelp.png\"> "
                       "then clicking on the item of interest.");

QString trackMapText = 
  gridPointEdit3D::tr ("This is the 2D tracking map.  It will show you where your cursor is in relation to the 3D cursor position.");

QString exagBarText = 
  gridPointEdit3D::tr ("This scroll bar controls/monitors the vertical exaggeration of the data.  Pressing the up and down "
                       "arrow keys will change the exaggeration by 1.0.  Clicking the mouse while the cursor is in the trough "
                       "will change the value by 5.0.  Dragging the slider will display the value in the <b>Exag:</b> field in "
                       "the status area on the left but the exaggeration will not change until the slider is released.  The "
                       "maximum value is 100.0 and the minimum value is 1.0.  If the exaggeration has been set to less than 1.0 "
                       "due to large vertical data range the scroll bar will be inactive.  The exaggeration can also be changed "
                       "by pressing the <b>Ctrl</b> key and then simultaneously pressing the <b>PageUp</b> or <b>PageDown</b> button.");

QString sliceBarText = 
  gridPointEdit3D::tr ("This scroll bar controls/monitors the slice as it moves through the data.  Pressing the up and down "
                       "arrow keys will move the slice through the data.  The slice size will be the percent of the viewable data "
                       "that is set with the slice size scroll bar.  When you press the up arrow the view will move "
                       "into the data one slice.  When displaying a slice, any edits will only effect data that is "
                       "in full color (non-translucent).  Slicing is extremely handy in dealing with very bumpy surfaces "
                       "to allow you to clean up hidden flyers.  You can turn off slicing by right clicking and selecting "
                       "<b>Turn off slicing</b>, by double clicking anywhere in the window, or by pressing the <b>Esc</b> key.");

QString sizeText = 
  gridPointEdit3D::tr ("Set the slice size using this scroll bar.  To understand how slicing works try "
                       "to visualize the surface as it would be in plan view but with the bottom (leading) edge defined "
                       "by the angle of view.  The slice size will be this percentage (default is 5 percent or 1/20th) of "
                       "the displayed data.  When you press one of the up or down arrow keys or click one of the slice bar "
                       "arrow buttons the slice will move by this amount.  The range for this scroll bar is 1 to 50.");


QString transText = 
  gridPointEdit3D::tr ("Set the transparency value to be used for data that is not in the current slice.  If "
                       "the slider is moved to the bottom, data outside the slice will be invisible.  If it is moved to the top "
                       "the data will be almost completely opaque.  The range for this scroll bar is 0 (transparent) to 64 "
                       "(semi-transparent).");

QString colorScaleBoxText = 
  gridPointEdit3D::tr ("The color scale is an index for the color ranges.  The scale can be turned off in the preferences dialog.<br><br>"
                       "<b>IMPORTANT NOTE: The top and bootom values of the color scale can be locked in the gridEdit program.  "
                       "When this is done these values will be locked here as well.</b>");


QString gridPointEdit3DAboutText = 
  gridPointEdit3D::tr ("<center><b>gridPointEdit</b><br><br>"
                       "Author : Jan C. Depner<br>"
                       "Date : 09 August 2010<br><br><br>"
                       "The History Of CHRTR2<br></center>"
                       "Back in 1994 I realized that we needed a simple replacement for the old CHRTR grid format.  "
                       "The major shortcomings of CHRTR were that it didn't support unequal lat and lon grid sizes "
                       "and it couldn't handle anything smaller than about 2 meter grids (due to 32 bit floating "
                       "point precision).  Instead of just sitting down and writing the damn thing I talked to a "
                       "bunch of other people about it.  At which point I realized the truth of the saying 'the "
                       "only thing with 20 legs, 20 arms, and 10 heads that doesn't have a brain is a committee'.  "
                       "The finalized version of the new grid format was released some time around 2006.  It sits "
                       "on top of HDF5 which is a gigantic container 'format'.  In order to use it you have to map "
                       "in a huge library and use a bunch of arcane function calls.  In it's defense, it can hold "
                       "anything and everything.  That's right, it's the world's largest crescent wrench.  I hate "
                       "crescent wrenches...<br><br>"
                       "The CHRTR2 format was designed in keeping with the KISS principle (Keep It Simple Stupid).  "
                       "It is used to store and retrieve a simple latitude and longitude grid with status and "
                       "optional horizontal and vertical uncertainty.  It has an ASCII header that you can read with "
                       "your own beady little eyes by simply 'more'ing or 'less'ing or, God forbid, 'type /page'ing "
                       "the file.  It is arranged south to north, west to east.  That is, the first row is the  "
                       "southernmost row and the first point in that row is the westernmost point.  The data is "
                       "stored as bit packed, scaled integers so the file has no 'endianness'.  As opposed to "
                       "the 'designed by committee' format described above, CHRTR2 took about two days to write.  "
                       "I should have done this in 1994!<br><br>") + mapText +
  gridPointEdit3D::tr ("<br><br><center>Jan Depner (area.based.editor@gmail.com)<br></center>");

QString miscLabelText = 
  gridPointEdit3D::tr ("This area displays the currently active mode or information about the operation in progress.");

QString statusBarText = 
  gridPointEdit3D::tr ("The status bar is used to display the progress of the current operation.");

QString progStatusText = 
  gridPointEdit3D::tr ("The status bar is used to display the progress of the current operation.");

QString flagText = 
  gridPointEdit3D::tr ("Click this button to access a pulldown menu allowing you to select data flagging options.  The "
                       "flagging options are:<br><br>"
                       "<ul>"
                       "<li><img source=\":/icons/user_flag01.png\"> - mark CHRTR2_USER_01 flagged data</li>"
                       "<li><img source=\":/icons/user_flag02.png\"> - mark CHRTR2_USER_02 flagged data</li>"
                       "<li><img source=\":/icons/user_flag03.png\"> - mark CHRTR2_USER_03 flagged data</li>"
                       "<li><img source=\":/icons/user_flag04.png\"> - mark CHRTR2_USER_04 flagged data</li>"
                       "<li><img source=\":/icons/landmask.png\">  - mark CHRTR2_LAND_MASK flagged data</li>"
                       "</ul><br><br>"
                       "The CHRTR2_USER_NN flags may be disabled if the CHRTR2 file being viewed does not have valid "
                       "user flags.");

QString hideText = 
  gridPointEdit3D::tr ("Click this button to access a pulldown menu allowing you to select flagged data to be hidden.  "
                       "There are five options available.  These are:<br><br>"
                       "<ul>"
                       "<li>hide CHRTR2_USER_01 flagged data</li>"
                       "<li>hide CHRTR2_USER_02 flagged data</li>"
                       "<li>hide CHRTR2_USER_03 flagged data</li>"
                       "<li>hide CHRTR2_USER_04 flagged data</li>"
                       "<li>hide CHRTR2_LAND_MASK flagged data</li>"
                       "</ul><br><br>"
                       "The CHRTR2_USER_NN flags may be disabled if the CHRTR2 file being viewed does not have valid "
                       "user flags.  More than one data type may be selected to be hidden.<br><br>"
                       "<b>IMPORTANT NOTE: Selecting any of these options <i>DOES NOT</i> cause data that doesn't "
                       "contain matching CHRTR2_USER flags to be unmasked (i.e. shown).  If a point is already masked "
                       "it will remain masked.  Also, once you hide something you can't get it back until you reset "
                       "or click a show button.  This is so you can combine data type/flag hiding with masking.</b>");


QString showText = 
  gridPointEdit3D::tr ("Click this button to access a pulldown menu allowing you to select flagged data to be shown.  "
                       "There are five options available.  These are:<br><br>"
                       "<ul>"
                       "<li>show CHRTR2_USER_01 flagged data</li>"
                       "<li>show CHRTR2_USER_02 flagged data</li>"
                       "<li>show CHRTR2_USER_03 flagged data</li>"
                       "<li>show CHRTR2_USER_04 flagged data</li>"
                       "<li>show CHRTR2_LAND_MASK flagged data</li>"
                       "</ul><br><br>"
                       "The CHRTR2_USER_NN flags may be disabled if the CHRTR2 file being viewed does not have valid "
                       "user flags.  More than one data type may be selected to be hidden.<br><br>"
                       "<b>IMPORTANT NOTE: Selecting any of these options will hide any data that does not "
                       "match the CHRTR2_USER flag.  Think of this as a <i>'show only these types'</i> option.</b>");

QString layerPrefsText = 
  gridPointEdit3D::tr ("Selecting this menu item will cause the layer preferences dialog to appear.  In this dialog you can "
                       "turn display of the layers (i.e. CHRTR2 files) on or off.");

QString toolbarText = 
  gridPointEdit3D::tr ("There are four available tool bars in the gridPointEdit3D program.  They are<br><br>"
                       "<ul>"
                       "<li>View tool bar - contains buttons to change the view</li>"
                       "<li>Utilities tool bar - contains buttons to modify the preferences and get context sensitive help</li>"
                       "<li>Edit tool bar - contains buttons to set editing modes</li>"
                       "<li>Mask tool bar - contains buttons to mask data inside or outside of rectangles or polygons</li>"
                       "</ul>"
                       "The tool bars may be turned on or off and relocated to any location on the screen.  You may click and "
                       "drag the tool bars using the handle to the left of (or above) the tool bar.  Right clicking in the tool "
                       "bar or in the menu bar will pop up a menu allowing you to hide or show the individual tool bars.  The "
                       "location, orientation, and visibility of the tool bars will be saved on exit.");

QString maskInsideRectText = 
  gridPointEdit3D::tr ("<img source=\":/icons/mask_inside_rect.png\"> Click this button to allow you to select a rectangular area "
                       "inside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                       "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize the "
                       "rectangle and mask the data, left click again.  If, at any time during the operation, you wish to "
                       "discard the rectangle and abort the operation simply click the middle mouse button or right click and "
                       "select one of the menu options.  To clear all masking press the mask reset button "
                       "<img source=\":/icons/mask_reset.png\">");
QString maskOutsideRectText = 
  gridPointEdit3D::tr ("<img source=\":/icons/mask_outside_rect.png\"> Click this button to allow you to select a rectangular area "
                       "outside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                       "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize the "
                       "rectangle and mask the data, left click again.  If, at any time during the operation, you wish to "
                       "discard the rectangle and abort the operation simply click the middle mouse button or right click and "
                       "select one of the menu options.  To clear all masking press the mask reset button "
                       "<img source=\":/icons/mask_reset.png\">");
QString maskInsidePolyText = 
  gridPointEdit3D::tr ("<img source=\":/icons/mask_inside_poly.png\"> Click this button to allow you to select a polygonal area "
                       "inside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                       "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                       "mask the data simply left click again.  If, at any time during the operation, "
                       "you wish to discard the polygon and abort the operation simply click the middle mouse "
                       "button or right click and select a menu option.  To clear all masking press the mask reset button "
                       "<img source=\":/icons/mask_reset.png\">");
QString maskOutsidePolyText = 
  gridPointEdit3D::tr ("<img source=\":/icons/mask_outside_poly.png\"> Click this button to allow you to select a polygonal area "
                       "outside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                       "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                       "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                       "mask the data simply left click again.  If, at any time during the operation, "
                       "you wish to discard the polygon and abort the operation simply click the middle mouse "
                       "button or right click and select a menu option.  To clear all masking press the mask reset button "
                       "<img source=\":/icons/mask_reset.png\">");
QString maskResetText = 
  gridPointEdit3D::tr ("<img source=\":/icons/mask_reset.png\"> Click this button to clear all data masking.");

QString measureText = 
  gridPointEdit3D::tr ("<img source=\":/icons/measure.png\"> Click this button to measure distances and depth differences.  When this button "
                       "is clicked the cursor will become the measure cursor <img source=\":/icons/measure_cursor.png\">.  Position the "
                       "box/crosshair on a point and then move the cursor.    The distance in meters, azimuth in degrees, and the difference "
                       "in Z in meters will be displayed in the status bar.  When you are finished click the left mouse button.<br><br>"
                       "<b>IMPORTANT NOTE: There are two sets of distance, azimuth, and deltaZ values.  They are labeled Nearest Point and "
                       "Cursor.  The nearest point is the one that the box/crosshair snaps to.  The cursor position is a bit more "
                       "complicated.  The cursor position is computed by assuming that the cursor  is actually in a plane that is parallel to the "
                       "screen face and at the depth of the anchor point (the starting point of the line, not the nearest point).  This gives "
                       "the user the ability to measure approximate distances without having to use the position of the nearest point to "
                       "compute an absolute distance.<br><br>"
                       "<font color=\"#ff0000\">TIP: To minimize Z distortion when trying to get approximate distances press the Reset button "
                       "<img source=\":/icons/reset_view.png\"> and then use the left and right arrow keys to rotate the view.</font></b>");

QString undoText = 
  gridPointEdit3D::tr ("<img source=\":/icons/undo.png\"> Click this button to undo the last edit operation.  When there are no edit "
                       "operations to be undone this button will not be enabled.");
