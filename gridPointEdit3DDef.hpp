
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



#ifndef _GRID_POINT_EDIT_3D_DEF_H_
#define _GRID_POINT_EDIT_3D_DEF_H_

#include "nvutility.h"
#include "nvutility.hpp"
#include "nvMapGL.hpp"

#include "chrtr2.h"
#include "chrtr2_shared.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gdal_priv.h"

#include <QtCore>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QSharedMemory>


#define         MARKER_W        15
#define         MARKER_H        10


//    Pointer interaction functions.

#define         NOOP                        0
#define         DELETE_RECTANGLE            1
#define         DELETE_POLYGON              2
#define         DELETE_POINT                3
#define         KEEP_POLYGON                4
#define         ROTATE                      5
#define         ZOOM                        6
#define         MASK_INSIDE_RECTANGLE       7
#define         MASK_OUTSIDE_RECTANGLE      8
#define         MASK_INSIDE_POLYGON         9
#define         MASK_OUTSIDE_POLYGON        10
#define         MEASURE                     11
#define         HIGHLIGHT_POLYGON           12
#define         CLEAR_POLYGON               13


#define         NUMSHADES                   100
#define         NUM_SCALE_LEVELS            16
#define         DEFAULT_SEGMENT_LENGTH      0.25
#define         CHRTRNULL                   10000000000000000.0
#define         EDIT_OPTIONS                7
#define         LINE_WIDTH                  2
#define         POLYGON_POINTS              NVMAPGL_POLYGON_POINTS
#define         NUM_TOOLBARS                4
#define         PRE_USER                    1
#define         MAX_SLICE_SIZE              50
#define         MAX_TRANS_VALUE             64
#define         CONTOUR_POINTS              1000


//  NUMPOPUPS should always be one more than you think you're using in rightMouse because the last one 
//  (i.e. [NUMPOPUPS - 1]) is used for turning off slicing.  You also have to modify slotPopupMenu
//  because it checks this value for debug purposes.

#define         NUMPOPUPS                   4


//  Button hotkeys that are editable by the user.  These are used as indexes into the options.buttonAccel, misc.buttonText,
//  misc.button, and misc.buttonIcon arrays.  To add or remove from this list you need to change it here, in set_defaults.cpp
//  and in gridPointEdit3D.cpp (look for the tool buttons that have one of these defined names attached).

#define         SAVE_EXIT_KEY               0
#define         SAVE_EXIT_MASK_KEY          1
#define         NO_SAVE_EXIT_KEY            2
#define         RESET_KEY                   3
#define         DELETE_POINT_MODE_KEY       4
#define         DELETE_RECTANGLE_MODE_KEY   5
#define         DELETE_POLYGON_MODE_KEY     6
#define         UNDO_KEY                    7
#define         CLEAR_HIGHLIGHT_KEY         8
#define         HIGHLIGHT_POLYGON_MODE_KEY  9
#define         CLEAR_POLYGON_MODE_KEY      10
#define         TOGGLE_CONTOUR_KEY          11

#define         HOTKEYS                     12


//  The OPTIONS structure contains all those variables that can be saved to the
//  users gridPointEdit3D QSettings.


typedef struct
{
  int32_t       position_form;              //  Position format number
  int32_t       contour_width;              //  Contour line width/thickness in pixels
  QColor        contour_color;              //  Color to be used for contours
  QColor        edit_color;                 //  Color to be used for marker and edit rectangles and polygons
  QColor        ref_color[2];               //  Color to be used for reference and null data.
  QColor        marker_color;               //  Color to be used for marker
  QColor        tracker_color;              //  Color to be used for "other CHRTR2 window" tracker
  QColor        background_color;           //  Color to be used for background
  QColor        scale_color;                //  Color to be used for the "scale"
  QFont         font;                       //  Font used for all ABE map GUI applications
  int32_t       smoothing_factor;           //  Contour smoothing factor (0-10)
  uint8_t       contour_location;           //  0 = draw at the top of the 3D space, 1 = draw in the point cloud.
  int32_t       color_index;                //  2 - color by horizontal uncertainty, 3 - color by vertical uncertainty
  int32_t       flag_index;                 //  0 - no flag, flag CHRTR2_USER_XX
  uint8_t       display_contours;           //  Display contours flag
  int32_t       function;                   //  Active edit or zoom function
  int32_t       slice_percent;              //  Percent of "Z" depth to use for a rotated "slice" view
  int32_t       slice_alpha;                //  Alpha (transparency) value for data that falls outside of the slice (0-255).
  float         z_factor;                   //  Conversion factor for Z values.  May be used to convert 
                                            //  m to ft...  Passed from chrtr2_view.
  float         z_offset;                   //  Offset value for Z values.  Passed from chrtr2_view.
  int32_t       point_size;                 //  Point size in pixels
  float         rotation_increment;         //  Degrees to rotate when pressing the left and right arrow keys
  float         nearest_value;              //  Value of point nearest to the cursor when moving the cursor
  int32_t       sparse_limit;               //  Number of input points to exceed before we zoom and rotate sparse data
  int32_t       zoom_percent;               //  Zoom in/out percentage
  double        exaggeration;               //  Z exaggeration
  float         zx_rotation;                //  Rotation around the ZX axis to be saved for startup.
  float         y_rotation;                 //  Rotation around the Y axis to be saved for startup.
  int32_t       main_button_icon_size;      //  Main button icon size (16, 24, 32)
  uint8_t       draw_scale;                 //  Set this to draw the scale in the display.
  QString       buttonAccel[HOTKEYS];       //  Accelerator key sequences for some of the buttons
  int32_t       undo_levels;                //  Number of undo levels
  uint8_t       display_interpolated;       //  Whether we want to display interpolated data (nomally off since there's not much point
                                            //  in editing interpolated data.
} OPTIONS;


// The POINT_DATA structure contains variables related to points in the CHRTR2 file.

typedef struct
{
  std::vector<double>       x;                         //  X position
  std::vector<double>       y;                         //  Y position
  std::vector<float>        z;                         //  Z value
  std::vector<float>        herr;                      //  Horizontal error
  std::vector<float>        verr;                      //  Vertical error
  std::vector<uint32_t>     val;                       //  Validity
  std::vector<int16_t>      chrtr2;                    //  CHRTR2 number
  std::vector<int32_t>      xcoord;                    //  X coordinate of the bin cell
  std::vector<int32_t>      ycoord;                    //  Y coordinate of the bin cell
  std::vector<uint8_t>      mask;                      //  Whether we are hiding this point or not
  int32_t                   count;                     //  Total number of points
  float                     view_min;                  //  Minimum z value - 5% of range
  float                     view_max;                  //  Maximum z value + 5% of range
} POINT_DATA;


// The UNDO block structure.

typedef struct
{
  std::vector<uint32_t>    val;                       //  Validity
  std::vector<uint32_t>    num;                       //  Point number in the POINT_DATA array
  int32_t                  count;                     //  Number of points in this undo block
} UNDO;


// General stuff.

typedef struct
{
  uint8_t       area_drawn;                 //  Area has been displayed flag
  uint8_t       drawing;                    //  Area is being drawn flag
  int32_t       save_function;              //  Saved active function when non-edit functions in use
  int32_t       save_mode;                  //  Saved active edit mode for exit (only delete functions)
  int32_t       num_records;                //  Number of records per section
  int32_t       poly_count;                 //  Polygon point count
  double        polygon_x[POLYGON_POINTS];  //  Polygon X
  double        polygon_y[POLYGON_POINTS];  //  Polygon Y
  int32_t       rotate_angle;               //  Rotation of view angle at 1/10 degree
  float         ortho_scale;                //  X scale of rotated view 
  int32_t       ortho_angle;                //  Rotation of view angle at 1/10 degree
  int32_t       ortho_min;                  //  Minimum "Z" value when rotated
  int32_t       ortho_max;                  //  Maximum "Z" value when rotated
  int32_t       ortho_range;                //  Max "Z" - min "Z" when rotated
  uint8_t       slice;                      //  Set if we're "slicing" the data
  int32_t       slice_min;                  //  Min "Z" value in slice
  int32_t       slice_max;                  //  Max "Z" value in slice
  int32_t       slice_size;                 //  Size of slice in "Z" (0 = off)
  double        slice_meters;               //  Size of slice in meters
  float         sin_array[3600];            //  Pre-computed sine values at 1/10 degree
  float         cos_array[3600];            //  Pre-computed cosine values at 1/10 degree
  int32_t       highlight_count;            //  Number of highlighted points
  std::vector<int32_t> highlight;           //  Highlighted points
  int32_t       maxd;                       //  Maximum contour density per grid cell
  uint8_t       resized;                    //  resized flag
  uint8_t       drawing_canceled;           //  set to cancel drawing
  uint8_t       color_array[2][NUMSHADES][4];//  Colors for points (0 is full color, 1 is semi-transparent)
  QColor        track_color_array[NUMSHADES];//  Colors for points in the tracker map
  int32_t       draw_area_width;            //  Width of map drawing area in pixels
  int32_t       draw_area_height;           //  Height of map drawing area in pixels
  NV_I32_COORD2 center_point;               //  center of drawing area in pixels
  int32_t       nearest_point;              //  point nearest to the cursor
  QIcon         buttonIcon[HOTKEYS];        //  Tool button icons for tools having accelerators
  QString       buttonText[HOTKEYS];        //  Tooltip text for the buttons that have editable accelerators
  QToolButton   *button[HOTKEYS];           //  Buttons that have editable accelerators
  QColor        widgetBackgroundColor;      //  The normal widget background color.
  QSharedMemory *chrtr2Share;               //  CHRTR2's shared memory pointer.
  CHRTR2_SHARE  *chrtr2_share;              //  Pointer to the CHRTR2_SHARE structure in shared memory.
  NV_F64_XYMBR   displayed_area;            //  Currently displayed area
  double        x_grid_size;                //  X grid spacing (degrees) for contours
  double        y_grid_size;                //  Y grid spacing (degrees) for contours
  float         min_z;
  float         max_z;
  QString       qsettings_app;              //  Application name for QSettings
  uint8_t       need_sparse;                //  Set to NVTrue if we have more than options.sparse_limit points to display.
  uint8_t       mask_active;                //  Set if we are masking any data.
  QWidget       *map_widget;                //  Map widget
  int32_t       process_id;                 //  This program's process ID
  QStatusBar    *progStatus;                //  Progress (and message) status bar
  QProgressBar  *statusProg;                //  Generic progress bar in status area
  QLabel        *statusProgLabel;           //  Generic progress bar lchrtr2l
  QPalette      statusProgPalette;          //  Background color palette for the progress bar lchrtr2l
  double        map_center_x;               //  Center of displayed area (lon)
  double        map_center_y;               //  Center of displayed area (lat)
  double        map_center_z;               //  Center of displayed area (z)
  std::vector<UNDO> undo;                   //  The undo block array
  int32_t       undo_count;                 //  The number of undo blocks
  scaleBox      *scale[NUM_SCALE_LEVELS];
  uint16_t      min_hsv_color;              //  HSV color wheel index for color to be used for minimum values (default 0)
  uint16_t      max_hsv_color;              //  HSV color wheel index for color to be used for maximum values (default 315)
  float         min_hsv_value;              //  Locked minimum data value for minimum HSV color
  float         max_hsv_value;              //  Locked maximum data value for maximum HSV color
  uint8_t       min_hsv_locked;             //  Set if min HSV is locked to a value
  uint8_t       max_hsv_locked;             //  Set if max HSV is locked to a value
  NV_F64_XYMBR  orig_bounds;                //  Starting bounds of area to be viewed
  NV_F64_XYMBC  bounds;                     //  Currently visible bounds
  NV_F64_XYMBC  aspect_bounds;              //  Bounds that are passed to nvMapGL in order to keep the proper aspect ratio
  float         contour_level;              //  Top level used for drawing contours at the top of the 3D space


  //  The following concern CHRTR2s as layers.  There are a few things from CHRTR2_SHARE that also need to be 
  //  addressed when playing with layers - open_args, display_chrtr2, and chrtr2_count.

  NV_F64_XYMBC  total_mbr;                  //  MBR of all of the displayed CHRTR2s
  NV_I32_COORD2 ll[MAX_CHRTR2_FILES];       //  lower left coordinates for get_buffer and put_buffer
  NV_I32_COORD2 ur[MAX_CHRTR2_FILES];       //  upper right coordinates for get_buffer and put_buffer
  int32_t       chrtr2_handle[MAX_CHRTR2_FILES]; //  CHRTR2 file handle
} MISC;


int32_t check_bounds (OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t index, uint8_t slice, uint8_t lock);
int32_t check_bounds (OPTIONS *options, MISC *misc, double x, double y, uint32_t v, uint8_t mask, int16_t chrtr2, uint8_t slice, uint8_t lock);
uint8_t check_line (MISC *misc, int32_t line);
void set_area (nvMapGL *map, OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t *x_bounds, int32_t *y_bounds,
               int32_t num_vertices, int32_t rect_flag);
void keep_area (nvMapGL *map, OPTIONS *options, MISC *misc, POINT_DATA *data, int32_t *x_bounds, int32_t *y_bounds,
                int32_t num_vertices);
void overlayFlag (nvMapGL *map, OPTIONS *options, MISC *misc, POINT_DATA *data);
void compute_ortho_values (nvMapGL *map, MISC *misc, OPTIONS *options, POINT_DATA *data, QScrollBar *sliceBar, uint8_t set_to_min);
void store_undo (MISC *misc, int32_t undo_levels, uint32_t val, uint32_t num);
void end_undo_block (MISC *misc);
void undo (MISC *misc, POINT_DATA *data);
uint8_t resize_undo (MISC *misc, OPTIONS *options, int32_t undo_levels);


#endif
