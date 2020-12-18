
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



//  gridPointEdit3D class.

/***************************************************************************\

    Module :        gridPointEdit3D

    Programmer :    Jan C. Depner

    Date :          03/03/05

    Purpose :       3D version of Area-Based Editor editor.

\***************************************************************************/

#include <unistd.h>
    
#include "gridPointEdit3D.hpp"
#include "gridPointEdit3DHelp.hpp"


void envout (OPTIONS *options, MISC *misc, QMainWindow *mainWindow);


gridPointEdit3D::gridPointEdit3D (int *argc, char **argv, QWidget *parent):
  QMainWindow (parent, 0)
{
  uint8_t min_hsv_color_set = NVFalse;
  uint8_t max_hsv_color_set = NVFalse;
  uint8_t min_hsv_value_set = NVFalse;
  uint8_t max_hsv_value_set = NVFalse;
  uint16_t min_hsv_color = 0;
  uint16_t max_hsv_color = 0;
  float min_hsv_value = 0.0;
  float max_hsv_value = 0.0;


  uint8_t envin (OPTIONS *options, MISC *misc, QMainWindow *mainWindow);
  void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore);


  extern char *optarg;
  int32_t option_index = 0;
  int32_t shmid = 0;


  //  Read the acknowledgments file for the Acknowledgments Help button.  This way I only have
  //  to change one file and copy it to the other programs icon folders to change the Acknowledgments
  //  Help text instead of changing it in every single program I use it in.

  QFile *aDataFile = new QFile (":/icons/ACKNOWLEDGMENTS");

  if (aDataFile->open (QIODevice::ReadOnly))
    {
      char string[256];

      while (aDataFile->readLine (string, sizeof (string)) > 0)
        {
	  acknowledgmentsText.append (string);
        }
      aDataFile->close ();
    }


  options.z_factor = 1.0;
  options.z_offset = 0.0;

  while (NVTrue) 
    {
      static struct option long_options[] = {{"shared_memory_key", required_argument, 0, 0},
                                             {"min_hsv_color", required_argument, 0, 0},
                                             {"max_hsv_color", required_argument, 0, 0},
                                             {"min_hsv_value", required_argument, 0, 0},
                                             {"max_hsv_value", required_argument, 0, 0},
                                             {0, no_argument, 0, 0}};

      char c = (char) getopt_long (*argc, argv, "n", long_options, &option_index);
      if (c == -1) break;

      switch (c) 
        {
        case 0:

          switch (option_index)
            {
            case 0:
              sscanf (optarg, "%d", &shmid);
              break;

            case 1:
              sscanf (optarg, "%hd", &min_hsv_color);
              min_hsv_color_set = NVTrue;
              break;

            case 2:
              sscanf (optarg, "%hd", &max_hsv_color);
              max_hsv_color_set = NVTrue;
              break;

            case 3:
              sscanf (optarg, "%f", &min_hsv_value);
              min_hsv_value = -min_hsv_value;
              min_hsv_value_set = NVTrue;
              break;

            case 4:
              sscanf (optarg, "%f", &max_hsv_value);
              max_hsv_value = -max_hsv_value;
              max_hsv_value_set = NVTrue;
              break;
            }
          break;


          //  This is a place saver.  Not used.

        case 'n':
          break;
        }
    }


  //  We must have the shared memory ID.

  if (!shmid)
    {
      QMessageBox::critical (this, tr ("gridPointEdit3D"), 
                             tr ("gridPointEdit3D can only be run from gridEdit using shared memory."),
                             QMessageBox::Close);
      exit (-1);
    }


  //  Get the shared memory area.  If it doesn't exist, quit.  It should
  //  have already been created by gridEdit.  The key is the process ID of the parent
  //  plus "_chrtr2".  The parent process ID is a required command line argument
  //  (--shared_memory_id).

  QString skey;
  skey.sprintf ("%d_chrtr2", shmid);


  misc.chrtr2Share = new QSharedMemory (skey);

  if (misc.chrtr2Share->attach (QSharedMemory::ReadWrite)) misc.chrtr2_share = (CHRTR2_SHARE *) misc.chrtr2Share->data ();


  options.position_form = misc.chrtr2_share->position_form;
  options.z_factor = misc.chrtr2_share->z_factor;
  options.z_offset = misc.chrtr2_share->z_offset;


  //  Have to set the focus policy or keypress events don't work properly at first in Focus Follows Mouse mode

  setFocusPolicy (Qt::WheelFocus);
  setFocus (Qt::ActiveWindowFocusReason);
  

  //  Set a few defaults for startup

  polygon_flip = NVFalse;
  prev_poly_x = -1;
  prev_poly_y = -1;
  popup_active = NVFalse;
  double_click = NVFalse;
  prefs_dialog = NULL;
  mv_marker = -1;
  mv_tracker = -1;
  mv_2D_tracker = -1;
  mv_measure_anchor = -1;
  mv_measure = -1;
  rb_polygon = -1;
  rb_measure = -1;
  rb_rectangle = -1;
  start_ctrl_x = -1;
  start_ctrl_y = -1;
  rotate_index = 0;
  lock_point = 0;
  slicing = NVFalse;
  misc.nearest_point = 0;


  save_z = NULL;


  //  Set up the marker data list

  marker[0].x = -MARKER_W;
  marker[0].y = -MARKER_H;
  marker[1].x = MARKER_W;
  marker[1].y = -MARKER_H;
  marker[2].x = MARKER_W;
  marker[2].y = -MARKER_H;
  marker[3].x = MARKER_W;
  marker[3].y = MARKER_H;
  marker[4].x = MARKER_W;
  marker[4].y = MARKER_H;
  marker[5].x = -MARKER_W;
  marker[5].y = MARKER_H;
  marker[6].x = -MARKER_W;
  marker[6].y = MARKER_H;
  marker[7].x = -MARKER_W;
  marker[7].y = -MARKER_H;
  marker[8].x = -MARKER_W;
  marker[8].y = 0;
  marker[9].x = -(MARKER_W / 5);
  marker[9].y = 0;
  marker[10].x = (MARKER_W / 5);
  marker[10].y = 0;
  marker[11].x = MARKER_W;
  marker[11].y = 0;
  marker[12].x = 0;
  marker[12].y = -MARKER_H;
  marker[13].x = 0;
  marker[13].y = -(MARKER_W / 5);
  marker[14].x = 0;
  marker[14].y = (MARKER_W / 5);
  marker[15].x = 0;
  marker[15].y = MARKER_H;


  //  Make the 2D "tracker" cursor painter path.

  tracker_2D = QPainterPath ();

  tracker_2D.moveTo (0, 0);
  tracker_2D.lineTo (30, 0);
  tracker_2D.lineTo (30, 20);
  tracker_2D.lineTo (0, 20);
  tracker_2D.lineTo (0, 0);

  tracker_2D.moveTo (0, 10);
  tracker_2D.lineTo (12, 10);

  tracker_2D.moveTo (30, 10);
  tracker_2D.lineTo (18, 10);

  tracker_2D.moveTo (15, 0);
  tracker_2D.lineTo (15, 6);

  tracker_2D.moveTo (15, 20);
  tracker_2D.lineTo (15, 14);


  //  Set the main icon

  setWindowIcon (QIcon (":/icons/gridPointEdit3D.jpg"));


  //  Define all of the cursors

  deleteRectCursor = QCursor (QPixmap (":/icons/delete_rect_cursor.png"), 3, 0);
  deletePolyCursor = QCursor (QPixmap (":/icons/delete_poly_cursor.png"), 3, 0);
  keepPolyCursor = QCursor (QPixmap (":/icons/keep_poly_cursor.png"), 5, 0);
  rotateCursor = QCursor (QPixmap (":/icons/rotate_cursor.png"), 17, 17);
  zoomCursor = QCursor (QPixmap (":/icons/zoom_cursor.png"), 11, 11);
  maskCursor = QCursor (QPixmap (":/icons/mask_cursor.png"), 1, 1);
  measureCursor = QCursor (QPixmap (":/icons/measure_cursor.png"), 1, 1);
  highlightPolyCursor = QCursor (QPixmap (":/icons/highlight_polygon_cursor.png"), 1, 1);
  clearPolyCursor = QCursor (QPixmap (":/icons/clear_polygon_cursor.png"), 1, 1);


  if (misc.chrtr2_share->polygon_count)
    {
      misc.chrtr2_share->edit_area.min_y = 999999999.0;
      misc.chrtr2_share->edit_area.max_y = -999999999.0;
      misc.chrtr2_share->edit_area.min_x = 999999999.0;
      misc.chrtr2_share->edit_area.max_x = -999999999.0;

      for (int32_t i = 0 ; i < misc.chrtr2_share->polygon_count ; i++)
        {
          misc.chrtr2_share->edit_area.min_y = qMin (misc.chrtr2_share->edit_area.min_y, misc.chrtr2_share->polygon_y[i]);
          misc.chrtr2_share->edit_area.min_x = qMin (misc.chrtr2_share->edit_area.min_x, misc.chrtr2_share->polygon_x[i]);
          misc.chrtr2_share->edit_area.max_y = qMax (misc.chrtr2_share->edit_area.max_y, misc.chrtr2_share->polygon_y[i]);
          misc.chrtr2_share->edit_area.max_x = qMax (misc.chrtr2_share->edit_area.max_x, misc.chrtr2_share->polygon_x[i]);
        }
    }


  //  Open the CHRTR2 files and compute the MBR of all of the CHRTR2 areas.

  misc.total_mbr.min_y = 999.0;
  misc.total_mbr.min_x = 999.0;
  misc.total_mbr.max_y = -999.0;
  misc.total_mbr.max_x = -999.0;

  for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
    {
      misc.chrtr2_handle[chrtr2] = chrtr2_open_file (misc.chrtr2_share->chrtr2_file[chrtr2], &misc.chrtr2_share->chrtr2_header[chrtr2], CHRTR2_UPDATE);

      if (misc.chrtr2_handle[chrtr2] < 0)
        {
          QMessageBox::warning (this, tr ("gridPointEdit Open CHRTR2 Structure"), tr ("The file ") + 
                                QDir::toNativeSeparators (QString (misc.chrtr2_share->chrtr2_file[chrtr2])) + 
                                tr (" is not a CHRTR2 structure or there was an error reading the file.") +
                                tr ("  The error message returned was:\n\n") + QString (chrtr2_strerror ()));
        }


      misc.total_mbr.min_y = qMin (misc.total_mbr.min_y, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
      misc.total_mbr.min_x = qMin (misc.total_mbr.min_x, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
      misc.total_mbr.max_y = qMax (misc.total_mbr.max_y, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
      misc.total_mbr.max_x = qMax (misc.total_mbr.max_x, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.elon);
    }


  //  Set all of the defaults

  set_defaults (&misc, &options, NVFalse);


  //  We have to get the icon size out of sequence (the rest of the options are read in env_in_out.cpp)
  //  so that we'll have the proper sized icons for the toolbars.  Otherwise, they won't be placed correctly.

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc.qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc.qsettings_app) + ".ini";
#endif


  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc.qsettings_app);
  options.main_button_icon_size = settings.value (tr ("main button icon size"), options.main_button_icon_size).toInt ();
  settings.endGroup ();


  //  Create View toolbar

  toolBar[0] = new QToolBar (tr ("View tool bar"));
  toolBar[0]->setToolTip (tr ("View tool bar"));
  addToolBar (toolBar[0]);
  toolBar[0]->setObjectName (tr ("gridPointEdit3D View tool bar"));

  QButtonGroup *exitGrp = new QButtonGroup (this);
  connect (exitGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotExit (int)));

  bExitSave = new QToolButton (this);
  misc.button[SAVE_EXIT_KEY] = bExitSave;
  bExitSave->setIcon (misc.buttonIcon[SAVE_EXIT_KEY]);
  exitGrp->addButton (bExitSave, 0);
  bExitSave->setWhatsThis (exitSaveText);
  toolBar[0]->addWidget (bExitSave);

  bExitMask = new QToolButton (this);
  misc.button[SAVE_EXIT_MASK_KEY] = bExitMask;
  bExitMask->setIcon (misc.buttonIcon[SAVE_EXIT_MASK_KEY]);
  exitGrp->addButton (bExitMask, 1);
  bExitMask->setWhatsThis (exitMaskText);
  toolBar[0]->addWidget (bExitMask);

  bExitNoSave = new QToolButton (this);
  misc.button[NO_SAVE_EXIT_KEY] = bExitNoSave;
  bExitNoSave->setIcon (misc.buttonIcon[NO_SAVE_EXIT_KEY]);
  exitGrp->addButton (bExitNoSave, 2);
  bExitNoSave->setWhatsThis (exitNoSaveText);
  toolBar[0]->addWidget (bExitNoSave);

  bReset = new QToolButton (this);
  misc.button[RESET_KEY] = bReset;
  bReset->setIcon (misc.buttonIcon[RESET_KEY]);
  bReset->setWhatsThis (resetText);
  connect (bReset, SIGNAL (clicked ()), this, SLOT (slotReset ()));
  toolBar[0]->addWidget (bReset);

  bInterpolated = new QToolButton (this);
  bInterpolated->setIcon (QIcon (":/icons/display_interpolated.png"));
  bInterpolated->setToolTip (tr ("Toggle display of interpolated data"));
  bInterpolated->setWhatsThis (interpolatedText);
  bInterpolated->setCheckable (true);
  toolBar[0]->addWidget (bInterpolated);


  //  If you add to the 5 buttons below you must change the value of PRE_USER in gridPointEdit3DDef.hpp.  These are the
  //  pre-user flag buttons.  NOTE: flag_invalid should always be the last before the USER flags (i.e. PRE_USER - 1).

  flagIcon[0] = QIcon (":/icons/user_flag.png");


  flagIcon[PRE_USER + 0] = QIcon (":/icons/user_flag01.png");
  flagIcon[PRE_USER + 1] = QIcon (":/icons/user_flag02.png");
  flagIcon[PRE_USER + 2] = QIcon (":/icons/user_flag03.png");
  flagIcon[PRE_USER + 3] = QIcon (":/icons/user_flag04.png");
  flagIcon[PRE_USER + 4] = QIcon (":/icons/user_flag05.png");


  QMenu *flagMenu = new QMenu (this);

  QActionGroup *flagGrp = new QActionGroup (this);
  connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
  flagGrp->setExclusive (true);

  flag[0] = flagMenu->addAction (tr ("Turn off data flags"));
  flagGrp->addAction (flag[0]);
  flag[0]->setIcon (flagIcon[0]);
  flag[0]->setCheckable (true);

  for (int32_t i = 0 ; i < 5 ; i++)
    {
      QString string = tr ("Mark ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[i] + tr (" data");
      flag[i + PRE_USER] = flagMenu->addAction (string);
      flagGrp->addAction (flag[i + PRE_USER]);
      flag[i + PRE_USER]->setIcon (flagIcon[i + PRE_USER]);
      flag[i + PRE_USER]->setCheckable (true);
    }


  bFlag = new QToolButton (this);
  bFlag->setIcon (QIcon (":/icons/user_flag.png"));
  bFlag->setToolTip (tr ("Toggle data flags on/off"));
  bFlag->setWhatsThis (flagText);
  bFlag->setPopupMode (QToolButton::InstantPopup);
  bFlag->setMenu (flagMenu);
  toolBar[0]->addWidget (bFlag);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  QButtonGroup *modeGrp = new QButtonGroup (this);
  connect (modeGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotMode (int)));
  modeGrp->setExclusive (true);


  bHighlightPoly = new QToolButton (this);
  misc.button[HIGHLIGHT_POLYGON_MODE_KEY] = bHighlightPoly;
  bHighlightPoly->setIcon (misc.buttonIcon[HIGHLIGHT_POLYGON_MODE_KEY]);
  modeGrp->addButton (bHighlightPoly, HIGHLIGHT_POLYGON);
  bHighlightPoly->setWhatsThis (highlightPolyText);
  bHighlightPoly->setCheckable (true);
  toolBar[0]->addWidget (bHighlightPoly);

  bClearPoly = new QToolButton (this);
  misc.button[CLEAR_POLYGON_MODE_KEY] = bClearPoly;
  bClearPoly->setIcon (misc.buttonIcon[CLEAR_POLYGON_MODE_KEY]);
  modeGrp->addButton (bClearPoly, CLEAR_POLYGON);
  bClearPoly->setWhatsThis (clearPolyText);
  bClearPoly->setCheckable (true);
  toolBar[0]->addWidget (bClearPoly);

  bClearHighlight = new QToolButton (this);
  misc.button[CLEAR_HIGHLIGHT_KEY] = bClearHighlight;
  bClearHighlight->setIcon (misc.buttonIcon[CLEAR_HIGHLIGHT_KEY]);
  bClearHighlight->setWhatsThis (clearHighlightText);
  connect (bClearHighlight, SIGNAL (clicked ()), this, SLOT (slotClearHighlight ()));
  toolBar[0]->addWidget (bClearHighlight);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bContour = new QToolButton (this);
  misc.button[TOGGLE_CONTOUR_KEY] = bContour;
  bContour->setIcon (misc.buttonIcon[TOGGLE_CONTOUR_KEY]);
  bContour->setWhatsThis (contourText);
  bContour->setCheckable (true);
  toolBar[0]->addWidget (bContour);


  //  Create Utilities toolbar

  toolBar[1] = new QToolBar (tr ("Utilities tool bar"));
  toolBar[1]->setToolTip (tr ("Utilities tool bar"));
  addToolBar (toolBar[1]);
  toolBar[1]->setObjectName (tr ("gridPointEdit3D Utilities tool bar"));


  bPrefs = new QToolButton (this);
  bPrefs->setIcon (QIcon (":/icons/prefs.png"));
  bPrefs->setToolTip (tr ("Change application preferences"));
  bPrefs->setWhatsThis (prefsText);
  connect (bPrefs, SIGNAL (clicked ()), this, SLOT (slotPrefs ()));
  toolBar[1]->addWidget (bPrefs);


  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  bHelp = QWhatsThis::createAction (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  toolBar[1]->addAction (bHelp);


  //  Create Edit toolbar

  toolBar[2] = new QToolBar (tr ("Edit tool bar"));
  toolBar[2]->setToolTip (tr ("Edit tool bar"));
  addToolBar (toolBar[2]);
  toolBar[2]->setObjectName (tr ("gridPointEdit3D Edit tool bar"));

  bStop = new QToolButton (this);
  bStop->setIcon (QIcon (":/icons/stop.png"));
  bStop->setToolTip (tr ("Stop drawing"));
  bStop->setWhatsThis (stopText);
  bStop->setEnabled (false);
  connect (bStop, SIGNAL (clicked ()), this, SLOT (slotStop ()));
  toolBar[2]->addWidget (bStop);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bDeletePoint = new QToolButton (this);
  misc.button[DELETE_POINT_MODE_KEY] = bDeletePoint;
  bDeletePoint->setIcon (misc.buttonIcon[DELETE_POINT_MODE_KEY]);
  modeGrp->addButton (bDeletePoint, DELETE_POINT);
  bDeletePoint->setWhatsThis (deletePointText);
  bDeletePoint->setCheckable (true);
  toolBar[2]->addWidget (bDeletePoint);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bDeleteRect = new QToolButton (this);
  misc.button[DELETE_RECTANGLE_MODE_KEY] = bDeleteRect;
  bDeleteRect->setIcon (misc.buttonIcon[DELETE_RECTANGLE_MODE_KEY]);
  modeGrp->addButton (bDeleteRect, DELETE_RECTANGLE);
  bDeleteRect->setWhatsThis (deleteRectText);
  bDeleteRect->setCheckable (true);
  toolBar[2]->addWidget (bDeleteRect);

  bDeletePoly = new QToolButton (this);
  misc.button[DELETE_POLYGON_MODE_KEY] = bDeletePoly;
  bDeletePoly->setIcon (misc.buttonIcon[DELETE_POLYGON_MODE_KEY]);
  modeGrp->addButton (bDeletePoly, DELETE_POLYGON);
  bDeletePoly->setWhatsThis (deletePolyText);
  bDeletePoly->setCheckable (true);
  toolBar[2]->addWidget (bDeletePoly);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bKeepPoly = new QToolButton (this);
  bKeepPoly->setIcon (QIcon (":/icons/keep_poly.png"));
  modeGrp->addButton (bKeepPoly, KEEP_POLYGON);
  bKeepPoly->setToolTip (tr ("Invalidate data outside polygon"));
  bKeepPoly->setWhatsThis (keepPolyText);
  bKeepPoly->setCheckable (true);
  toolBar[2]->addWidget (bKeepPoly);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bUndo = new QToolButton (this);
  misc.button[UNDO_KEY] = bUndo;
  bUndo->setIcon (misc.buttonIcon[UNDO_KEY]);
  bUndo->setToolTip (tr ("Undo last edit operation"));
  bUndo->setWhatsThis (undoText);
  connect (bUndo, SIGNAL (clicked ()), this, SLOT (slotUndo ()));
  toolBar[2]->addWidget (bUndo);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bMeasure = new QToolButton (this);
  bMeasure->setIcon (QIcon (":/icons/measure.png"));
  bMeasure->setToolTip (tr ("Measure distance, azimuth, and Z difference"));
  modeGrp->addButton (bMeasure, MEASURE);
  bMeasure->setWhatsThis (measureText);
  bMeasure->setCheckable (true);
  toolBar[2]->addWidget (bMeasure);


  //  Create Mask toolbar

  toolBar[3] = new QToolBar (tr ("Mask tool bar"));
  toolBar[3]->setToolTip (tr ("Mask tool bar"));
  addToolBar (toolBar[3]);
  toolBar[3]->setObjectName (tr ("gridPointEdit3D Mask tool bar"));

  bMaskOutsideRect = new QToolButton (this);
  bMaskOutsideRect->setIcon (QIcon (":/icons/mask_outside_rect.png"));
  bMaskOutsideRect->setToolTip (tr ("Mask data outside a rectangle"));
  bMaskOutsideRect->setWhatsThis (maskOutsideRectText);
  bMaskOutsideRect->setCheckable (true);
  modeGrp->addButton (bMaskOutsideRect, MASK_OUTSIDE_RECTANGLE);
  toolBar[3]->addWidget (bMaskOutsideRect);

  bMaskOutsidePoly = new QToolButton (this);
  bMaskOutsidePoly->setIcon (QIcon (":/icons/mask_outside_poly.png"));
  bMaskOutsidePoly->setToolTip (tr ("Mask data outside a polygon"));
  bMaskOutsidePoly->setWhatsThis (maskOutsidePolyText);
  bMaskOutsidePoly->setCheckable (true);
  modeGrp->addButton (bMaskOutsidePoly, MASK_OUTSIDE_POLYGON);
  toolBar[3]->addWidget (bMaskOutsidePoly);


  toolBar[3]->addSeparator ();
  toolBar[3]->addSeparator ();


  bMaskInsideRect = new QToolButton (this);
  bMaskInsideRect->setIcon (QIcon (":/icons/mask_inside_rect.png"));
  bMaskInsideRect->setToolTip (tr ("Mask data inside a rectangle"));
  bMaskInsideRect->setWhatsThis (maskInsideRectText);
  bMaskInsideRect->setCheckable (true);
  modeGrp->addButton (bMaskInsideRect, MASK_INSIDE_RECTANGLE);
  toolBar[3]->addWidget (bMaskInsideRect);

  bMaskInsidePoly = new QToolButton (this);
  bMaskInsidePoly->setIcon (QIcon (":/icons/mask_inside_poly.png"));
  bMaskInsidePoly->setToolTip (tr ("Mask data inside a polygon"));
  bMaskInsidePoly->setWhatsThis (maskInsidePolyText);
  bMaskInsidePoly->setCheckable (true);
  modeGrp->addButton (bMaskInsidePoly, MASK_INSIDE_POLYGON);
  toolBar[3]->addWidget (bMaskInsidePoly);


  toolBar[3]->addSeparator ();
  toolBar[3]->addSeparator ();


  QMenu *hideMenu = new QMenu (this);

  hideGrp = new QActionGroup (this);
  hideGrp->setExclusive (false);
  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));


  for (int32_t i = 0 ; i < 5 ; i++)
    {
      QString user;
      user.sprintf (tr ("Hide CHRTR2_USER_%02d").toLatin1 (), i);
      hideAct[i] = hideMenu->addAction (user);
      hideAct[i]->setCheckable (true);
      hideAct[i]->setChecked (false);
      hideGrp->addAction (hideAct[i]);
    }


  bHide = new QToolButton (this);
  bHide->setToolTip (tr ("Hide data based on CHRTR2_USER flags"));
  bHide->setWhatsThis (hideText);
  bHide->setPopupMode (QToolButton::InstantPopup);
  bHide->setIcon (QIcon (":/icons/hide_user_flag.png"));
  bHide->setMenu (hideMenu);
  toolBar[3]->addWidget (bHide);


  QMenu *showMenu = new QMenu (this);


  showGrp = new QActionGroup (this);
  showGrp->setExclusive (false);
  connect (showGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotShowMenu (QAction *)));


  for (int32_t i = 0 ; i < 5 ; i++)
    {
      QString user;
      user.sprintf (tr ("Show CHRTR2_USER_%02d").toLatin1 (), i);
      showAct[i] = showMenu->addAction (user);
      showAct[i]->setCheckable (true);
      showAct[i]->setChecked (false);
      showGrp->addAction (showAct[i]);
    }


  bShow = new QToolButton (this);
  bShow->setToolTip (tr ("Show data based on CHRTR2_USER flags"));
  bShow->setWhatsThis (showText);
  bShow->setPopupMode (QToolButton::InstantPopup);
  bShow->setIcon (QIcon (":/icons/show_user_flag.png"));
  bShow->setMenu (showMenu);
  toolBar[3]->addWidget (bShow);


  toolBar[3]->addSeparator ();
  toolBar[3]->addSeparator ();


  bMaskReset = new QToolButton (this);
  bMaskReset->setIcon (QIcon (":/icons/mask_reset.png"));
  bMaskReset->setToolTip (tr ("Clear data mask"));
  bMaskReset->setWhatsThis (maskResetText);
  connect (bMaskReset, SIGNAL (clicked ()), this, SLOT (slotMaskReset ()));
  toolBar[3]->addWidget (bMaskReset);


  //  Set the icon sizes so that the tool bars will set up properly.

  prev_icon_size = options.main_button_icon_size;

  QSize mainButtonIconSize (options.main_button_icon_size, options.main_button_icon_size);

  for (int32_t i = 0 ; i < NUM_TOOLBARS ; i++) toolBar[i]->setIconSize (mainButtonIconSize);


  //  Get the user's defaults if available

  if (!envin (&options, &misc, this))
    {
      //  Set the geometry from defaults since we didn't get any from the saved settings.

      this->resize (misc.draw_area_width, misc.draw_area_height);
      this->move (0, 0);
    }


  // Set the application font

  QApplication::setFont (options.font);


  //  Connect the interpolated button here since we save this setting and retrieve it with envin.

  bInterpolated->setChecked (options.display_interpolated);
  connect (bInterpolated, SIGNAL (clicked ()), this, SLOT (slotInterpolated ()));


  //  Check the min and max colors and flip them if needed.

  if (min_hsv_color_set) misc.min_hsv_color = min_hsv_color;
  if (max_hsv_color_set) misc.max_hsv_color = max_hsv_color;

  if (misc.max_hsv_color > 315) misc.max_hsv_color = 315;
  if (misc.min_hsv_color > 315) misc.min_hsv_color = 0;

  if (misc.max_hsv_color < misc.min_hsv_color)
    {
      uint16_t tmpu16 = misc.max_hsv_color;
      misc.max_hsv_color = misc.min_hsv_color;
      misc.min_hsv_color = tmpu16;
    }


  //  Check the min and max values

  if (min_hsv_value_set)
    {
      misc.min_hsv_locked = NVTrue;
      misc.min_hsv_value = min_hsv_value;
    }
      
  if (max_hsv_value_set)
    {
      misc.max_hsv_locked = NVTrue;
      misc.max_hsv_value = max_hsv_value;
    }


  //  Allocate the colors.

  float hue_inc = (float) (misc.max_hsv_color - misc.min_hsv_color) / (float) (NUMSHADES + 1);

  for (int32_t j = 0 ; j < NUMSHADES ; j++)
    {
      misc.track_color_array[j].setHsv ((int32_t) ((j - misc.min_hsv_color) * hue_inc), 255, 255, 255); 
      misc.color_array[0][j][0] = misc.color_array[1][j][0] = misc.track_color_array[j].red ();
      misc.color_array[0][j][1] = misc.color_array[1][j][1] = misc.track_color_array[j].green ();
      misc.color_array[0][j][2] = misc.color_array[1][j][2] = misc.track_color_array[j].blue ();
      misc.color_array[0][j][3] = misc.track_color_array[j].alpha ();

      misc.color_array[1][j][3] = options.slice_alpha;
    }


  //  Allocate the undo blocks based on the max number of undo levels (user option).

  try
    {
      misc.undo.resize (options.undo_levels);
    }
  catch (std::bad_alloc&)
    {
      perror ("Allocating undo block pointers in gridPointEdit3D.cpp");
      exit (-1);
    }

  memset (misc.undo.data (), 0, options.undo_levels * sizeof (UNDO));


  //  Use geometry to get the width and height.

  QRect tmp = this->geometry ();
  int32_t width = tmp.width ();
  int32_t height = tmp.height ();


  misc.save_mode = options.function;
  misc.save_function = options.function;


  //  Set the map values from the defaults

  mapdef.draw_width = width;
  mapdef.draw_height = height;
  mapdef.exaggeration = options.exaggeration;
  mapdef.min_z_extents = 0.0;
  mapdef.mode = NVMAPGL_POINT_MODE;
  mapdef.auto_scale = NVTrue;
  mapdef.projected = 0;

  mapdef.coasts = NVFalse;
  mapdef.landmask = NVFalse;
  mapdef.light_model = GL_LIGHT_MODEL_AMBIENT;
  mapdef.renderMode = NVMAPGL_RENDER_POINT_MODE;

  mapdef.coast_color = Qt::white;
  mapdef.background_color = options.background_color;
  mapdef.scale_color = options.scale_color;
  mapdef.zoom_percent = options.zoom_percent;
  mapdef.draw_scale = options.draw_scale;


  //  Save the original bounds for use if we move the area from within gridPointEdit3D.

  misc.orig_bounds = misc.chrtr2_share->edit_area;


  //  If grid3D is up we need to tell it to clear it's memory until we are finished with the 
  //  3D editor.  If we have a huge amount of memory in use it can slow things down too much.
  //  The other end of this (i.e. setting GRIDPOINTEDIT3D_CLOSED) happens in gridEdit.

  misc.chrtr2Share->lock ();
  {
    misc.chrtr2_share->key = GRIDPOINTEDIT3D_OPENED;
  }
  misc.chrtr2Share->unlock ();


  //  Read in the data so that we can set the bounds correctly.

  get_buffer (&data, &misc);


  //  Set number of points loaded in the window title

  QString title;
  title.sprintf (tr (" - Number of points - %d").toLatin1 (), data.count);
  title.prepend (QString (VERSION));
  this->setWindowTitle (title);


  mapdef.initial_bounds.min_x = misc.displayed_area.min_x;
  mapdef.initial_bounds.min_y = misc.displayed_area.min_y;
  mapdef.initial_bounds.max_x = misc.displayed_area.max_x;
  mapdef.initial_bounds.max_y = misc.displayed_area.max_y;
  mapdef.initial_bounds.min_z = 999999999.0;
  mapdef.initial_bounds.max_z = -999999999.0;


  //  Compute the initial Z bounds.

  for (int32_t i = 0 ; i < data.count ; i++)
    {
      if (!check_bounds (&options, &misc, &data, i, NVFalse, NVFalse))
        {
          mapdef.initial_bounds.min_z = qMin (mapdef.initial_bounds.min_z, (double) -data.z[i]);
          mapdef.initial_bounds.max_z = qMax (mapdef.initial_bounds.max_z, (double) -data.z[i]);
        }
    }

  mapdef.initial_y_rotation = options.y_rotation;
  mapdef.initial_zx_rotation = options.zx_rotation;


  //  Save the displayed area for other CHRTR2 programs

  misc.chrtr2_share->displayed_area = misc.displayed_area;



  QFrame *frame = new QFrame (this, 0);

  setCentralWidget (frame);


  //  Make the map.

  map = new nvMapGL (this, &mapdef, "gridPointEdit3D");
  map->setWhatsThis (mapText);


  misc.map_widget = map;


  //  Connect to the signals from the map class.

  connect (map, SIGNAL (mousePressSignal (QMouseEvent *, double, double, double)), this,
           SLOT (slotMousePress (QMouseEvent *, double, double, double)));
  connect (map, SIGNAL (mouseDoubleClickSignal (QMouseEvent *, double, double, double)), this,
           SLOT (slotMouseDoubleClick (QMouseEvent *, double, double, double)));
  connect (map, SIGNAL (mouseReleaseSignal (QMouseEvent *, double, double, double)), this, 
           SLOT (slotMouseRelease (QMouseEvent *, double, double, double)));
  connect (map, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double, double, NVMAPGL_DEF)), this, 
           SLOT (slotMouseMove (QMouseEvent *, double, double, double, NVMAPGL_DEF)));
  connect (map, SIGNAL (wheelSignal (QWheelEvent *, double, double, double)), this, 
           SLOT (slotWheel (QWheelEvent *, double, double, double)));
  connect (map, SIGNAL (resizeSignal (QResizeEvent *)), this, SLOT (slotResize (QResizeEvent *)));
  connect (map, SIGNAL (exaggerationChanged (float, float)), this, SLOT (slotExaggerationChanged (float, float)));


  //  Layouts, what fun!

  QVBoxLayout *vBox = new QVBoxLayout (frame);


  //  Set up the VBox for the 2D tracker box and the status information.

  QGroupBox *leftBox = new QGroupBox ();
  QVBoxLayout *leftBoxLayout = new QVBoxLayout ();
  leftBox->setLayout (leftBoxLayout);


  int32_t trackBoxWidth = 275, trackBoxHeight = 275;


  //  Set up the 2D tracker box.

  track_mapdef.projection = CYLINDRICAL_EQUIDISTANT;
  track_mapdef.draw_width = 100;
  track_mapdef.draw_height = 100;
  track_mapdef.overlap_percent = 0;
  track_mapdef.grid_inc_x = 0.0;
  track_mapdef.grid_inc_y = 0.0;

  track_mapdef.coasts = NVFalse;
  track_mapdef.landmask = NVFalse;

  track_mapdef.border = 0;
  track_mapdef.coast_color = Qt::white;
  track_mapdef.grid_color = Qt::white;
  track_mapdef.background_color = Qt::black;

  track_mapdef.initial_bounds.min_x = misc.chrtr2_share->edit_area.min_x;
  track_mapdef.initial_bounds.min_y = misc.chrtr2_share->edit_area.min_y;
  track_mapdef.initial_bounds.max_x = misc.chrtr2_share->edit_area.max_x;
  track_mapdef.initial_bounds.max_y = misc.chrtr2_share->edit_area.max_y;


  trackMap = new nvMap (this, &track_mapdef);
  trackMap->setMinimumSize (trackBoxWidth - 20, trackBoxHeight - 20);
  trackMap->setMaximumSize (trackBoxWidth - 20, trackBoxHeight - 20);


  trackMap->setWhatsThis (trackMapText);


  //  Connect to the signals from the map class.

  connect (trackMap, SIGNAL (mousePressSignal (QMouseEvent *, double, double)), this,
           SLOT (slotTrackMousePress (QMouseEvent *, double, double)));
  connect (trackMap, SIGNAL (mouseReleaseSignal (QMouseEvent *, double, double)), this, 
           SLOT (slotTrackMouseRelease (QMouseEvent *, double, double)));
  connect (trackMap, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double)), this,
           SLOT (slotTrackMouseMove (QMouseEvent *, double, double)));
  connect (trackMap, SIGNAL (resizeSignal (QResizeEvent *)), this, SLOT (slotTrackResize (QResizeEvent *)));
  connect (trackMap, SIGNAL (preRedrawSignal (NVMAP_DEF)), this, SLOT (slotTrackPreRedraw (NVMAP_DEF)));
  connect (trackMap, SIGNAL (postRedrawSignal (NVMAP_DEF)), this, SLOT (slotTrackPostRedraw (NVMAP_DEF)));


  leftBoxLayout->addWidget (trackMap);


  //  Now we set up the information/status portion of the left side box.

  QGroupBox *filBox = new QGroupBox ();
  filBox->setMaximumWidth (trackBoxWidth);
  filBox->setMinimumWidth (trackBoxWidth);
  QVBoxLayout *filBoxLayout = new QVBoxLayout ();
  filBox->setLayout (filBoxLayout);

  chrtr2Label = new QLabel (this);
  chrtr2Label->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  chrtr2Label->setToolTip (tr ("CHRTR2 name for current point"));
  chrtr2Label->setWhatsThis (tr ("CHRTR2 name for current point"));
  filBoxLayout->addWidget (chrtr2Label);


  leftBoxLayout->addWidget (filBox);


  QGroupBox *statBox = new QGroupBox ();
  QGridLayout *statBoxLayout = new QGridLayout;
  statBox->setLayout (statBoxLayout);
  statBox->setMaximumWidth (trackBoxWidth);


  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      misc.scale[i] = new scaleBox (this);
      misc.scale[i]->setMinimumWidth (80);
      statBoxLayout->addWidget (misc.scale[i], i, 0, Qt::AlignLeft);
    }


  QLabel *latName = new QLabel ("Lat:", this);
  latName->setToolTip (tr ("Latitude of cursor"));
  latName->setMinimumSize (latName->sizeHint ());
  statBoxLayout->addWidget (latName, 0, 1, Qt::AlignLeft);

  latLabel = new QLabel ("N 90 00 00.00", this);
  latLabel->setToolTip (tr ("Latitude of cursor"));
  latLabel->setMinimumSize (latLabel->sizeHint ());
  statBoxLayout->addWidget (latLabel, 0, 2, Qt::AlignLeft);
  
  QLabel *lonName = new QLabel ("Lon:", this);
  lonName->setToolTip (tr ("Longitude of cursor"));
  lonName->setMinimumSize (lonName->sizeHint ());
  statBoxLayout->addWidget (lonName, 1, 1, Qt::AlignLeft);

  lonLabel = new QLabel ("N 90 00 00.00", this);
  lonLabel->setToolTip (tr ("Longitude of cursor"));
  lonLabel->setMinimumSize (lonLabel->sizeHint ());
  statBoxLayout->addWidget (lonLabel, 1, 2, Qt::AlignLeft);

  QLabel *coordXName = new QLabel ("CoordX:", this);
  coordXName->setToolTip (tr ("X grid coordinate of current point"));
  coordXName->setMinimumSize (coordXName->sizeHint ());
  coordXName->setMaximumSize (coordXName->sizeHint ());
  statBoxLayout->addWidget (coordXName, 2, 1, Qt::AlignLeft);

  coordXLabel = new QLabel (this);
  coordXLabel->setToolTip (tr ("X grid coordinate of current point"));
  coordXLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (coordXLabel, 2, 2, Qt::AlignLeft);

  QLabel *coordYName = new QLabel ("CoordY:", this);
  coordYName->setToolTip (tr ("Y grid coordinate of current point"));
  coordYName->setMinimumSize (coordYName->sizeHint ());
  coordYName->setMaximumSize (coordYName->sizeHint ());
  statBoxLayout->addWidget (coordYName, 3, 1, Qt::AlignLeft);

  coordYLabel = new QLabel (this);
  coordYLabel->setToolTip (tr ("Y grid coordinate of current point"));
  coordYLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (coordYLabel, 3, 2, Qt::AlignLeft);

  QLabel *zName = new QLabel ("Z:", this);
  zName->setToolTip (tr ("Current point Z value"));
  zName->setMinimumSize (zName->sizeHint ());
  statBoxLayout->addWidget (zName, 4, 1, Qt::AlignLeft);

  zLabel = new QLabel ("0000.00", this);
  zLabel->setToolTip (tr ("Current point Z value"));
  zLabel->setMinimumSize (zLabel->sizeHint ());
  zLabel->setAutoFillBackground (true);
  zPalette = zLabel->palette ();
  statBoxLayout->addWidget (zLabel, 4, 2, Qt::AlignLeft);

  QLabel *hvName = new QLabel ("H / V:", this);
  hvName->setToolTip (tr ("Horizontal/Vertical uncertainty of current point"));
  hvName->setMinimumSize (hvName->sizeHint ());
  hvName->setMaximumSize (hvName->sizeHint ());
  statBoxLayout->addWidget (hvName, 5, 1, Qt::AlignLeft);

  hvLabel = new QLabel (this);
  hvLabel->setToolTip (tr ("Horizontal/Vertical uncertainty of current point"));
  hvLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (hvLabel, 5, 2, Qt::AlignLeft);


  QLabel *exagName = new QLabel ("Exag:", this);
  exagName->setToolTip (tr ("Vertical exaggeration"));
  exagName->setMinimumSize (exagName->sizeHint ());
  statBoxLayout->addWidget (exagName, 6, 1, Qt::AlignLeft);

  exagLabel = new QLabel (this);
  exagLabel->setToolTip (tr ("Vertical exaggeration"));
  exagLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (exagLabel, 6, 2, Qt::AlignLeft);
  QString lbl;
  lbl.sprintf (tr ("%5.3f").toLatin1 (), options.exaggeration);
  exagLabel->setText (lbl);


  statBoxLayout->setColumnStretch (1, 1);
  statBoxLayout->setRowStretch (NUM_SCALE_LEVELS + 1, 0);


  leftBoxLayout->addWidget (statBox, 1);


  exagBar = new QScrollBar (Qt::Vertical);
  exagBar->setTracking (true);
  exagBar->setInvertedAppearance (true);
  exagBar->setToolTip (tr ("Vertical exaggeration"));
  exagBar->setWhatsThis (exagBarText);
  exagPalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  exagPalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  exagPalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  exagPalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  exagBar->setPalette (exagPalette);
  exagBar->setMinimum (100);
  exagBar->setMaximum (10000);
  exagBar->setSingleStep (100);
  exagBar->setPageStep (500);
  connect (exagBar, SIGNAL (actionTriggered (int)), this, SLOT (slotExagTriggered (int)));
  connect (exagBar, SIGNAL (sliderReleased ()), this, SLOT (slotExagReleased ()));


  QGroupBox *sliceBox = new QGroupBox (tr ("Slice"));
  QVBoxLayout *sliceBoxLayout = new QVBoxLayout ();
  sliceBox->setLayout (sliceBoxLayout);


  QHBoxLayout *barBoxLayout = new QHBoxLayout ();


  sliceBar = new QScrollBar (Qt::Vertical);
  sliceBar->setTracking (true);
  sliceBar->setInvertedAppearance (true);
  sliceBar->setToolTip (tr ("Slice through the data (<b>Up Arrow</b> or <b>Down Arrow</b>)"));
  sliceBar->setWhatsThis (sliceBarText);
  slicePalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  sliceBar->setPalette (slicePalette);
  connect (sliceBar, SIGNAL (actionTriggered (int)), this, SLOT (slotSliceTriggered (int)));
  barBoxLayout->addWidget (sliceBar);


  QVBoxLayout *ctrlBox = new QVBoxLayout ();

  transBar = new QScrollBar (Qt::Vertical);
  transBar->setTracking (true);
  transBar->setInvertedAppearance (true);
  transBar->setToolTip (tr ("Transparency of data outside of slice"));
  transBar->setWhatsThis (transText);
  transPalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  transPalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  transPalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  transPalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  transBar->setPalette (transPalette);
  transBar->setMinimum (0);
  transBar->setMaximum (MAX_TRANS_VALUE);
  transBar->setSingleStep (1);
  transBar->setPageStep (8);
  transBar->setValue (options.slice_alpha);
  connect (transBar, SIGNAL (actionTriggered (int)), this, SLOT (slotTransTriggered (int)));
  ctrlBox->addWidget (transBar, 1);


  transLabel = new QLabel ("000", this);
  transLabel->setAlignment (Qt::AlignCenter);
  transLabel->setMinimumSize (transLabel->sizeHint ());
  transLabel->setToolTip (tr ("Transparency value"));
  transLabel->setWhatsThis (transText);
  transLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  transLabel->setLineWidth (2);
  QString string;
  string.sprintf ("%02d", options.slice_alpha);
  transLabel->setText (string);
  ctrlBox->addWidget (transLabel);


  sizeBar = new QScrollBar (Qt::Vertical);
  sizeBar->setTracking (true);
  sizeBar->setInvertedAppearance (true);
  sizeBar->setToolTip (tr ("Size of slice as percentage of area"));
  sizeBar->setWhatsThis (sizeText);
  sizePalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  sizePalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  sizePalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  sizePalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  sizeBar->setPalette (sizePalette);
  sizeBar->setMinimum (1);
  sizeBar->setMaximum (MAX_SLICE_SIZE);
  sizeBar->setSingleStep (1);
  sizeBar->setPageStep (5);
  sizeBar->setValue (options.slice_percent);
  connect (sizeBar, SIGNAL (actionTriggered (int)), this, SLOT (slotSizeTriggered (int)));
  ctrlBox->addWidget (sizeBar, 1);

  sizeLabel = new QLabel ("00", this);
  sizeLabel->setAlignment (Qt::AlignCenter);
  sizeLabel->setMinimumSize (sizeLabel->sizeHint ());
  sizeLabel->setToolTip (tr ("Slice size as a percentage"));
  sizeLabel->setWhatsThis (sizeText);
  sizeLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  sizeLabel->setLineWidth (2);
  sizeLabel->setNum (options.slice_percent);
  ctrlBox->addWidget (sizeLabel);

  barBoxLayout->addLayout (ctrlBox);

  sliceBoxLayout->addLayout (barBoxLayout);


  meterLabel = new QLabel ("00000.0", this);
  meterLabel->setAlignment (Qt::AlignCenter);
  meterLabel->setMinimumSize (meterLabel->sizeHint ());
  meterLabel->setToolTip (tr ("Slice size in meters"));
  meterLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  meterLabel->setLineWidth (2);
  meterLabel->setText ("");

  sliceBoxLayout->addWidget (meterLabel);


  QHBoxLayout *hBox = new QHBoxLayout ();
  hBox->addWidget (leftBox);
  hBox->addWidget (exagBar);
  hBox->addWidget (map, 1);
  hBox->addWidget (sliceBox);
  vBox->addLayout (hBox, 1);


  //  Set Edit toolbar button states based on saved options

  if (options.function == DELETE_POINT) bDeletePoint->setChecked (true);

  if (options.function == DELETE_RECTANGLE) bDeleteRect->setChecked (true);

  if (options.function == DELETE_POLYGON) bDeletePoly->setChecked (true);

  if (options.function == KEEP_POLYGON) bKeepPoly->setChecked (true);

  bContour->setChecked (options.display_contours);
  connect (bContour, SIGNAL (clicked ()), this, SLOT (slotContour ()));


  //  Set all of the button hotkey tooltips and shortcuts

  for (int32_t i = 0 ; i < HOTKEYS ; i++) slotPrefHotKeyChanged (i);


  //  Set the CHRTR2 user flag hide and show text and tooltips.

  hideAct[0]->setText (tr ("Hide ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[0] + " data");
  hideAct[1]->setText (tr ("Hide ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[1] + " data");
  hideAct[2]->setText (tr ("Hide ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[2] + " data");
  hideAct[3]->setText (tr ("Hide ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[3] + " data");
  hideAct[4]->setText (tr ("Hide Land Masked data"));

  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[0], "CHRTR2_USER_01")) hideAct[0]->setVisible (false);
  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[1], "CHRTR2_USER_02")) hideAct[1]->setVisible (false);
  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[2], "CHRTR2_USER_03")) hideAct[2]->setVisible (false);
  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[3], "CHRTR2_USER_04")) hideAct[3]->setVisible (false);


  showAct[0]->setText (tr ("Show ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[0] + " data");
  showAct[1]->setText (tr ("Show ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[1] + " data");
  showAct[2]->setText (tr ("Show ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[2] + " data");
  showAct[3]->setText (tr ("Show ") + misc.chrtr2_share->chrtr2_header[0].user_flag_name[3] + " data");

  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[0], "CHRTR2_USER_01")) showAct[0]->setVisible (false);
  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[1], "CHRTR2_USER_02")) showAct[1]->setVisible (false);
  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[2], "CHRTR2_USER_03")) showAct[2]->setVisible (false);
  if (!strcmp (misc.chrtr2_share->chrtr2_header[0].user_flag_name[3], "CHRTR2_USER_04")) showAct[3]->setVisible (false);


  //  Right click popup menu

  popupMenu = new QMenu (map);
  connect (popupMenu, SIGNAL (triggered (QAction *)), this, SLOT (slotPopupMenu (QAction *)));

  for (int32_t i = 0 ; i < NUMPOPUPS ; i++) popup[i] = popupMenu->addAction (("            "));

  popupMenu->addSeparator ();
  popupHelp = popupMenu->addAction (tr ("Help"));
  connect (popupHelp, SIGNAL (triggered ()), this, SLOT (slotPopupHelp ()));


  //  Setup the file menu (don't set shortcuts for these menu items because they're handled by the keypress event with the associated buttons)

  saveExitAction = new QAction (tr ("Save and Exit"), this);
  saveExitAction->setStatusTip (tr ("Save changes and exit"));
  connect (saveExitAction, SIGNAL (triggered ()), this, SLOT (slotExitSave ()));

  maskExitAction = new QAction (tr ("Save, Exit, and Mask"), this);
  maskExitAction->setStatusTip (tr ("Save changes, exit, and filter mask in gridEdit"));
  connect (maskExitAction, SIGNAL (triggered ()), this, SLOT (slotExitMask ()));

  QAction *noSaveExitAction = new QAction (tr ("Exit"), this);
  noSaveExitAction->setStatusTip (tr ("Exit without saving changes"));
  connect (noSaveExitAction, SIGNAL (triggered ()), this, SLOT (slotExitNoSave ()));


  QMenu *fileMenu = menuBar ()->addMenu (tr ("File"));
  fileMenu->addAction (saveExitAction);
  fileMenu->addAction (maskExitAction);
  fileMenu->addAction (noSaveExitAction);


  //  Setup the Edit menu

  QAction *prefsAct = new QAction (tr ("Preferences"), this);
  prefsAct->setStatusTip (tr ("Change program preferences"));
  prefsAct->setWhatsThis (prefsText);
  connect (prefsAct, SIGNAL (triggered ()), this, SLOT (slotPrefs ()));


  QMenu *editMenu = menuBar ()->addMenu (tr ("Edit"));
  editMenu->addAction (prefsAct);


  //  Setup the Layers menu.

  QAction *layerPrefs = new QAction (tr ("Display"), this);
  layerPrefs->setStatusTip (tr ("Turn display of layers on or off"));
  layerPrefs->setWhatsThis (layerPrefsText);
  connect (layerPrefs, SIGNAL (triggered ()), this, SLOT (slotLayers ()));

  layerMenu = menuBar ()->addMenu (tr ("Layers"));
  layerMenu->addAction (layerPrefs);


  //  Setup the help menu.

  QAction *toolHelp = new QAction (tr ("Tool bars"), this);
  toolHelp->setStatusTip (tr ("Help on tool bars"));
  connect (toolHelp, SIGNAL (triggered ()), this, SLOT (slotToolbarHelp ()));

  QAction *whatsThisAct = QWhatsThis::createAction (this);
  whatsThisAct->setIcon (QIcon (":/icons/contextHelp.png"));

  QAction *aboutAct = new QAction (tr ("About"), this);
  aboutAct->setStatusTip (tr ("Information about gridPointEdit3D"));
  connect (aboutAct, SIGNAL (triggered ()), this, SLOT (about ()));

  QAction *acknowledgments = new QAction (tr ("Acknowledgments"), this);
  acknowledgments->setStatusTip (tr ("Information about supporting libraries"));
  connect (acknowledgments, SIGNAL (triggered ()), this, SLOT (slotAcknowledgments ()));

  QAction *aboutQtAct = new QAction (tr ("About Qt"), this);
  aboutQtAct->setStatusTip (tr ("Information about Qt"));
  connect (aboutQtAct, SIGNAL (triggered ()), this, SLOT (aboutQt ()));

  QMenu *helpMenu = menuBar ()->addMenu (tr ("Help"));
  helpMenu->addAction (toolHelp);
  helpMenu->addAction (whatsThisAct);
  helpMenu->addSeparator ();
  helpMenu->addAction (aboutAct);
  helpMenu->addSeparator ();
  helpMenu->addAction (acknowledgments);
  helpMenu->addAction (aboutQtAct);


  setMainButtons (NVTrue);


  //  Set up the status bars.

  QGroupBox *fpBox = new QGroupBox ();
  QGridLayout *fpBoxLayout = new QGridLayout;
  fpBox->setLayout (fpBoxLayout);


  misc.statusProgLabel = new QLabel (this);
  misc.statusProgLabel->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  misc.statusProgLabel->setToolTip (tr ("Active mode"));
  misc.statusProgLabel->setWhatsThis (miscLabelText);
  misc.statusProgLabel->setAutoFillBackground (true);
  misc.statusProgPalette = misc.statusProgLabel->palette ();
  fpBoxLayout->addWidget (misc.statusProgLabel, 1, 0);


  //  Progress status bar

  misc.progStatus = new QStatusBar ();
  misc.progStatus->setSizeGripEnabled (false);
  misc.progStatus->setToolTip (tr ("Progress bar and label"));
  misc.progStatus->setWhatsThis (progStatusText);
  fpBoxLayout->addWidget (misc.progStatus, 1, 1);


  misc.statusProg = new QProgressBar (this);
  misc.statusProg->setRange (0, 100);
  misc.statusProg->setValue (0);
  misc.statusProg->setTextVisible (false);
  misc.progStatus->addWidget (misc.statusProg, 10);


  vBox->addWidget (fpBox);


  //  Set the tracking timer function.  This will track the cursor in other CHRTR2 program map windows.
  //  Also used to limit the number of points in the polygon functions.

  trackCursor = new QTimer (this);
  connect (trackCursor, SIGNAL (timeout ()), this, SLOT (slotTrackCursor ()));
  trackCursor->start (50);


  //  Set the blink timer (half second) function.

  blinkTimer = new QTimer (this);
  connect (blinkTimer, SIGNAL (timeout ()), this, SLOT (slotBlinkTimer ()));
  blinkTimer->start (500);


  setFunctionCursor (options.function);


  show ();


  //  Get the map center for the reset view slot.

  map->getMapCenter (&misc.map_center_x, &misc.map_center_y, &misc.map_center_z);


  redrawMap (NVTrue);
}



gridPointEdit3D::~gridPointEdit3D ()
{
}



void 
gridPointEdit3D::setMainButtons (uint8_t enable)
{
  //  If we changed the icon size in the preferences dialog we need to resize all of the buttons.

  if (options.main_button_icon_size != prev_icon_size)
    {
      QSize mainButtonIconSize (options.main_button_icon_size, options.main_button_icon_size);

      for (int32_t i = 0 ; i < NUM_TOOLBARS ; i++)
        {
          toolBar[i]->setIconSize (mainButtonIconSize);
          toolBar[i]->adjustSize ();
        }

      prev_icon_size = options.main_button_icon_size;
    }


  bExitNoSave->setEnabled (enable);
  bReset->setEnabled (enable);


  //  Only enable the layers menu if we have more than one layer.

  if (misc.chrtr2_share->chrtr2_count > 1)
    {
      layerMenu->setEnabled (enable);
    }
  else
    {
      layerMenu->setEnabled (false);
    }


  //  Only enable UNDO if we saved an undo block.

  if (misc.undo_count)
    {
      bUndo->setEnabled (enable);
    }
  else
    {
      bUndo->setEnabled (false);
    }


  //  Set the size of the slice in meters if slicing is on.

  if (misc.slice)
    {
      QString string;
      if (misc.slice_meters < 10.0)
        {
          string.sprintf ("%.2f", misc.slice_meters);
        }
      else if (misc.slice_meters < 100.0)
        {
          string.sprintf ("%.1f", misc.slice_meters);
        }
      else
        {
          string.sprintf ("%.0f", misc.slice_meters);
        }
      meterLabel->setText (string);
    }
  else
    {
      meterLabel->setText ("");
    }

  bExitSave->setEnabled (enable);
  bExitMask->setEnabled (enable);
  saveExitAction->setEnabled (enable);
  maskExitAction->setEnabled (enable);

  bInterpolated->setEnabled (enable);

  bContour->setEnabled (enable);

  bPrefs->setEnabled (enable);


  //  Stop is always the inverse.

  bStop->setEnabled (!enable);


  bDeletePoint->setEnabled (enable);
  bDeleteRect->setEnabled (enable);
  bDeletePoly->setEnabled (enable);
  bKeepPoly->setEnabled (enable);


  bFlag->setIcon (flagIcon[options.flag_index]);
  bFlag->setEnabled (enable);

  if (!enable || (enable && misc.highlight_count))
    {
      bClearHighlight->setEnabled (enable);
      bClearPoly->setEnabled (enable);
    }
  bHighlightPoly->setEnabled (enable);

  bMeasure->setEnabled (enable);
  bMaskInsideRect->setEnabled (enable);
  bMaskInsidePoly->setEnabled (enable);
  bMaskOutsideRect->setEnabled (enable);
  bMaskOutsidePoly->setEnabled (enable);

  bHide->setEnabled (enable);
  bShow->setEnabled (enable);

  if (!enable || misc.mask_active) bMaskReset->setEnabled (enable);
}



void 
gridPointEdit3D::setFunctionCursor (int32_t function)
{
  //  Default to point mode for the map.

  map->setMapMode (NVMAPGL_POINT_MODE);


  discardMovableObjects ();


  switch (function)
    {
    case DELETE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Rectangular delete mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deleteRectCursor);
      bDeleteRect->setChecked (true);
      break;

    case DELETE_POLYGON:
      misc.statusProgLabel->setText (tr ("Polygon delete mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deletePolyCursor);
      bDeletePoly->setChecked (true);
      break;

    case KEEP_POLYGON:
      misc.statusProgLabel->setText (tr ("Delete outside polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (keepPolyCursor);
      bKeepPoly->setChecked (true);
      break;

    case DELETE_POINT:
      misc.statusProgLabel->setText (tr ("Delete subrecord/record mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::PointingHandCursor);
      bDeletePoint->setChecked (true);
      break;

    case NOOP:
      misc.statusProgLabel->setText (tr ("No Op mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::PointingHandCursor);
      break;

    case ROTATE:
      misc.statusProgLabel->setText (tr ("Rotate data"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (rotateCursor);
      if (misc.need_sparse) map->setMapMode (NVMAPGL_SPARSE_MODE);
      break;

    case ZOOM:
      misc.statusProgLabel->setText (tr ("Zoom in/out"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (zoomCursor);
      if (misc.need_sparse) map->setMapMode (NVMAPGL_SPARSE_MODE);
      break;

    case MASK_INSIDE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Mask inside rectangle mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);
      break;

    case MASK_OUTSIDE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Mask outside rectangle mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);
      break;

    case MASK_INSIDE_POLYGON:
      misc.statusProgLabel->setText (tr ("Mask inside polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);
      break;

    case MASK_OUTSIDE_POLYGON:
      misc.statusProgLabel->setText (tr ("Mask outside polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);
      break;

    case MEASURE:
      misc.statusProgLabel->setText (tr ("Measuring mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (measureCursor);
      break;

    case HIGHLIGHT_POLYGON:
      misc.statusProgLabel->setText (tr ("Highlight points in polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (highlightPolyCursor);
      bHighlightPoly->setChecked (true);
      break;

    case CLEAR_POLYGON:
      misc.statusProgLabel->setText (tr ("Clear highlights in polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (clearPolyCursor);
      bClearPoly->setChecked (true);
      break;
    }
}



void 
gridPointEdit3D::discardMovableObjects ()
{
  map->closeMovingList (&mv_marker);
  map->closeMovingList (&mv_tracker);
  trackMap->closeMovingPath (&mv_2D_tracker);
  map->closeMovingList (&mv_measure_anchor);
  map->closeMovingList (&mv_measure);
  map->discardRubberbandRectangle (&rb_rectangle);
  map->discardRubberbandPolygon (&rb_polygon);
  map->discardRubberbandLine (&rb_measure);
}



void 
gridPointEdit3D::leftMouse (int32_t mouse_x, int32_t mouse_y, double lon __attribute__ ((unused)),
                      double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  int32_t *px, *py;


  //  If the popup menu is up discard this mouse press

  if (popup_active)
    {
      popup_active = NVFalse;
      return;
    }


  switch (options.function)
    {
    case NOOP:
    case DELETE_POINT:
      break;
    
    case DELETE_RECTANGLE:
    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:

      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, mouse_x, mouse_y, &px, &py);


          //  Invalidate, restore, or mask the area.

          qApp->setOverrideCursor (Qt::WaitCursor);
          qApp->processEvents ();

          set_area (map, &options, &misc, &data, px, py, 4, 1);

          qApp->restoreOverrideCursor ();


          map->discardRubberbandRectangle (&rb_rectangle);


          if (options.function == MASK_INSIDE_RECTANGLE || options.function == MASK_OUTSIDE_RECTANGLE) misc.mask_active = NVTrue;


          redrawMap (NVTrue);


          setFunctionCursor (options.function);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, mouse_x, mouse_y, options.edit_color, 2, Qt::SolidLine);
        }
      break;

    case DELETE_POLYGON:
    case KEEP_POLYGON:
    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
    case HIGHLIGHT_POLYGON:
    case CLEAR_POLYGON:

      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          int32_t count;
          map->closeRubberbandPolygon (rb_polygon, mouse_x, mouse_y, &count, &px, &py);


          //  Invalidate or restore the area.

          qApp->setOverrideCursor (Qt::WaitCursor);
          qApp->processEvents ();

          switch (options.function)
            {
            case DELETE_POLYGON:
            case HIGHLIGHT_POLYGON:
            case CLEAR_POLYGON:
              set_area (map, &options, &misc, &data, px, py, count, 0);
              break;

            case MASK_INSIDE_POLYGON:
            case MASK_OUTSIDE_POLYGON:
              set_area (map, &options, &misc, &data, px, py, count, 0);
              misc.mask_active = NVTrue;
              break;

            case KEEP_POLYGON:
              keep_area (map, &options, &misc, &data, px, py, count);
              break;
            }

          qApp->restoreOverrideCursor ();

          prev_poly_x = -1;


          map->discardRubberbandPolygon (&rb_polygon);
          redrawMap (NVTrue);
        }
      else
        {
          map->anchorRubberbandPolygon (&rb_polygon, mouse_x, mouse_y, options.edit_color, LINE_WIDTH, NVTrue, Qt::SolidLine);
          prev_poly_x = -1;
        }
      break;


    case MEASURE:
      if (map->rubberbandLineIsActive (rb_measure))
        {
          map->discardRubberbandLine (&rb_measure);
          map->closeMovingList (&mv_measure_anchor);
          map->closeMovingList (&mv_measure);
          map->flush ();
        }
      else
        {
          if (mv_measure_anchor < 0)
            {
              map->setMovingList (&mv_measure_anchor, marker, 16, mouse_x, mouse_y, 0.0, 2, options.edit_color);
            }
          else
            {
              map->setMovingList (&mv_measure, marker, 16, mouse_x, mouse_y, 0.0, 2, options.edit_color);
            }


          //  Snap the start of the line to the nearest point

          int32_t pix_x, pix_y;
          map->get2DCoords (data.x[misc.nearest_point], data.y[misc.nearest_point], -data.z[misc.nearest_point], &pix_x, &pix_y);

          map->anchorRubberbandLine (&rb_measure, pix_x, pix_y, options.edit_color, LINE_WIDTH, Qt::SolidLine);
          line_anchor.x = data.x[misc.nearest_point];
          line_anchor.y = data.y[misc.nearest_point];
          line_anchor.z = data.z[misc.nearest_point];
        }
      break;
    }
}



void 
gridPointEdit3D::midMouse (int32_t mouse_x __attribute__ ((unused)), int32_t mouse_y __attribute__ ((unused)),
                     double lon __attribute__ ((unused)), double lat __attribute__ ((unused)),
                     double z __attribute__ ((unused)))
{
  switch (options.function)
    {
    case NOOP:
      break;

    case DELETE_POINT:
      if (misc.nearest_point != -1)
        {
          store_undo (&misc, options.undo_levels, data.val[misc.nearest_point], misc.nearest_point);
          data.val[misc.nearest_point] = CHRTR2_NULL;
          end_undo_block (&misc);
        }

      redrawMap (NVTrue);
      break;

    case DELETE_RECTANGLE:
    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
      map->discardRubberbandRectangle (&rb_rectangle);
      break;

    case DELETE_POLYGON:
    case KEEP_POLYGON:
    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
    case HIGHLIGHT_POLYGON:
    case CLEAR_POLYGON:
      map->discardRubberbandPolygon (&rb_polygon);
      prev_poly_x = -1;
      break;

    case MEASURE:
      map->discardRubberbandLine (&rb_measure);
      map->closeMovingList (&mv_measure_anchor);
      map->closeMovingList (&mv_measure);
      break;
    }
  setFunctionCursor (options.function);
}



void 
gridPointEdit3D::slotMouseDoubleClick (QMouseEvent *e __attribute__ ((unused)), double lon __attribute__ ((unused)),
                                       double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  //  Lock in nearest point so mouse moves won't change it if we want to run one of the ancillary programs.

  lock_point = misc.nearest_point;


  //  Flip the double_click flag.  The right-click menu sets this to NVTrue so it will flip to NVFalse.
  //  Left-click sets it to NVFalse so it will filp to NVTrue;

  double_click = !double_click;


  //  Double click is hard-wired to turn off slicing.

  if (misc.slice) slotPopupMenu (popup[NUMPOPUPS - 1]);
}



//  Freakin' menus!

void 
gridPointEdit3D::rightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z __attribute__ ((unused)))
{
  QString tmp;


  menu_cursor_lon = lon;
  menu_cursor_lat = lat;
  menu_cursor_x = mouse_x;
  menu_cursor_y = mouse_y;


  QPoint pos (mouse_x, mouse_y);


  //  Lock in nearest point so mouse moves won't change it if we want to run one of the ancillary programs.

  lock_point = misc.nearest_point;


  //  Popups need global positioning

  QPoint global_pos = map->mapToGlobal (pos);


  //  In most cases popup[2] through popup[NUMPOPUPS-1] are not used so we set them to false.  If we need them for specific buttons
  //  we'll set them to true in the switch.

  popup[0]->setVisible (true);
  popup[1]->setVisible (true);
  for (int32_t i = 2 ; i < NUMPOPUPS ; i++) popup[i]->setVisible (false);


  switch (options.function)
    {
    case NOOP:
      break;

    case DELETE_POINT:
      popup[0]->setText (tr ("Delete point"));
      popup[1]->setText (tr ("Highlight point"));


      //  Don't allow point marking if we are flagging some data.

      if (!options.flag_index)
        {
          popup[1]->setEnabled (true);
        }
      else
        {
          popup[1]->setEnabled (false);
        }

      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DELETE_RECTANGLE:
      popup[0]->setText (tr ("Close rectangle and delete points"));
      popup[1]->setText (tr ("Discard rectangle"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DELETE_POLYGON:
      popup[0]->setText (tr ("Close polygon and delete points"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case KEEP_POLYGON:
      popup[0]->setText (tr ("Close polygon and delete points"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
      popup[0]->setText (tr ("Close rectangle and mask data"));
      popup[1]->setText (tr ("Discard rectangle"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
      popup[0]->setText (tr ("Close polygon and mask data"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MEASURE:
      popup[0]->setVisible (false);
      popup[1]->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case HIGHLIGHT_POLYGON:
      popup[0]->setText (tr ("Close polygon and highlight points"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case CLEAR_POLYGON:
      popup[0]->setText (tr ("Close polygon and un-highlight points"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;
    }

  if (misc.slice)
    {
      popup[NUMPOPUPS - 1]->setText (tr ("Turn off slicing"));
      popup[NUMPOPUPS - 1]->setVisible (true);
    }
}



//  IMPORTANT NOTE:  Never use misc.nearest_point in slotPopupMenu.  Use lock_point instead.  This is set when we right-click,
//  double-click, or press a key so that subsequent mouse movement will not affect what we were trying to do when we clicked or pressed.

void 
gridPointEdit3D::slotPopupMenu (QAction *ac)
{
  QStringList lst, items;
  QString res_str, item, boxSize;


  //  Nice little debug statement for the next time I add a popup slot and forget to change this code.

  if (NUMPOPUPS > 4) fprintf (stderr, "%s %s %d - Someone tell Jan to fix this function because NUMPOPUPS has changed\n", __FILE__,__FUNCTION__,__LINE__);


  popup_active = NVFalse;


  //  Each of the following popup slots will execute whatever was set up in rightMouse above.  Instead of 
  //  commenting each piece and then having to change it each time I move something I'll let the rightMouse
  //  function serve as documentation for each piece.

  if (ac == popup[0])
    {
      switch (options.function)
        {
        case NOOP:
          break;

        case DELETE_POINT:
          if (lock_point != -1)
            {
              //  Single point kills are a real waste of undo ;-)

              store_undo (&misc, options.undo_levels, data.val[lock_point], lock_point);
              data.val[lock_point] = CHRTR2_NULL;
              end_undo_block (&misc);

              redrawMap (NVTrue);
            }
          break;

        case DELETE_RECTANGLE:
        case DELETE_POLYGON:
        case KEEP_POLYGON:
        case MASK_INSIDE_RECTANGLE:
        case MASK_OUTSIDE_RECTANGLE:
        case MASK_INSIDE_POLYGON:
        case MASK_OUTSIDE_POLYGON:
        case HIGHLIGHT_POLYGON:
        case CLEAR_POLYGON:
          leftMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
          break;
        }
    }
  else if (ac == popup[1])
    {
      switch (options.function)
        {
        case DELETE_POINT:
          try
            {
              misc.highlight.resize (misc.highlight_count + 1);
            }
          catch (std::bad_alloc&)
            {
              perror ("Allocating misc.highlight in gridPointEdit3D.cpp");
              exit (-1);
            }

          misc.highlight[misc.highlight_count] = lock_point;
          misc.highlight_count++;

          bClearHighlight->setEnabled (true);

          redrawMap (NVFalse);
          break;

        case DELETE_RECTANGLE:
        case DELETE_POLYGON:
          midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
          break;

        default:
          midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
          break;
        }
    }
  else if (ac == popup[2])
    {
    }
  else if (ac == popup[NUMPOPUPS - 1])
    {
      //  Turn off slicing if it is enabled.

      misc.slice = NVFalse;

      misc.slice_min = misc.ortho_min;
      sliceBar->setValue (misc.ortho_min);


      //  Turn on depth testing in the map widget

      map->setDepthTest (NVTrue);


      redrawMap (NVTrue);
    }
}



void 
gridPointEdit3D::slotPopupHelp ()
{
  switch (options.function)
    {
    case DELETE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), deleteRectText, map);
      break;

    case DELETE_POINT:
      QWhatsThis::showText (QCursor::pos ( ), deletePointText, map);
      break;

    case DELETE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), deletePolyText, map);
      break;

    case KEEP_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), keepPolyText, map);
      break;

    case MASK_INSIDE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), maskInsideRectText, map);
      break;

    case MASK_OUTSIDE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), maskOutsideRectText, map);
      break;

    case MASK_INSIDE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), maskInsidePolyText, map);
      break;

    case MASK_OUTSIDE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), maskOutsidePolyText, map);
      break;

    case MEASURE:
      QWhatsThis::showText (QCursor::pos ( ), measureText, map);
      break;

    case HIGHLIGHT_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), highlightPolyText, map);
      break;

    case CLEAR_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), clearPolyText, map);
      break;
    }

  if (options.function != DELETE_POINT) midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
}



//  Timer - timeout signal.  Very much like an X workproc.  This tracks the cursor in associated
//  programs, not in this window.  This is active whenever the mouse leaves this window.

void
gridPointEdit3D::slotTrackCursor ()
{
  char                  ltstring[25], lnstring[25];
  QString               string;
  static NV_F64_COORD2  prev_xy;


  //  If we're in the process of redrawing we need to get out of here.

  if (misc.drawing) return;


  //  Since this is always a child process of something we want to exit (saving settings) if we see the
  //  CHILD_PROCESS_FORCE_EXIT key

  //if (misc.chrtr2_share->key == CHILD_PROCESS_FORCE_EXIT) slotExit (0);


  //  If we're not in this window, erase the marker.

  if (misc.chrtr2_share->active_window_id != misc.process_id) map->closeMovingList (&mv_marker);                 


  NV_F64_COORD2 xy;
  xy.y = misc.chrtr2_share->cursor_position.y;
  xy.x = misc.chrtr2_share->cursor_position.x;

  if (misc.chrtr2_share->active_window_id != misc.process_id && xy.y >= misc.displayed_area.min_y && xy.y <= misc.displayed_area.max_y && 
      xy.x >= misc.displayed_area.min_x && xy.x <= misc.displayed_area.max_x && xy.y != prev_xy.y && xy.x != prev_xy.x)
    {
      //  Find the nearest point to the cursor's location in whatever window has the focus.

      double min_dist = 999999999.0;
      int32_t hit = -1;
      for (int32_t i = 0 ; i < data.count ; i++)
        {
          double dist = sqrt (((xy.y - data.y[i]) * (xy.y - data.y[i])) + ((xy.x - data.x[i]) * (xy.x - data.x[i])));

          if (dist < min_dist)
            {
              min_dist = dist;
              hit = i;
            }
        }


      if (hit >= 0)
        {
          //  First set the 3D tracker.

          int32_t pix_x, pix_y;
          map->get2DCoords (data.x[hit], data.y[hit], -data.z[hit], &pix_x, &pix_y);
          map->setMovingList (&mv_tracker, marker, 16, pix_x, pix_y, 0.0, 2, options.tracker_color);


          //  Then set the 2D tracker.

          QBrush b1;
          trackMap->setMovingPath (&mv_2D_tracker, tracker_2D, data.x[hit], data.y[hit], 2, options.tracker_color,
                                   b1, NVFalse, Qt::SolidLine);
        }


      double deg, min, sec;
      char       hem;

      strcpy (ltstring, fixpos (xy.y, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
      strcpy (lnstring, fixpos (xy.x, &deg, &min, &sec, &hem, POS_LON, options.position_form));

      latLabel->setText (ltstring);
      lonLabel->setText (lnstring);


      NV_I32_COORD2 coord;
      chrtr2_get_coord (misc.chrtr2_handle[0], xy.y, xy.x, &coord);

      coordXLabel->setNum (coord.x);
      coordYLabel->setNum (coord.y);
    }
  prev_xy = xy;
}



//  Blink timer slot.

void
gridPointEdit3D::slotBlinkTimer ()
{
  static int32_t     blink = 0;


  //  We want to blink the mask reset button (every half_second) if we have a mask set so that the user will know.

  if (misc.mask_active)
    {
      if (blink)
        {
          bMaskReset->setIcon (QIcon (":/icons/mask_reset.png"));
          blink = 0;
        }
      else
        {
          bMaskReset->setIcon (QIcon (":/icons/mask_reset_red.png"));
          blink = 1;
        }

      qApp->processEvents ();
    }
}



//  Mouse press signal from the map class.

void 
gridPointEdit3D::slotMousePress (QMouseEvent *e, double lon, double lat, double z)
{
  if (misc.drawing) return;


  if (e->button () == Qt::LeftButton)
    {
      //  Check for the control key modifier.  If it's set, we want to rotate the image.

      if (e->modifiers () == Qt::ControlModifier)
        {
	  //  Get the cursor position at this time.

          start_ctrl_x = e->x ();
          start_ctrl_y = e->y ();

          misc.save_function = options.function;
          options.function = ROTATE;
          setFunctionCursor (options.function);
        }
      else
        {
          if (options.function == DELETE_POINT)
            {
              if (misc.nearest_point != -1)
                {
                  //  Check for the shift key modifier.  If it's set, we want to delete the point.

                  if (e->modifiers () == Qt::ShiftModifier)
                    {
                      store_undo (&misc, options.undo_levels, data.val[misc.nearest_point], misc.nearest_point);
                      data.val[misc.nearest_point] = CHRTR2_NULL;
                      end_undo_block (&misc);
                    }
                }
            }

          leftMouse (e->x (), e->y (), lon, lat, z);
        }
    }

  if (e->button () == Qt::MidButton)
    {
      //  Check for the control key modifier.  If it's set, we want to center on the cursor.

      if (e->modifiers () == Qt::ControlModifier)
        {
          map->setMapCenter (lon, lat, z);
        }
      else
        {
          midMouse (e->x (), e->y (), lon, lat, z);
        }
    }

  if (e->button () == Qt::RightButton)
    {
      //  Check for the control key modifier.  If it's set, we want to zoom based on cursor movement.

      if (e->modifiers () == Qt::ControlModifier)
        {
          start_ctrl_y = e->y ();

          misc.save_function = options.function;
          options.function = ZOOM;
          setFunctionCursor (options.function);
        }
      else
        {
          rightMouse (e->x (), e->y (), lon, lat, z);
        }
    }
}



//  Mouse release signal from the map class.

void 
gridPointEdit3D::slotMouseRelease (QMouseEvent *e, double lon __attribute__ ((unused)),
                           double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  if (e->button () == Qt::LeftButton)
    {
      if (options.function == ROTATE)
        {
          options.function = misc.save_function;
          setFunctionCursor (options.function);


          //  All we have to do is flush here since setFunctionCursor will set the map mode back to NVMAPGL_POINT_MODE and
          //  we haven't actually changed the point data.

          if (misc.need_sparse) map->flush ();
        }
      else
        {
          popup_active = NVFalse;
        }
    }

  if (e->button () == Qt::MidButton) popup_active = NVFalse;

  if (e->button () == Qt::RightButton)
    {
      if (options.function == ZOOM)
        {
          options.function = misc.save_function;
          setFunctionCursor (options.function);


          //  All we have to do is flush here since setFunctionCursor will set the map mode back to NVMAPGL_POINT_MODE and
          //  we haven't actually changed the point data.

          if (misc.need_sparse) map->flush ();
        }
    }
}



void 
gridPointEdit3D::geo_xyz_label (double lat, double lon, double z)
{
  char ltstring[25], lnstring[25], hem;
  QString val_string;
  double deg, min, sec;
  static float prev_z_factor = 1.0, prev_z_offset = 0.0;


  strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
  strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));

  latLabel->setText (ltstring);
  lonLabel->setText (lnstring);

  val_string.sprintf ("%.2f", -z * options.z_factor + options.z_offset);



  NV_I32_COORD2 coord;
  chrtr2_get_coord (misc.chrtr2_handle[0], lat, lon, &coord);

  coordXLabel->setNum (coord.x);
  coordYLabel->setNum (coord.y);


  //  Set yellow background for scaled or offset data.

  if ((options.z_factor != 1.0 && prev_z_factor == 1.0) || (options.z_offset != 0.0 && prev_z_offset == 0.0))
    {
      zPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      zLabel->setPalette (zPalette);
      zLabel->setToolTip (tr ("Scaled/offset current point Z value"));
      zLabel->setWhatsThis (tr ("Scaled/offset current point Z value"));
    }
  else if ((options.z_factor == 1.0 && prev_z_factor != 1.0) || (options.z_offset == 0.0 && prev_z_offset != 0.0))
    {
      zPalette.setColor (QPalette::Normal, QPalette::Window, misc.widgetBackgroundColor);
      zPalette.setColor (QPalette::Active, QPalette::Window, misc.widgetBackgroundColor);
      zLabel->setPalette (zPalette);
      zLabel->setToolTip (tr ("Current point Z value"));
      zLabel->setWhatsThis (tr ("Current point Z value"));
    }
  zLabel->setText (val_string);

  prev_z_factor = options.z_factor;
  prev_z_offset = options.z_offset;
}



//  Mouse wheel signal from the map class.

void
gridPointEdit3D::slotWheel (QWheelEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)),
                            double z __attribute__ ((unused)))
{
  if (e->delta () > 0)
    {
      //  Zoom in or out when pressing the Ctrl key and using the mouse wheel.

      if (e->modifiers () == Qt::ControlModifier) map->zoomInPercent ();


      //  Otherwise we're slicing.

      else
        {
          slotSliceTriggered (QAbstractSlider::SliderSingleStepSub);
        }
    }
  else
    {
      //  Zoom in or out when pressing the Ctrl key and using the mouse wheel.

      if (e->modifiers () == Qt::ControlModifier) map->zoomOutPercent ();


      //  Otherwise we're slicing.

      else
        {
          slotSliceTriggered (QAbstractSlider::SliderSingleStepAdd);
        }
    }
}



//  Mouse move signal from the map class.

void
gridPointEdit3D::slotMouseMove (QMouseEvent *e, double lon, double lat, double z, NVMAPGL_DEF l_mapdef)
{
  NV_I32_COORD2             xy;
  NV_F64_COORD3             hot = {999.0, 999.0, 999.0};
  static NV_I32_COORD2      prev_xy = {-1, -1};
  static NV_F64_COORD3      prev_hot;
  double                    dist, min_dist;
  static int32_t            prev_nearest_point = -1;



  //  Let other CHRTR2 programs know which window we're in.

  misc.chrtr2_share->active_window_id = misc.process_id;


  //  If it's still drawing don't do anything

  if (misc.drawing) return;


  //  Get rid of the tracking cursor from slotTrackCursor ().  But only if it already exists, otherwise we
  //  will be creating a new one (which we don't want to do).

  if (mv_tracker >= 0) map->closeMovingList (&mv_tracker);
  if (mv_2D_tracker >= 0) trackMap->closeMovingPath (&mv_2D_tracker);


  xy.x = e->x ();
  xy.y = e->y ();


  //  Track the cursor (not the marker) position for other CHRTR2 programs but only if we're in plan view

  misc.chrtr2_share->cursor_position.y = lat;
  misc.chrtr2_share->cursor_position.x = lon;
  misc.chrtr2_share->cursor_position.z = z;


  //  Only process if the pointer position has changed pixels

  if (xy.x == prev_xy.x && xy.y == prev_xy.y) return;


  QString chrtr2_string = "", distance_string = "", hv_string = "";
  double distance[2], azimuth[2];


  switch (options.function)
    {
    case ROTATE:
      if (start_ctrl_x != xy.x || start_ctrl_y != xy.y)
	{
	  int32_t diff_x = xy.x - start_ctrl_x;


	  if (diff_x)
            {
              //  Flip the sign if we are above the center point looking at the top of the surface or below
              //  the center point looking at the bottom.  This allows the Y rotation from the mouse click/drag
              //  to be more intuitive.

              double zxrot = map->getZXRotation ();
              int32_t half = l_mapdef.draw_height / 2;


	      //  If we're within 15 degrees of flat rotate normally for the front side.

	      if ((zxrot < 15.0 && zxrot > -15.0) || (zxrot > 345.0) || (zxrot < -345.0))
		{
		  //  Don't do anything.
		}


	      //  Reverse rotate reverse for the back side.

	      else if ((zxrot > -195.0 && zxrot < -165.0) || (zxrot < 195.0 && zxrot > 165.0))
		{
		  diff_x = -diff_x;
		}


	      //  Otherwise, check to see which side is up.

	      else
		{
		  if ((xy.y < half && ((zxrot > 0.0 && zxrot < 180.0) || (zxrot < -180.0))) ||
		      (xy.y > half && ((zxrot < 0.0 && zxrot > -180.0) || (zxrot > 180.0)))) diff_x = -diff_x;
		}

              map->rotateY ((double) diff_x / 5.0);
            }

	  int32_t diff_y = xy.y - start_ctrl_y;

	  if (diff_y) map->rotateZX ((double) diff_y / 5.0);

	  start_ctrl_x = xy.x;
	  start_ctrl_y = xy.y;
	}
      break;

    case ZOOM:
      if (start_ctrl_y != xy.y)
	{
	  int32_t diff_y = xy.y - start_ctrl_y;


	  if (diff_y < -10)
            {
              map->zoomInPercent ();
              start_ctrl_y = xy.y;
            }
          else if (diff_y > 10)
            {
              map->zoomOutPercent ();
              start_ctrl_y = xy.y;
            }
	}
      break;

    case NOOP:
    case DELETE_POINT:
    case MEASURE:

      misc.nearest_point = -1;


      //  Find the nearest valid point to the cursor.

      min_dist = 9999999999.0;
      for (int32_t i = 0 ; i < data.count ; i++)
        {
          if (!check_bounds (&options, &misc, &data, i, misc.slice, NVFalse))
            {
              dist = sqrt ((double) ((lat - data.y[i]) * (lat - data.y[i])) + (double) ((lon - data.x[i]) * (lon - data.x[i])));


              if (dist < min_dist)
                {
                  misc.nearest_point = i;

                  min_dist = dist;
                  hot.x = data.x[i];
                  hot.y = data.y[i];
                  hot.z = data.z[i];
                }
            }
        }


      misc.chrtr2_share->cursor_position.y = data.y[misc.nearest_point];
      misc.chrtr2_share->cursor_position.x = data.x[misc.nearest_point];



      //  Update the status bars

      // 
      //                      if our point has moved or we have just selected a new overplot we will go
      //                      into this code block.
      //

      if (misc.nearest_point != -1 && (misc.nearest_point != prev_nearest_point || options.function == MEASURE))
        {
          //  Show distance and azimuth to last highlighted point in status bar

          if (misc.highlight_count)
            {
              invgp (NV_A0, NV_B0, data.y[misc.highlight[misc.highlight_count - 1]], data.x[misc.highlight[misc.highlight_count - 1]],
                     data.y[misc.nearest_point], data.x[misc.nearest_point], &distance[0], &azimuth[0]);

              if (distance[0] < 0.01) azimuth[0] = 0.0;

              distance_string.sprintf ("Distance : %.2f (m)  Azimuth : %.2f (degrees)", distance[0], azimuth[0]);

              misc.statusProgLabel->setToolTip (tr ("Distance and azimuth from last highlighted point"));
              misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, misc.widgetBackgroundColor);
              misc.statusProgLabel->setPalette (misc.statusProgPalette);
              misc.statusProgLabel->setText (distance_string);
            }
          else
            {
              misc.statusProgLabel->setToolTip ("");
            }


          chrtr2_string = QFileInfo (QString (misc.chrtr2_share->chrtr2_file[data.chrtr2[misc.nearest_point]])).fileName ().remove (".ch2");
          chrtr2Label->setToolTip (QString (misc.chrtr2_share->chrtr2_file[data.chrtr2[misc.nearest_point]]));

          chrtr2Label->setText (chrtr2_string);

          hv_string.sprintf ("%.2f / %.2f", data.herr[misc.nearest_point], data.verr[misc.nearest_point]);
          hvLabel->setText (hv_string);


          //  Set the delete point marker.

          int32_t pix_x, pix_y;
          map->get2DCoords (hot.x, hot.y, -hot.z, &pix_x, &pix_y);

          map->setMovingList (&mv_marker, marker, 16, pix_x, pix_y, 0.0, 2, options.marker_color);


          if (options.function == MEASURE)
            {
              if (map->rubberbandLineIsActive (rb_measure))
                {
                  //  Snap the end of the line to the nearest point

                  int32_t pix_x, pix_y;
                  map->get2DCoords (data.x[misc.nearest_point], data.y[misc.nearest_point], -data.z[misc.nearest_point], &pix_x, &pix_y);

                  map->dragRubberbandLine (rb_measure, pix_x, pix_y);


                  invgp (NV_A0, NV_B0, data.y[misc.nearest_point], data.x[misc.nearest_point], line_anchor.y, line_anchor.x, &distance[0], &azimuth[0]);

                  if (distance[0] < 0.01) azimuth[0] = 0.0;

                  NV_F64_COORD3 coords;
                  map->getFaux3DCoords (line_anchor.x, line_anchor.y, -line_anchor.z, xy.x, xy.y, &coords);
                  invgp (NV_A0, NV_B0, coords.y, coords.x, line_anchor.y, line_anchor.x, &distance[1], &azimuth[1]);

                  if (distance[1] < 0.01) azimuth[1] = 0.0;

                  distance_string.sprintf ("Nearest Point Dist: %.2f  Az: %.2f  deltaZ: %.2f     Cursor Dist: %.2f  Az: %.2f  deltaZ: %.2f",
                                           distance[0], azimuth[0], line_anchor.z - data.z[misc.nearest_point], distance[1], azimuth[1], 
                                           line_anchor.z - (-coords.z));

                  misc.statusProgLabel->setText (distance_string);
                  misc.statusProgLabel->setVisible (true);
                }
            }
          geo_xyz_label (lat, lon, z);
        }


      //  Set the previous nearest point positions.
             
      prev_hot = hot;
      break;


    case DELETE_RECTANGLE:
    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
      if (map->rubberbandRectangleIsActive (rb_rectangle)) map->dragRubberbandRectangle (rb_rectangle, xy.x, xy.y);

      geo_xyz_label (lat, lon, z);
      chrtr2Label->setText (chrtr2_string);
      break;

    case DELETE_POLYGON:
    case KEEP_POLYGON:
    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
    case HIGHLIGHT_POLYGON:
    case CLEAR_POLYGON:

      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          if (!polygon_flip)
            {
              polygon_flip = NVTrue;

              if (prev_poly_x > -1)
                {
                  map->vertexRubberbandPolygon (rb_polygon, prev_poly_x, prev_poly_y);
                }
              else
                {
                  map->vertexRubberbandPolygon (rb_polygon, xy.x, xy.y);
                }
            }
          else
            {
              map->dragRubberbandPolygon (rb_polygon, xy.x, xy.y);
              prev_poly_x = xy.x;
              prev_poly_y = xy.y;
              polygon_flip = NVFalse;
            }
        }

      geo_xyz_label (lat, lon, z);
      chrtr2Label->setText (chrtr2_string);

      break;
    }


  //  Track the cursor in the 2D tracker box.

  QBrush b1;
  trackMap->setMovingPath (&mv_2D_tracker, tracker_2D, misc.chrtr2_share->cursor_position.x, misc.chrtr2_share->cursor_position.y, 2, options.tracker_color, b1,
                           NVFalse, Qt::SolidLine);


  //  Set the previous cursor.

  prev_xy = xy;

  prev_nearest_point = misc.nearest_point;
}



//  Resize signal from the map class.  With Qt 4.6.2 this seems to be redundant (and can cause all kinds of problems) so
//  this is just a stub at the moment.

void
gridPointEdit3D::slotResize (QResizeEvent *e __attribute__ ((unused)))
{
#if QT_VERSION < 0x040602
  redrawMap (NVTrue);
#endif
}



//  This is where we load all of our data in to OpenGL display lists (in nvMapGL.cpp).

void
gridPointEdit3D::redrawMap (uint8_t redraw2D)
{
  static uint8_t startup = NVTrue;


  void setScale (float actual_min, float actual_max, float attr_min, float attr_max, MISC *misc, OPTIONS *options, uint8_t min_lock,
                 uint8_t max_lock);
  uint8_t compute_bounds (OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t *viewable_count, float *attr_min, float *attr_max);



  misc.drawing = NVTrue;


  if (startup)
    {
      map->enableSignals ();
      trackMap->enableSignals ();
      startup = NVFalse;
    }


  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  discardMovableObjects ();


  //  Erase all text from the map.

  map->clearText ();


  //  Erase all objects (like spheres) from the map.

  map->clearDisplayLists ();


  //  Erase all data points from the map.

  map->clearDataPoints ();


  //  Erase all contour lines from the map.

  map->clearLines ();


  //  If we have more than sparse_limit points, erase all sparse points from the map.

  if (misc.need_sparse) map->clearSparsePoints ();


  setMainButtons (NVFalse);


  NVMAPGL_DEF l_mapdef = map->getMapdef ();


  misc.draw_area_height = l_mapdef.draw_height;
  misc.draw_area_width = l_mapdef.draw_width;

  map->setCursor (Qt::WaitCursor);

  misc.drawing_canceled = NVFalse;


  //  Compute the Z range of the data.

  int32_t viewable_count;
  float attr_min;
  float attr_max;

  misc.bounds = mapdef.initial_bounds;
  misc.bounds.min_z = CHRTRNULL;
  misc.bounds.max_z = -CHRTRNULL;


  //  Compute the min and max values for defining the 3D space and optionally for coloring.

  uint8_t bounds_changed = compute_bounds (&options, &misc, &data, &viewable_count, &attr_min, &attr_max);


  //  Compute the top level for drawing contours in the 3D space.  This is 1 percent above the top level so that it won't
  //  mask any data points.

  misc.contour_level = misc.bounds.max_z + ((misc.bounds.max_z - misc.bounds.min_z) * 0.01);


  //  If we're not coloring by attribute and the min or max hsv lock is set we need to check to
  //  see if we want to use the locked value(s).

  color_min_z = misc.bounds.min_z;
  color_max_z = misc.bounds.max_z;

  uint8_t min_lock = NVFalse, max_lock = NVFalse;
  if (!options.color_index)
    {
      if (misc.min_hsv_locked)
        {
          if (color_max_z > misc.min_hsv_value)
            {
              color_max_z = misc.min_hsv_value;
              max_lock = NVTrue;
            }
        }

      if (misc.max_hsv_locked)
        {
          if (color_min_z < misc.max_hsv_value)
            {
              color_min_z = misc.max_hsv_value;
              min_lock = NVTrue;
            }
        }
    }

  color_range_z = color_max_z - color_min_z;


  //  Only display if there are viewable points.

  if (viewable_count)
    {
      setScale (misc.bounds.min_z, misc.bounds.max_z, attr_min, attr_max, &misc, &options, min_lock, max_lock);


      range_x = misc.bounds.max_x - misc.bounds.min_x;
      range_y = misc.bounds.max_y - misc.bounds.min_y;
      range_z = misc.bounds.max_z - misc.bounds.min_z;


      //  Add 10 percent to the X, Y, and Z bounds.

      misc.bounds.min_x -= (range_x * 0.10);
      misc.bounds.max_x += (range_x * 0.10);

      misc.bounds.min_y -= (range_y * 0.10);
      misc.bounds.max_y += (range_y * 0.10);

      if (range_z == 0.0)
        {
          misc.bounds.min_z -= 1.0;
          misc.bounds.max_z += 1.0;
          range_z = misc.bounds.max_z - misc.bounds.min_z;
        }
      else
        {
          misc.bounds.min_z -= (range_z * 0.10);
          misc.bounds.max_z += (range_z * 0.10);
        }


      if (bounds_changed) map->setBounds (misc.aspect_bounds);


      if (!misc.slice) compute_ortho_values (map, &misc, &options, &data, sliceBar, NVTrue);


      int32_t c_index = -1;
      double attr_range = attr_max - attr_min;
      int32_t prev_xcoord = -1;
      int32_t prev_ycoord = -1;
      int32_t min_z_index = -1;
      int32_t max_z_index = -1;
      float min_z = 999999999.0;
      float max_z = -999999999.0;
      uint8_t save_min[4], save_max[4];
      float z_value;


      for (int32_t i = 0 ; i < data.count ; i++)
        {
          //  Check against the viewing area.

          int32_t trans = 0;
          if ((trans = check_bounds (&options, &misc, &data, i, misc.slice, NVFalse)) < 2)
            {
              z_value = data.z[i];


              //  Check for color by attribute

              if (!options.color_index)
                {
                  //  Color by depth

                  c_index = NINT ((NUMSHADES - 1) - ((-data.z[i]) - color_min_z) / color_range_z * (float) (NUMSHADES - 1));


                  //  Check for out of range data.

                  if (c_index < 0) c_index = 0;
                  if (c_index > NUMSHADES - 1) c_index = NUMSHADES - 1;
                }
              else if (options.color_index == 2)
                {
                  //  Color by horizontal uncertainty

                  c_index = (NUMSHADES - 1) - NINT ((data.herr[i] - attr_min) / attr_range * (float) (NUMSHADES - 1));
                }
              else if (options.color_index == 3)
                {
                  //  Color by vertical uncertainty

                  c_index = (NUMSHADES - 1) - NINT ((data.verr[i] - attr_min) / attr_range * (float) (NUMSHADES - 1));
                }


              uint8_t tmp[4];

              if (c_index < 0)
                {
                  tmp[0] = options.ref_color[trans].red ();
                  tmp[1] = options.ref_color[trans].green ();
                  tmp[2] = options.ref_color[trans].blue ();
                  tmp[3] = options.ref_color[trans].alpha ();
                }
              else
                {
                  //  Check for locked data and plot in the reference color.

                  if (misc.chrtr2_share->lock_chrtr2[data.chrtr2[i]])
                    {
                      tmp[0] = options.ref_color[0].red ();
                      tmp[1] = options.ref_color[0].green ();
                      tmp[2] = options.ref_color[0].blue ();
                      tmp[3] = options.ref_color[0].alpha ();
                    }
                  else
                    { 
                      memcpy (tmp, misc.color_array[trans][c_index], 4);
                    }
                }


              //  If we have more than sparse_limit points we want to load a sparse layer for rotating and zooming.

              if (misc.need_sparse)
                {
                  //  If we are slicing and we need sparse data for rotation and zoom, only display sparse points
                  //  outside of the slice.  If we aren't slicing, display them all.

                  if (!misc.slice || !trans) map->setDataPoints (data.x[i], data.y[i], -z_value, tmp, 0, NVFalse);


                  //  Get the min and max displayed Z values for each bin.

                  if (prev_xcoord != -1 && (data.xcoord[i] != prev_xcoord || data.ycoord[i] != prev_ycoord))
                    {
                      if (min_z_index >= 0)
                        {
                          map->setSparsePoints (data.x[min_z_index], data.y[min_z_index], -data.z[min_z_index], save_min, 0, NVFalse);


                          //  If we're slicing and we need sparse data for zoom and rotate, only load points inside the slice.

                          if (misc.slice && trans) map->setDataPoints (data.x[min_z_index], data.y[min_z_index], -data.z[min_z_index], tmp, 0, NVFalse);
                        }

                      if (max_z_index >= 0 && max_z_index != min_z_index)
                        {
                          map->setSparsePoints (data.x[max_z_index], data.y[max_z_index], -data.z[max_z_index], save_max, 0, NVFalse);


                          //  If we're slicing and we need sparse data for zoom and rotate, only load points inside the slice.

                          if (misc.slice && trans) map->setDataPoints (data.x[max_z_index], data.y[max_z_index], -data.z[max_z_index], tmp, 0, NVFalse);
                        }

                      min_z_index = max_z_index = -1;
                      min_z = 999999999.0;
                      max_z = -999999999.0;
                    }


                  //  Don't use null points to determine min and max values for the  bin.

                  if (data.val[i] && data.z[i] < min_z)
                    {
                      min_z = data.z[i];
                      min_z_index = i;
                      memcpy (save_min, tmp, 4);
                    }

                  if (data.val[i] && data.z[i] > max_z)
                    {
                      max_z = data.z[i];
                      max_z_index = i;
                      memcpy (save_max, tmp, 4);
                    }

                  prev_xcoord = data.xcoord[i];
                  prev_ycoord = data.ycoord[i];
                }


              //  We don't need sparse data for rotating or zooming so we set all of the data points.

              else
                {
                  map->setDataPoints (data.x[i], data.y[i], -z_value, tmp, 0, NVFalse);
                }
            }
        }


      //  If we have more than sparse_limit points we want to load a sparse layer for rotating and zooming.

      if (misc.need_sparse)
        {
          if (min_z_index >= 0)
            {
              map->setSparsePoints (data.x[min_z_index], data.y[min_z_index], -data.z[min_z_index], save_min, 0, NVFalse);

              if (misc.slice) map->setDataPoints (data.x[min_z_index], data.y[min_z_index], -data.z[min_z_index], save_min, 0, NVFalse);
            }

          if (max_z_index >= 0 && max_z_index != min_z_index)
            {
              map->setSparsePoints (data.x[max_z_index], data.y[max_z_index], -data.z[max_z_index], save_max, 0, NVFalse);

              if (misc.slice) map->setDataPoints (data.x[max_z_index], data.y[max_z_index], -data.z[max_z_index], save_max, 0, NVFalse);
            }

          map->setSparsePoints (0.0, 0.0, 0.0, save_min, options.point_size, NVTrue);
        }


      map->setDataPoints (0.0, 0.0, 0.0, save_min, options.point_size, NVTrue);
    }


  if (!misc.drawing_canceled) overlayFlag (map, &options, &misc, &data);


  //  We want to flush the map just in case we didn't have any points to draw.

  map->flush ();


  //  Redraw the 2D tracker unless this redraw was caused by an operation that has no effect on the 2D map.

  if (redraw2D) trackMap->redrawMap (NVTrue);


  misc.drawing = NVFalse;

  setMainButtons (NVTrue);

  setFunctionCursor (options.function);

  qApp->restoreOverrideCursor ();
}



void 
gridPointEdit3D::slotMaskReset ()
{
  //  Clear the "show" and "hide" buttons

  disconnect (showGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotShowMenu (QAction *)));
  for (int32_t i = 0 ; i < 5 ; i++) showAct[i]->setChecked (false);
  connect (showGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotShowMenu (QAction *)));

  disconnect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));
  for (int32_t i = 0 ; i < 5 ; i++)
    {
      hideAct[i]->setChecked (false);
      hideAct[i]->setEnabled (true);
    }
  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  for (int32_t i = 0 ; i < data.count ; i++) data.mask[i] = NVFalse;
  misc.mask_active = NVFalse;
  bMaskReset->setIcon (QIcon (":/icons/mask_reset.png"));


  //  Turn off slicing if it was on (this will also call redrawMap ()).

  slotPopupMenu (popup[NUMPOPUPS - 1]);


  redrawMap (NVTrue);
}



void 
gridPointEdit3D::slotReset ()
{
  misc.poly_count = 0;

  misc.slice = NVFalse;

  misc.slice_min = misc.ortho_min;
  sliceBar->setValue (misc.ortho_min);


  //  Turn on depth testing in the map widget

  map->setDepthTest (NVTrue);


  discardMovableObjects ();


  map->setMapCenter (misc.map_center_x, misc.map_center_y, misc.map_center_z);


  map->resetPOV ();


  redrawMap (NVTrue);
}



void
gridPointEdit3D::slotInterpolated ()
{
  if (bInterpolated->isChecked ())
    {
      options.display_interpolated = NVTrue;
    }
  else
    {
      options.display_interpolated = NVFalse;
    }

  redrawMap (NVTrue);
}



void 
gridPointEdit3D::slotSliceTriggered (int action)
{
  if (!misc.drawing)
    {
      //  Have to compute the ortho values and set the slider to minimum before we slice for the first time.

      if (!misc.slice) compute_ortho_values (map, &misc, &options, &data, sliceBar, NVTrue);


      misc.slice = NVTrue;
      map->setDepthTest (NVFalse);

      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
        case QAbstractSlider::SliderPageStepAdd:
          misc.slice_min -= misc.slice_size;
          misc.slice_max = misc.slice_min + misc.slice_size;
          break;

        case QAbstractSlider::SliderSingleStepSub:
        case QAbstractSlider::SliderPageStepSub:
          misc.slice_max += misc.slice_size;
          misc.slice_min = misc.slice_max - misc.slice_size;
          break;

        case QAbstractSlider::SliderMove:
          misc.slice_min = sliceBar->value ();
          misc.slice_max = misc.slice_min + misc.slice_size;
          break;
        }


      if (misc.slice_min < misc.ortho_min)
        {
          misc.slice_min = misc.ortho_min;
          misc.slice_max = misc.ortho_min + misc.slice_size;
        }

      if (action != QAbstractSlider::SliderMove) sliceBar->setValue (misc.slice_min);

      slicing = NVTrue;
      redrawMap (NVFalse);
      slicing = NVFalse;
    }
}



void 
gridPointEdit3D::slotSizeTriggered (int action)
{
  if (!misc.drawing)
    {
      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
        case QAbstractSlider::SliderPageStepAdd:
          options.slice_percent--;
          if (options.slice_percent <= 0) options.slice_percent = 1;
          break;

        case QAbstractSlider::SliderSingleStepSub:
        case QAbstractSlider::SliderPageStepSub:
          options.slice_percent++;
          if (options.slice_percent > MAX_SLICE_SIZE) options.slice_percent = MAX_SLICE_SIZE;
          break;

        case QAbstractSlider::SliderMove:
          options.slice_percent = sizeBar->value ();
          break;
        }


      if (action != QAbstractSlider::SliderMove) sizeBar->setValue (options.slice_percent);


      int32_t val = sliceBar->value ();

      sizeLabel->setNum (options.slice_percent);

      compute_ortho_values (map, &misc, &options, &data, sliceBar, NVFalse);
      map->setDepthTest (NVFalse);

      misc.slice_min = val;
      misc.slice_max = misc.slice_min + misc.slice_size;

      if (misc.slice_max > misc.ortho_max)
        {
          misc.slice_min = misc.ortho_max - misc.slice_size;
          misc.slice_max = misc.ortho_max;
        }

      slicing = NVTrue;
      redrawMap (NVFalse);
      slicing = NVFalse;
    }
}



void 
gridPointEdit3D::slotTransTriggered (int action)
{
  if (!misc.drawing)
    {
      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
        case QAbstractSlider::SliderPageStepAdd:
          options.slice_alpha--;
          if (options.slice_alpha < 0) options.slice_alpha = 1;
          break;

        case QAbstractSlider::SliderSingleStepSub:
        case QAbstractSlider::SliderPageStepSub:
          options.slice_alpha++;
          if (options.slice_alpha > MAX_TRANS_VALUE) options.slice_alpha = MAX_TRANS_VALUE;
          break;

        case QAbstractSlider::SliderMove:
          options.slice_alpha = transBar->value ();
          break;
        }


      if (action != QAbstractSlider::SliderMove) transBar->setValue (options.slice_alpha);


      //  Set the semi-transparent colors for slicing.

      float hue_inc = (float) (misc.max_hsv_color - misc.min_hsv_color) / (float) (NUMSHADES + 1);

      for (int32_t j = 0 ; j < NUMSHADES ; j++)
	{
	  QColor color;
	  color.setHsv ((int32_t) ((j - misc.min_hsv_color) * hue_inc), 255, 255, 255);
	  misc.color_array[0][j][0] = misc.color_array[1][j][0] = color.red ();
	  misc.color_array[0][j][1] = misc.color_array[1][j][1] = color.green ();
	  misc.color_array[0][j][2] = misc.color_array[1][j][2] = color.blue ();
	  misc.color_array[0][j][3] = color.alpha ();

	  misc.color_array[1][j][3] = options.slice_alpha;
	}
      options.ref_color[1] = options.ref_color[0];
      options.ref_color[1].setAlpha (options.slice_alpha);


      QString string;
      string.sprintf ("%02d", options.slice_alpha);
      transLabel->setText (string);


      redrawMap (NVFalse);
    }
}



void 
gridPointEdit3D::slotExagTriggered (int action)
{
  if (!misc.drawing)
    {
      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
          if (options.exaggeration >= 1.0)
            {
              options.exaggeration -= 1.0;

              if (options.exaggeration < 1.0) options.exaggeration = 1.0;

              map->setExaggeration (options.exaggeration);

              redrawMap (NVFalse);
            }
          break;

        case QAbstractSlider::SliderPageStepAdd:
          if (options.exaggeration >= 5.0)
            {
              options.exaggeration -= 5.0;

              if (options.exaggeration < 1.0) options.exaggeration = 1.0;

              map->setExaggeration (options.exaggeration);

              redrawMap (NVFalse);
            }
          break;

        case QAbstractSlider::SliderSingleStepSub:
          options.exaggeration += 1.0;
          if (options.exaggeration > 100.0) options.exaggeration = 100.0;
          map->setExaggeration (options.exaggeration);

          redrawMap (NVFalse);

          break;

        case QAbstractSlider::SliderPageStepSub:
          options.exaggeration += 5.0;
          if (options.exaggeration > 100.0) options.exaggeration = 100.0;
          map->setExaggeration (options.exaggeration);

          redrawMap (NVFalse);

          break;

        case QAbstractSlider::SliderMove:
          QString lbl;
          lbl.sprintf (tr ("%5.3f").toLatin1 (), (float) exagBar->value () / 100.0);
          exagLabel->setText (lbl);
          break;
        }
    }
}



void 
gridPointEdit3D::slotExagReleased ()
{
  options.exaggeration = (float) exagBar->value () / 100.0;
  map->setExaggeration (options.exaggeration);

  redrawMap (NVFalse);
}



void 
gridPointEdit3D::slotExaggerationChanged (float exaggeration, float apparent_exaggeration)
{
  if (exaggeration < 1.0)
    {
      exagBar->setEnabled (false);
    }
  else
    {
      exagBar->setEnabled (true);
    }

  QString lbl;
  lbl.sprintf (tr ("%5.3f").toLatin1 (), apparent_exaggeration);
  exagLabel->setText (lbl);

  options.exaggeration = exaggeration;

  disconnect (exagBar, SIGNAL (actionTriggered (int)), this, SLOT (slotExagTriggered (int)));
  exagBar->setValue (NINT (options.exaggeration * 100.0));
  connect (exagBar, SIGNAL (actionTriggered (int)), this, SLOT (slotExagTriggered (int)));
}



void
gridPointEdit3D::keyPressEvent (QKeyEvent *e)
{
  void actionKey (gridPointEdit3D *parent, nvMapGL *map, POINT_DATA *data, MISC *misc, QString key, int32_t lock_point);


  //  Lock in nearest point so mouse moves won't change it.

  lock_point = misc.nearest_point;


  //  If we're drawing and a key is pressed we want to cancel drawing.

  if (misc.drawing)
    {
      misc.drawing_canceled = NVTrue;
      return;
    }


  //  First check for the hard-wired keys.

  switch (e->key ())
    {
      //  Looking for the return or enter key when we have highlighted points.

    case Qt::Key_Enter:
    case Qt::Key_Return:

      //  If we have highlighted points, kill them.

      if (misc.highlight_count)
        {
          for (int32_t i = 0 ; i < misc.highlight_count ; i++)
            {
              int32_t j = misc.highlight[i];

              store_undo (&misc, options.undo_levels, data.val[j], j);
              data.val[j] = CHRTR2_NULL;
            }
          end_undo_block (&misc);

          misc.highlight.clear ();
          misc.highlight_count = 0;

          options.function = misc.save_function;

          redrawMap (NVTrue);
          return;
        }
      break;


    case Qt::Key_Escape:

      //  Turn off slicing if it was on (this will also call redrawMap ()).

      if (misc.slice) slotPopupMenu (popup[NUMPOPUPS - 1]);

      return;
      break;

    case Qt::Key_Left:
      map->rotateY (-options.rotation_increment);
      return;
      break;

    case Qt::Key_PageUp:
      options.exaggeration += 1.0;
      if (options.exaggeration > 100.0) options.exaggeration = 100.0;
      map->setExaggeration (options.exaggeration);


      //  Redraw the data (which is needed because we changed the exaggeration).

      redrawMap (NVFalse);

      return;
      break;

    case Qt::Key_Right:
      map->rotateY (options.rotation_increment);
      return;
      break;

    case Qt::Key_PageDown:
      options.exaggeration -= 1.0;
      if (options.exaggeration < 1.0) options.exaggeration = 1.0; 
      map->setExaggeration (options.exaggeration);


      //  Redraw the data (which is needed because we changed the exaggeration).

      redrawMap (NVFalse);

      return;
      break;

    case Qt::Key_Up:

      if (e->modifiers () == Qt::ControlModifier)
        {
          map->rotateZX (-options.rotation_increment);
        }
      else
        {
          slotSliceTriggered (QAbstractSlider::SliderSingleStepSub);
        }
      return;
      break;

    case Qt::Key_Down:

      if (e->modifiers () == Qt::ControlModifier)
        {
          map->rotateZX (options.rotation_increment);
        }
      else
        {
          //  If we're not already slicing, do nothing.

          if (misc.slice) slotSliceTriggered (QAbstractSlider::SliderSingleStepAdd);
        }
      return;
      break;
    }


  e->accept ();
}



void 
gridPointEdit3D::slotRedrawMap ()
{
  redrawMap (NVTrue);
}



void 
gridPointEdit3D::closeEvent (QCloseEvent *event __attribute__ ((unused)))
{
  int32_t ret = QMessageBox::Yes;
  ret = QMessageBox::information (this, tr ("gridPointEdit3D"), tr ("Do you wish to save any changes you may have made?"),
                                  QMessageBox::Yes,  QMessageBox::No,  QMessageBox::NoButton);

  if (ret == QMessageBox::Yes)
    {
      slotExit (0);
    }
  else
    {
      slotExit (2);
    }
}



//  A bunch of slots.

void 
gridPointEdit3D::slotExitSave ()
{
  slotExit (0);
}



void 
gridPointEdit3D::slotExitMask ()
{
  slotExit (1);
}



void 
gridPointEdit3D::slotExitNoSave ()
{
  slotExit (2);
}



void 
gridPointEdit3D::slotExit (int id)
{
  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  //  Clear up any undo memory we've used.

  for (int32_t i = 0 ; i < misc.undo_count ; i++)
    {
      if (misc.undo[i].count)
        {
          misc.undo[i].val.clear ();
          misc.undo[i].num.clear ();
        }
    }


  //  Clear up any highlight memory we had.

  if (misc.highlight_count)
    {
      misc.highlight.clear ();
      misc.highlight_count = 0;
    }


  //  Make sure the default function is one of the delete functions

  options.function = misc.save_mode;


  //  Get the last rotation values

  options.zx_rotation = map->getZXRotation ();
  options.y_rotation = map->getYRotation ();


  //  Save the user's defaults

  envout (&options, &misc, this);


  //  We output a 0 if no changes have been made or a 1 if changes to the CHRTR2 structure have been made.  The second value (after the comma)
  //  is the filtered flag.  This tells gridEdit that it needs to filter mask the area.

  int32_t put_ret = 0;

  switch (id)
    {
      //  Normal exit and save.

    case 0:
      map->setCursor (Qt::WaitCursor);

      put_ret = put_buffer (&data, &misc);

      if (put_ret)
        {
          fprintf (stdout, "Edit return status:1,0\n");
        }
      else
        {
          fprintf (stdout, "Edit return status:0,0\n");
        }
      fflush (stdout);
      break;


    case 1:
      map->setCursor (Qt::WaitCursor);

      put_ret = put_buffer (&data, &misc);

      if (put_ret)
        {
          fprintf (stdout, "Edit return status:1,1\n");
        }
      else
        {
          fprintf (stdout, "Edit return status:0,0\n");
        }
      fflush (stdout);
      break;


      //  Exit without saving

    case 2:
      fprintf (stdout, "Edit return status:0,0\n");
      fflush (stdout);
      break;
    }


  for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++) chrtr2_close_file (misc.chrtr2_handle[chrtr2]);


  //  Let go of the shared memory.

  misc.chrtr2Share->detach ();


  clean_exit (0);
}



void 
gridPointEdit3D::slotPrefs ()
{
  if (prefs_dialog) prefs_dialog->close ();

  prefs_dialog = new Prefs (this, &options, &misc, &data);


  //  Put the dialog in the middle of the screen.

  prefs_dialog->move (x () + width () / 2 - prefs_dialog->width () / 2, y () + height () / 2 - prefs_dialog->height () / 2);

  connect (prefs_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotPrefDataChanged ()));
  connect (prefs_dialog, SIGNAL (hotKeyChangedSignal (int32_t)), this, SLOT (slotPrefHotKeyChanged (int32_t)));
}



//  Changed some of the preferences

void 
gridPointEdit3D::slotPrefDataChanged ()
{
  bContour->setChecked (options.display_contours);

  if (options.function == DELETE_POINT) bDeletePoint->setChecked (true);
  if (options.function == DELETE_RECTANGLE) bDeleteRect->setChecked (true);
  if (options.function == DELETE_POLYGON) bDeletePoly->setChecked (true);
  if (options.function == KEEP_POLYGON) bKeepPoly->setChecked (true);


  //  Set the semi-transparent colors for slicing.

  float hue_inc = (float) (misc.max_hsv_color - misc.min_hsv_color) / (float) (NUMSHADES + 1);

  for (int32_t j = 0 ; j < NUMSHADES ; j++) 
    {
      QColor color;
      color.setHsv ((int32_t) ((j  - misc.min_hsv_color) * hue_inc), 255, 255, options.slice_alpha);

      misc.color_array[1][j][0] = color.red ();
      misc.color_array[1][j][0] = color.green ();
      misc.color_array[1][j][0] = color.blue ();
      misc.color_array[1][j][0] = options.slice_alpha;
    }

  map->setBackgroundColor (options.background_color);
  map->setScaleColor (options.scale_color);
  map->enableScale (options.draw_scale);
  map->setZoomPercent (options.zoom_percent);

  slicePalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  sliceBar->setPalette (slicePalette);


  for (int32_t i = 0 ; i < HOTKEYS ; i++) slotPrefHotKeyChanged (i);


  //  Save the user's defaults

  envout (&options, &misc, this);


  //  Redraw the data (which is needed because we may have changed the exaggeration).

  if (!misc.drawing) redrawMap (NVTrue);
}



void 
gridPointEdit3D::slotPrefHotKeyChanged (int32_t i)
{
  QString tip = misc.buttonText[i] + "  [" + options.buttonAccel[i] + "]";
  misc.button[i]->setToolTip (tip);
  misc.button[i]->setShortcut (QKeySequence (options.buttonAccel[i]));
}



void 
gridPointEdit3D::slotClearHighlight ()
{
  if (misc.highlight_count)
    {
      misc.highlight.clear ();
      misc.highlight_count = 0;
    }

  bClearHighlight->setEnabled (false);
  redrawMap (NVFalse);
}



void
gridPointEdit3D::slotContour ()
{
  if (bContour->isChecked ())
    {
      options.display_contours = NVTrue;
    }
  else
    {
      options.display_contours = NVFalse;
    }

  redrawMap (NVTrue);
}



void 
gridPointEdit3D::slotMode (int id)
{
  misc.save_function = options.function;
  options.function = id;
  setFunctionCursor (options.function);


  switch (options.function)
    {
      //  Kill any flags that may be on so we won't sit there wondering why we're not highlighting stuff.  DOH!!!

    case HIGHLIGHT_POLYGON:
      slotFlagMenu (flag[0]);
      break;


      //  Save the last edit mode in case we exit

    case DELETE_RECTANGLE:
    case DELETE_POLYGON:
    case DELETE_POINT:
    case KEEP_POLYGON:
      misc.save_mode = options.function;
      break;
    }
}



void
gridPointEdit3D::slotStop ()
{
  misc.drawing_canceled = NVTrue;
}



void
gridPointEdit3D::slotFlagMenu (QAction *action)
{
  for (int32_t i = 0 ; i < PRE_USER + 5 ; i++)
    {
      if (action == flag[i])
        {
          options.flag_index = i;
          break;
        }
    }


  bFlag->setIcon (flagIcon[options.flag_index]);

  redrawMap (NVFalse);
}



void
gridPointEdit3D::slotHideMenu (QAction *action __attribute__ ((unused)))
{
  //  Clear the "show" buttons that match the "hide" buttons selected.

  disconnect (showGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotShowMenu (QAction *)));
  for (int32_t i = 0 ; i < 5 ; i++)
    {
      if (hideAct[i]->isChecked ()) showAct[i]->setChecked (false);
    }
  connect (showGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotShowMenu (QAction *)));


  //  IMPORTANT NOTE: Once we hide something we can't get it back until we reset or click a show button.
  //  This is so you can combine data type/flag hiding with masking.

  uint32_t flags[5] = {CHRTR2_USER_01, CHRTR2_USER_02, CHRTR2_USER_03, CHRTR2_USER_04, CHRTR2_LAND_MASK};
  uint8_t hide_flag[5];

  for (int32_t i = 0 ; i < 5 ; i++)
    {
      if (hideAct[i]->isChecked ())
	{
	  hide_flag[i] = NVTrue;
          hideAct[i]->setEnabled (false);
	}
      else
	{
	  hide_flag[i] = NVFalse;
	}
    }


  //  Use data.mask to hide the data

  for (int32_t i = 0 ; i < data.count ; i++) 
    {
      for (int32_t j = 0 ; j < 5 ; j++)
	{
	  if (hide_flag[j])
	    {
	      if (data.val[i] & flags[j]) data.mask[i] = NVTrue;
	    }
	}
    }

  misc.mask_active = NVTrue;


  redrawMap (NVFalse);
}



void
gridPointEdit3D::slotShowMenu (QAction *action __attribute__ ((unused)))
{
  //  Clear the "hide" buttons

  disconnect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));
  for (int32_t i = 0 ; i < 5 ; i++)
    {
      hideAct[i]->setChecked (false);
      hideAct[i]->setEnabled (true);
    }
  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));


  uint32_t flags[5] = {CHRTR2_USER_01, CHRTR2_USER_02, CHRTR2_USER_03, CHRTR2_USER_04, CHRTR2_LAND_MASK};
  uint8_t show_flag[5];

  for (int32_t i = 0 ; i < 5 ; i++)
    {
      if (showAct[i]->isChecked ())
	{
	  show_flag[i] = NVTrue;
	}
      else
	{
	  show_flag[i] = NVFalse;
	}
    }


  //  Use data.mask to show the data

  for (int32_t i = 0 ; i < data.count ; i++) 
    {
      uint8_t hit = NVFalse;
      for (int32_t j = 0 ; j < 5 ; j++)
	{
	  if (show_flag[j] && (data.val[i] & flags[j]))
	    {
	      hit = NVTrue;
	      break;
	    }
	}

      if (hit)
	{
	  data.mask[i] = NVFalse;
	}
      else
	{
	  data.mask[i] = NVTrue;
	}
    }


  misc.mask_active = NVTrue;


  redrawMap (NVFalse);
}



void 
gridPointEdit3D::slotLayers ()
{
  manageLayers_dialog = new manageLayers (this, &options, &misc);

  connect (manageLayers_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotManageLayersDataChanged ()));
}



void 
gridPointEdit3D::slotManageLayersDataChanged ()
{
  misc.chrtr2_share->settings_changed = NVTrue;

  if (!misc.drawing) redrawMap (NVTrue);
}



void
gridPointEdit3D::slotToolbarHelp ()
{
  QWhatsThis::showText (QCursor::pos ( ), toolbarText, map);
}


//  Undo the last edit operation.

void 
gridPointEdit3D::slotUndo ()
{
  undo (&misc, &data);

  redrawMap (NVTrue);
}



//  2D track map slots

//  Mouse press signal from the map class.

void 
gridPointEdit3D::slotTrackMousePress (QMouseEvent *e __attribute__ ((unused)), double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  if (e->button () == Qt::LeftButton) {};
  if (e->button () == Qt::MidButton) {};
  if (e->button () == Qt::RightButton) {};
}



//  Mouse release signal from the map class.  Right now we don't really need this.

void 
gridPointEdit3D::slotTrackMouseRelease (QMouseEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  if (e->button () == Qt::LeftButton) {};
  if (e->button () == Qt::MidButton) {};
  if (e->button () == Qt::RightButton) {};
}



//  Mouse move signal from the map class.

void
gridPointEdit3D::slotTrackMouseMove (QMouseEvent *e __attribute__ ((unused)), double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  char                      ltstring[25], lnstring[25], hem;
  double                    deg, min, sec;
  NV_F64_COORD2             xy;
  QString                   y_string, x_string;
  CHRTR2_RECORD             chrtr2_record;
  double                    dist;
  static int32_t            prev_nearest_point = -1;




  //  Let other CHRTR2 programs know which window we're in.  This is a special case since we have two map windows here.  We're lying about the 
  //  active window ID (process ID) by subtracting 1 from this program's PID.  This makes trackCursor track it locally.

  misc.chrtr2_share->active_window_id = misc.process_id - 1;


  //  Track the cursor position for other CHRTR2 programs

  misc.chrtr2_share->cursor_position.y = lat;
  misc.chrtr2_share->cursor_position.x = lon;


  //  Try to find the highest layer with a valid value to display in the status bar.

  xy.y = lat;
  xy.x = lon;
  int32_t hit = -1;

  for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (misc.chrtr2_share->display_chrtr2[chrtr2] && inside_mbr_coord2 (&misc.chrtr2_share->chrtr2_header[chrtr2].mbr, xy))
        {
          chrtr2_read_record_lat_lon (misc.chrtr2_handle[chrtr2], lat, lon, &chrtr2_record);

          hit = chrtr2;
          break;
        }
    }


  //  We only change the status CHRTR2 values if we were actually over a CHRTR2 data area

  if (hit >= 0) chrtr2Label->setText (QFileInfo (misc.chrtr2_share->chrtr2_file[hit]).fileName ().remove (".ch2"));


  misc.nearest_point = -1;


  double min_dist = 9999999999.0;
  for (int32_t i = 0 ; i < data.count ; i++)
    {
      //  Do not use null points.  Do not use masked points. Do not check points that are not on the display.

      if (!check_bounds (&options, &misc, &data, i, misc.slice, NVFalse))
        {
          dist = sqrt ((double) ((lat - data.y[i]) * (lat - data.y[i])) + (double) ((lon - data.x[i]) * (lon - data.x[i])));


          if (dist < min_dist)
            {
              min_dist = dist;

              misc.nearest_point = i;
            }
        }
    }


  QString chrtr2_string = "", distance_string = "", hv_string = "";


  //  Update the status bars

  // 
  //                      if our point has moved or we have just selected a new overplot we will go
  //                      into this code block.
  //

  if (misc.nearest_point != -1 && misc.nearest_point != prev_nearest_point)
    {
      chrtr2_string = QFileInfo (QString (misc.chrtr2_share->chrtr2_file[data.chrtr2[misc.nearest_point]])).fileName ().remove (".ch2");
      chrtr2Label->setToolTip (QString (misc.chrtr2_share->chrtr2_file[data.chrtr2[misc.nearest_point]]));

      chrtr2Label->setText (chrtr2_string);

      hv_string.sprintf ("%.2f / %.2f", data.herr[misc.nearest_point], data.verr[misc.nearest_point]);
      hvLabel->setText (hv_string);


      prev_nearest_point = misc.nearest_point;


      strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
      strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));

      latLabel->setText (ltstring);
      lonLabel->setText (lnstring);


      NV_I32_COORD2 coord;
      chrtr2_get_coord (misc.chrtr2_handle[0], lat, lon, &coord);

      coordXLabel->setNum (coord.x);
      coordYLabel->setNum (coord.y);
    }
}



//  Resize signal from the map class.

void
gridPointEdit3D::slotTrackResize (QResizeEvent *e __attribute__ ((unused)))
{
}



//  Prior to drawing coastline signal from the map class.  This is where we do our heavy lifting.

void 
gridPointEdit3D::slotTrackPreRedraw (NVMAP_DEF l_mapdef __attribute__ ((unused)))
{
  void scribe (nvMap *map, OPTIONS *options, MISC *misc, float *ar, int32_t ncc, int32_t nrr, double xorig, double yorig);
  void scribe3D (nvMapGL *map, OPTIONS *options, MISC *misc, float *ar, int32_t ncc, int32_t nrr, double xorig, double yorig);


  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  uint8_t small_enough = NVTrue;
  uint8_t big_enough = NVTrue;


  int32_t grid_height = NINT ((misc.bounds.max_y - misc.bounds.min_y) / misc.y_grid_size);
  int32_t grid_width = NINT ((misc.bounds.max_x - misc.bounds.min_x) / misc.x_grid_size);


  //  Compute the adjusted min y and min x for the grid indexing.  This makes the grid an 
  //  exact number of grid spaces wide and high.

  double adj_rymin = misc.bounds.max_y - ((float) grid_height * misc.y_grid_size);
  double adj_rxmin = misc.bounds.max_x - ((float) grid_width * misc.x_grid_size);


  //  If the area to be binned is too big or small, flag it.

  if (grid_width * grid_height > 1000000) small_enough = NVFalse;
  if (grid_width * grid_height <= 0) big_enough = NVFalse;


  float *grid = NULL;
  uint16_t *count = NULL;
  int32_t length;

  if (options.display_contours && small_enough && big_enough)
    {
      grid = (float *) malloc (sizeof (float) * grid_height * grid_width);

      if (grid == NULL)
        {
          perror ("Allocating grid memory : ");
          exit (0);
        }


      count = (uint16_t *) malloc (sizeof (uint16_t) * grid_height * grid_width);

      if (count == NULL)
        {
          perror ("Allocating count memory : ");
          exit (0);
        }

      for (int32_t i = 0 ; i < grid_height * grid_width ; i++)
        {
          count[i] = 0;
          grid[i] = CHRTRNULL;
        }
    }


  for (int32_t i = 0 ; i < data.count ; i++)
    {
      int32_t xyz_x, xyz_y, xyz_z;
      double dummy;
      if (data.val[i])
        {
          //  Check against the viewing area.

          int32_t trans = 0;
          if ((trans = check_bounds (&options, &misc, data.x[i], data.y[i], data.val[i], NVFalse, data.chrtr2[i], NVFalse, NVFalse)) < 2)
            {
              int32_t c_index = NINT ((NUMSHADES - 1) - ((-data.z[i]) - color_min_z) / color_range_z * (float) (NUMSHADES - 1));


              //  Check for out of range data.

              if (c_index < 0) c_index = 0;
              if (c_index > NUMSHADES - 1) c_index = NUMSHADES - 1;


              QColor tmp = misc.track_color_array[c_index];

              trackMap->map_to_screen (1, &data.x[i], &data.y[i], &dummy, &xyz_x, &xyz_y, &xyz_z);
              trackMap->fillRectangle (xyz_x, xyz_y, 2, 2, tmp, NVFalse);


              if (big_enough && small_enough && options.display_contours)
                {
                  //  Load the grid (figure out which bin the point is in).

                  c_index = NINT ((int32_t) ((data.y[i] - adj_rymin) / misc.y_grid_size) * grid_width + (int32_t) ((data.x[i] - adj_rxmin) /
                                                                                                                     misc.x_grid_size));


                  //  Make sure there is no memory overrun.

                  if (c_index < grid_width * grid_height)
                    {
                      if (!count[c_index]) grid[c_index] = 0.0;
                      grid[c_index] = (grid[c_index] * (float) count[c_index] + (data.z[i] * options.z_factor + options.z_offset)) /
                        ((float) count[c_index] + 1.0);
                      count[c_index]++;
                    }
                }
            }
        }
    }



  //  Plot contours on 2D tracker if requested.

  if (big_enough && small_enough && options.display_contours)
    {
      float *spline_array_x_in = (float *) malloc (sizeof (float) * grid_width);
      float *spline_array_y_in = (float *) malloc (sizeof (float) * grid_width);
      float *spline_array_x_out = (float *) malloc (sizeof (float) * grid_width);
      float *spline_array_y_out = (float *) malloc (sizeof (float) * grid_width);
      if (spline_array_x_in == NULL || spline_array_y_in == NULL ||
          spline_array_x_out == NULL || spline_array_y_out == NULL)
        {
          perror ("Allocating spline memory : ");
          exit (0);
        }


      //  Fill the spline arrays

      int32_t k;
      for (int32_t i = 0 ; i < grid_height ; i++)
        {
          k = 0;
          for (int32_t j = 0 ; j < grid_width ; j++)
            {
              if (count[i * grid_width + j])
                {
                  //  Make sure there is no memory overrun.

                  if (k < grid_width)
                    {
                      spline_array_x_in[k] = (float) j;
                      spline_array_y_in[k] = grid[i * grid_width + j];
                      k++;
                    }
                }
            }


          //  Do not interpolate lines with fewer than 3 points so the slope doesn't go wacko.
                        
          if (k > 2)
            {
              interpolate (1.0, k, 0.0, grid_width - 1, &length, spline_array_x_in, spline_array_y_in, spline_array_x_out, spline_array_y_out);


              //  Set all points that do not have a 'real' neighbor within 3 bins to be CHRTRNULL.  
              //  This way they won't be contoured.

              uint8_t good_flag;
              for (int32_t j = 0 ; j < length ; j++)
                {
                  good_flag = NVFalse;
                  for (int32_t m = 0 ; m < k ; m++)
                    {
                      if (ABS (j - spline_array_x_in[m]) < 3)
                        {
                          good_flag = NVTrue;
                          break;
                        }
                    }

                  if (good_flag)
                    {
                      grid[i * grid_width + j] = spline_array_y_out[j];
                    }
                  else
                    {
                      grid[i * grid_width + j] = CHRTRNULL;
                    }
                }
            }
        }


      //  Contour it

      scribe3D (map, &options, &misc, grid, grid_width, grid_height, adj_rxmin, adj_rymin);
      scribe (trackMap, &options, &misc, grid, grid_width, grid_height, adj_rxmin, adj_rymin);


      //  Free all of the memory.

      if (grid != NULL) free (grid);
      if (count != NULL) free (count);
      if (spline_array_x_in != NULL) free (spline_array_x_in);
      if (spline_array_y_in != NULL) free (spline_array_y_in);
      if (spline_array_x_out != NULL) free (spline_array_x_out);
      if (spline_array_y_out != NULL) free (spline_array_y_out);
    }

  trackMap->flush ();
}



//  Post grid drawing signal from map class.  Mostly just cleanup work.

void 
gridPointEdit3D::slotTrackPostRedraw (NVMAP_DEF mapdef __attribute__ ((unused)))
{
  qApp->restoreOverrideCursor ();
}



//  Clean exit routine

void 
gridPointEdit3D::clean_exit (int32_t ret)
{
  //  Have to close the GL widget or it stays on screen in VirtualBox

  map->close ();

  exit (ret);
}



void
gridPointEdit3D::about ()
{
  QMessageBox msgBox;
  msgBox.setText (QString ("<center>") + VERSION + QString ("</center>"));
  msgBox.setInformativeText (gridPointEdit3DAboutText);
  msgBox.setStandardButtons (QMessageBox::Ok);
  msgBox.setDefaultButton (QMessageBox::Ok);
  msgBox.setIconPixmap (QPixmap (":/icons/gridPointEdit3D.jpg"));
  msgBox.exec ();
}



void
gridPointEdit3D::slotAcknowledgments ()
{
  QMessageBox::about (this, VERSION, acknowledgmentsText);
}



void
gridPointEdit3D::aboutQt ()
{
  QMessageBox::aboutQt (this, VERSION);
}
