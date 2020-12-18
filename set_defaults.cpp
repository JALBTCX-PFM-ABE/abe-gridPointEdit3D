
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

void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore)
{
  if (!restore)
    {
      misc->draw_area_width = 1220;
      misc->draw_area_height = 950;
      misc->drawing_canceled = NVFalse;
      misc->resized = NVTrue;
      misc->area_drawn = NVFalse;
      misc->poly_count = 0;
      misc->slice = NVFalse;
      misc->drawing = NVFalse;
      misc->need_sparse = NVFalse;
      misc->mask_active = NVFalse;
      misc->highlight_count = 0;
      misc->maxd = 10;


      //  Set up the sine and cosine values for slicing.

      for (int32_t i = 0 ; i < 3600 ; i++)
        {
          misc->sin_array[i] = sinf (((float) i / 10.0) * NV_DEG_TO_RAD);
          misc->cos_array[i] = cosf (((float) i / 10.0) * NV_DEG_TO_RAD);
        }


      //  Tooltip text for the buttons that have editable accelerators

      misc->buttonText[SAVE_EXIT_KEY] = gridPointEdit3D::tr ("Save changes and exit");
      misc->buttonText[SAVE_EXIT_MASK_KEY] = gridPointEdit3D::tr ("Save changes, exit, and filter mask in gridEdit");
      misc->buttonText[NO_SAVE_EXIT_KEY] = gridPointEdit3D::tr ("Exit without saving changes");
      misc->buttonText[RESET_KEY] = gridPointEdit3D::tr ("Reset to original view");
      misc->buttonText[DELETE_POINT_MODE_KEY] = gridPointEdit3D::tr ("Select delete point edit mode");
      misc->buttonText[DELETE_RECTANGLE_MODE_KEY] = gridPointEdit3D::tr ("Select delete rectangle edit mode");
      misc->buttonText[DELETE_POLYGON_MODE_KEY] = gridPointEdit3D::tr ("Select delete polygon edit mode");
      misc->buttonText[UNDO_KEY] = gridPointEdit3D::tr ("Undo last edit operation");
      misc->buttonText[CLEAR_HIGHLIGHT_KEY] = gridPointEdit3D::tr ("Clear all highlighted/marked points");
      misc->buttonText[HIGHLIGHT_POLYGON_MODE_KEY] = gridPointEdit3D::tr ("Select highlight/mark points in polygon mode");
      misc->buttonText[CLEAR_POLYGON_MODE_KEY] = gridPointEdit3D::tr ("Select clear all highlighted/marked points in polygon mode");
      misc->buttonText[TOGGLE_CONTOUR_KEY] = gridPointEdit3D::tr ("Toggle contour drawing");


      //  ToolButton icons for the buttons that have editable accelerators

      misc->buttonIcon[SAVE_EXIT_KEY] = QIcon (":/icons/exit_save.png");
      misc->buttonIcon[SAVE_EXIT_MASK_KEY] = QIcon (":/icons/exit_mask.png");
      misc->buttonIcon[NO_SAVE_EXIT_KEY] = QIcon (":/icons/exit_no_save.png");
      misc->buttonIcon[RESET_KEY] = QIcon (":/icons/reset_view.png");
      misc->buttonIcon[DELETE_POINT_MODE_KEY] = QIcon (":/icons/delete_point.png");
      misc->buttonIcon[DELETE_RECTANGLE_MODE_KEY] = QIcon (":/icons/delete_rect.png");
      misc->buttonIcon[DELETE_POLYGON_MODE_KEY] = QIcon (":/icons/delete_poly.png");
      misc->buttonIcon[UNDO_KEY] = QIcon (":/icons/undo.png");
      misc->buttonIcon[CLEAR_HIGHLIGHT_KEY] = QIcon (":/icons/clear_highlight.png");
      misc->buttonIcon[HIGHLIGHT_POLYGON_MODE_KEY] = QIcon (":/icons/highlight_polygon.png");
      misc->buttonIcon[CLEAR_POLYGON_MODE_KEY] = QIcon (":/icons/clear_polygon.png");
      misc->buttonIcon[TOGGLE_CONTOUR_KEY] = QIcon (":/icons/contour.png");


      misc->chrtr2_share->key = 0;
      misc->displayed_area.min_y = -91.0;

      misc->qsettings_app = gridPointEdit3D::tr ("gridPointEdit3D");

      misc->undo_count = 0;
      misc->process_id = getpid ();
      misc->min_hsv_color = 0;
      misc->max_hsv_color = 315;
      misc->min_hsv_locked = NVFalse;
      misc->max_hsv_locked = NVFalse;
      misc->min_hsv_value = 0.0;
      misc->max_hsv_value = 0.0;


      //  This is a special case for an option.  We don't want to reset the undo levels if we called for a restore of defaults from the prefs dialog.

      options->undo_levels = 100;
    }


  options->function = DELETE_POLYGON;
  options->slice_percent = 5;
  options->slice_alpha = 24;
  options->contour_width = 2;
  options->smoothing_factor = 0;
  options->contour_location = 0;
  options->display_contours = NVFalse;
  options->point_size = 3;
  options->contour_color = QColor (255, 255, 255);
  options->edit_color = QColor (255, 255, 255, 255);
  options->ref_color[0] = QColor (255, 255, 255, 255);
  options->marker_color = QColor (255, 255, 255, 255);
  options->tracker_color = QColor (255, 255, 0, 255);
  options->background_color = QColor (0, 0, 0, 255);
  options->scale_color = QColor (255, 255, 255, 255);
  options->font = QApplication::font ();
  options->main_button_icon_size = 24;
  options->rotation_increment = 5.0;
  options->color_index = 0;
  options->flag_index = PRE_USER - 1;
  options->sparse_limit = 250000;
  options->exaggeration = 3.0;
  options->zoom_percent = 10;
  options->draw_scale = NVTrue;
  options->zx_rotation = 0.0;
  options->y_rotation = 0.0;
  options->display_interpolated = NVFalse;


  options->buttonAccel[SAVE_EXIT_KEY] = "Ctrl+s";
  options->buttonAccel[SAVE_EXIT_MASK_KEY] = "Ctrl+f";
  options->buttonAccel[NO_SAVE_EXIT_KEY] = "Ctrl+q";
  options->buttonAccel[RESET_KEY] = "Ctrl+r";
  options->buttonAccel[DELETE_POINT_MODE_KEY] = "F3";
  options->buttonAccel[DELETE_RECTANGLE_MODE_KEY] = "F4";
  options->buttonAccel[DELETE_POLYGON_MODE_KEY] = "F5";
  options->buttonAccel[UNDO_KEY] = "Ctrl+z";
  options->buttonAccel[CLEAR_HIGHLIGHT_KEY] = "Ctrl+h";
  options->buttonAccel[HIGHLIGHT_POLYGON_MODE_KEY] = "Ctrl+p";
  options->buttonAccel[CLEAR_POLYGON_MODE_KEY] = "Ctrl+c";
  options->buttonAccel[TOGGLE_CONTOUR_KEY] = "Alt+c";
}
