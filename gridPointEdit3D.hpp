
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



/*  gridPointEdit3D class definitions.  */

#ifndef GRID_POINT_EDIT_3D_H
#define GRID_POINT_EDIT_3D_H

#include "gridPointEdit3DDef.hpp"
#include "prefs.hpp"
#include "version.hpp"

#include "manageLayers.hpp"


void get_buffer (POINT_DATA *data, MISC *misc);
int32_t put_buffer (POINT_DATA *data, MISC *misc);


class gridPointEdit3D:public QMainWindow
{
  Q_OBJECT 


 public:

  gridPointEdit3D (int *argc = 0, char **argv = 0, QWidget *parent = 0);
  ~gridPointEdit3D ();

  void redrawMap (uint8_t redraw2D);


 protected:

  POINT_DATA      data;

  OPTIONS         options;

  MISC            misc;

  Prefs           *prefs_dialog;

  manageLayers    *manageLayers_dialog;

  QMenu           *layerMenu;

  QPainterPath    tracker_2D;

  QTimer          *trackCursor, *blinkTimer;

  QLabel          *latLabel, *lonLabel, *coordXLabel, *coordYLabel, *zLabel, *chrtr2Label, *exagLabel, *hvLabel, *transLabel, *sizeLabel, *meterLabel;

  QScrollBar      *sliceBar, *transBar, *sizeBar, *exagBar;

  nvMapGL         *map;

  nvMap           *trackMap;

  NVMAPGL_DEF     mapdef;

  NVMAP_DEF       track_mapdef;

  QGroupBox       *colorScaleBox;

  QToolBar        *toolBar[NUM_TOOLBARS];

  QButtonGroup    *exitGrp;

  QActionGroup    *flagGrp, *hideGrp, *showGrp;

  QAction         *bHelp, *popup[NUMPOPUPS], *popupHelp, *saveExitAction, *maskExitAction, *flag[5 + PRE_USER], *hideAct[5], *showAct[5];

  QToolButton     *bExitSave, *bExitMask, *bExitNoSave, *bReset, *bInterpolated, *bMeasure, *bPrefs, *bStop, *bDeletePoint, *bDeleteRect, *bDeletePoly,
                  *bKeepPoly, *bUndo, *bFlag, *bHighlightPoly, *bClearPoly, *bClearHighlight, *bHide, *bShow, *bMaskInsideRect, *bMaskOutsideRect,
                  *bMaskInsidePoly, *bMaskOutsidePoly, *bMaskReset, *bContour;

  QIcon           flagIcon[5 + PRE_USER];

  QCursor         rotateCursor, zoomCursor, maskCursor, deleteRectCursor, deletePolyCursor, measureCursor, keepPolyCursor, highlightPolyCursor, clearPolyCursor;

  QTabWidget      *notebook;

  QMenu           *popupMenu;

  QPalette        zPalette, slicePalette, transPalette, sizePalette, exagPalette;

  QString         acknowledgmentsText;

  NV_I32_COORD2   marker[16];

  float           *save_z, color_min_z, color_max_z, color_range_z;

  uint8_t         polygon_flip, slicing, double_click, popup_active;

  int32_t         popup_prog[NUMPOPUPS], menu_cursor_x, menu_cursor_y, mv_tracker, mv_marker, rb_rectangle, rb_polygon, rb_measure, mv_measure,
                  mv_measure_anchor, mv_2D_tracker, active_window_id, lock_point, prev_poly_x, prev_poly_y, prev_icon_size, start_ctrl_x, start_ctrl_y,
                  rotate_index;

  double          menu_cursor_lat, menu_cursor_lon, menu_cursor_z, range_x, range_y, range_z;

  NV_F64_COORD3   line_anchor;


  void closeEvent (QCloseEvent *event);
  void setFunctionCursor (int32_t function);
  void setMainButtons (uint8_t enable);
  void geo_xyz_label (double lat, double lon, double z);

  void discardMovableObjects ();

  void midMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z);
  void leftMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z);
  void rightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z);
  void clean_exit (int32_t ret);


protected slots:

  void slotPopupMenu (QAction *ac);
  void slotPopupHelp ();

  void slotMouseDoubleClick (QMouseEvent *e, double lon, double lat, double z);
  void slotMousePress (QMouseEvent *e, double lon, double lat, double z);
  void slotMouseRelease (QMouseEvent *e, double lon, double lat, double z);
  void slotWheel (QWheelEvent *e, double lon, double lat, double z);
  void slotMouseMove (QMouseEvent *e, double lon, double lat, double z, NVMAPGL_DEF l_mapdef);
  void slotResize (QResizeEvent *e);

  void slotTrackMousePress (QMouseEvent *e, double lon, double lat);
  void slotTrackMouseRelease (QMouseEvent *e, double lon, double lat);
  void slotTrackMouseMove (QMouseEvent *e, double lon, double lat);
  void slotTrackResize (QResizeEvent *e);
  void slotTrackPreRedraw (NVMAP_DEF mapdef);
  void slotTrackPostRedraw (NVMAP_DEF mapdef);

  void slotTrackCursor ();
  void slotBlinkTimer ();

  void slotExitSave ();
  void slotExitMask ();
  void slotExitNoSave ();
  void slotExit (int id);

  void slotMaskReset ();
  void slotReset ();
  void slotInterpolated ();

  void slotSliceTriggered (int action);
  void slotSizeTriggered (int action);
  void slotTransTriggered (int action);

  void slotExagTriggered (int action);
  void slotExagReleased ();
  void slotExaggerationChanged (float exaggeration, float apparent_exaggeration);

  void slotClearHighlight ();

  void slotFlagMenu (QAction *action);

  void slotHideMenu (QAction *action);
  void slotShowMenu (QAction *action);

  void slotRedrawMap ();

  void slotMode (int id);

  void slotStop ();

  void slotContour ();

  void slotLayers ();
  void slotManageLayersDataChanged ();

  void slotToolbarHelp ();

  void slotUndo ();

  void slotPrefs ();
  void slotPrefDataChanged ();
  void slotPrefHotKeyChanged (int32_t i);

  void about ();
  void slotAcknowledgments ();
  void aboutQt ();


 private:

  void keyPressEvent (QKeyEvent *e);

};

#endif
