
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



#include "prefs.hpp"
#include "prefsHelp.hpp"

Prefs::Prefs (QWidget *parent, OPTIONS *op, MISC *mi, POINT_DATA *da):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  options = op;
  misc = mi;
  data = da;

  mod_share = *misc->chrtr2_share;
  mod_options = *op;

  setModal (true);


  hotKeyD = contoursD = NULL;


  dataChanged = NVFalse;


  setWindowTitle (tr ("gridPointEdit3D Preferences"));

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  QRadioButton *hdms = new QRadioButton (tr ("Hemisphere Degrees Minutes Seconds.decimal"));
  QRadioButton *hdm_ = new QRadioButton (tr ("Hemisphere Degrees Minutes.decimal"));
  QRadioButton *hd__ = new QRadioButton (tr ("Hemisphere Degrees.decimal"));
  QRadioButton *sdms = new QRadioButton (tr ("+/-Degrees Minutes Seconds.decimal"));
  QRadioButton *sdm_ = new QRadioButton (tr ("+/-Degrees Minutes.decimal"));
  QRadioButton *sd__ = new QRadioButton (tr ("+/-Degrees.decimal"));

  QGroupBox *formBox = new QGroupBox (this);
  QVBoxLayout *formBoxLayout = new QVBoxLayout;
  formBox->setLayout (formBoxLayout);

  bGrp = new QButtonGroup (this);
  bGrp->setExclusive (true);
  connect (bGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotPositionClicked (int)));

  bGrp->addButton (hdms, 0);
  bGrp->addButton (hdm_, 1);
  bGrp->addButton (hd__, 2);
  bGrp->addButton (sdms, 3);
  bGrp->addButton (sdm_, 4);
  bGrp->addButton (sd__, 5);

  formBoxLayout->addWidget (hdms);
  formBoxLayout->addWidget (hdm_);
  formBoxLayout->addWidget (hd__);
  formBoxLayout->addWidget (sdms);
  formBoxLayout->addWidget (sdm_);
  formBoxLayout->addWidget (sd__);

  formBoxLayout->addStretch (1);


  prefTab = new QTabWidget ();
  prefTab->setTabPosition (QTabWidget::North);

  prefTab->addTab (formBox, tr ("Position Format"));
  prefTab->setTabToolTip (0, tr ("Set the position display format"));
  prefTab->setTabWhatsThis (0, bGrpText);


  QGroupBox *cbox = new QGroupBox (tr ("Colors"), this);
  QVBoxLayout *cboxLayout = new QVBoxLayout;
  cbox->setLayout (cboxLayout);


  bMarkerColor = new QPushButton (tr ("Marker"), this);
  bMarkerColor->setToolTip (tr ("Change marker color"));
  bMarkerColor->setWhatsThis (markerColorText);
  bMarkerPalette = bMarkerColor->palette ();
  connect (bMarkerColor, SIGNAL (clicked ()), this, SLOT (slotMarkerColor ()));
  cboxLayout->addWidget (bMarkerColor);


  bTrackerColor = new QPushButton (tr ("Tracker"), this);
  bTrackerColor->setToolTip (tr ("Change tracker color"));
  bTrackerColor->setWhatsThis (trackerColorText);
  bTrackerPalette = bTrackerColor->palette ();
  connect (bTrackerColor, SIGNAL (clicked ()), this, SLOT (slotTrackerColor ()));
  cboxLayout->addWidget (bTrackerColor);


  bContourColor = new QPushButton (tr ("Contour"), this);
  bContourColor->setToolTip (tr ("Change contour color"));
  bContourColor->setWhatsThis (contourColorText);
  bContourPalette = bContourColor->palette ();
  connect (bContourColor, SIGNAL (clicked ()), this, SLOT (slotContourColor ()));
  cboxLayout->addWidget (bContourColor);


  bBackgroundColor = new QPushButton (tr ("Background"), this);
  bBackgroundColor->setToolTip (tr ("Change display background color"));
  bBackgroundColor->setWhatsThis (backgroundColorText);
  bBackgroundPalette = bBackgroundColor->palette ();
  connect (bBackgroundColor, SIGNAL (clicked ()), this, SLOT (slotBackgroundColor ()));
  cboxLayout->addWidget (bBackgroundColor);


  bScaleColor = new QPushButton (tr ("Scale"), this);
  bScaleColor->setToolTip (tr ("Change scale color"));
  bScaleColor->setWhatsThis (scaleColorText);
  bScalePalette = bScaleColor->palette ();
  connect (bScaleColor, SIGNAL (clicked ()), this, SLOT (slotScaleColor ()));
  cboxLayout->addWidget (bScaleColor);


  prefTab->addTab (cbox, tr ("Colors"));
  prefTab->setTabToolTip (1, tr ("Set the display colors"));
  prefTab->setTabWhatsThis (1, colorText);


  QGroupBox *conBox = new QGroupBox (this);
  QHBoxLayout *conBoxLayout = new QHBoxLayout;
  conBox->setLayout (conBoxLayout);
  QVBoxLayout *conBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *conBoxRightLayout = new QVBoxLayout;
  conBoxLayout->addLayout (conBoxLeftLayout);
  conBoxLayout->addLayout (conBoxRightLayout);


  QGroupBox *intBox = new QGroupBox (tr ("Interval"), this);
  QHBoxLayout *intBoxLayout = new QHBoxLayout;
  intBox->setLayout (intBoxLayout);

  contourInt = new QLineEdit (intBox);
  contourInt->setToolTip (tr ("Change the contour interval"));
  contourInt->setWhatsThis (contourIntText);
  intBoxLayout->addWidget (contourInt);

  bContourLevels = new QPushButton (tr ("Set Contour Levels"), intBox);
  bContourLevels->setToolTip (tr ("Set non-uniform individual contour intervals"));
  bContourLevels->setWhatsThis (contourLevelsText);
  connect (bContourLevels, SIGNAL (clicked ()), this, SLOT (slotContourLevelsClicked ()));
  intBoxLayout->addWidget (bContourLevels);

  conBoxLeftLayout->addWidget (intBox);


  QGroupBox *smBox = new QGroupBox (tr ("Smoothing"), this);
  QHBoxLayout *smBoxLayout = new QHBoxLayout;
  smBox->setLayout (smBoxLayout);
  contourSm = new QSpinBox (smBox);
  contourSm->setRange (0, 10);
  contourSm->setSingleStep (1);
  contourSm->setToolTip (tr ("Change the contour smoothing factor (0 - 10)"));
  contourSm->setWhatsThis (contourSmText);
  smBoxLayout->addWidget (contourSm);
  conBoxLeftLayout->addWidget (smBox);


  lGrp = new QButtonGroup (this);
  lGrp->setExclusive (true);
  connect (lGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotContourLocationClicked (int)));

  QGroupBox *locBox = new QGroupBox (tr ("Contour location"), this);
  QHBoxLayout *locBoxLayout = new QHBoxLayout;
  locBox->setLayout (locBoxLayout);
  locBox->setWhatsThis (locBoxText);

  topCheck = new QCheckBox (tr ("Top"), locBox);
  topCheck->setToolTip (tr ("Contours displayed at the top of the 3D space"));
  topCheck->setWhatsThis (topCheckText);
  lGrp->addButton (topCheck, 0);
  locBoxLayout->addWidget (topCheck);

  cloudCheck = new QCheckBox (tr ("Point cloud"), locBox);
  cloudCheck->setToolTip (tr ("Contours displayed in the point cloud"));
  cloudCheck->setWhatsThis (cloudCheckText);
  lGrp->addButton (cloudCheck, 1);
  locBoxLayout->addWidget (cloudCheck);

  conBoxLeftLayout->addWidget (locBox);


  QGroupBox *zBox = new QGroupBox (tr ("Scale"), this);
  QHBoxLayout *zBoxLayout = new QHBoxLayout;
  zBox->setLayout (zBoxLayout);
  ZFactor = new QComboBox (zBox);
  ZFactor->setToolTip (tr ("Change the Z scale factor"));
  ZFactor->setWhatsThis (ZFactorText);
  ZFactor->setEditable (true);
  ZFactor->addItem ("1.00000 " + tr ("(no scaling)"));
  ZFactor->addItem ("3.28084 " + tr ("(meters to feet)"));
  ZFactor->addItem ("0.34080 " + tr ("(feet to meters)"));
  ZFactor->addItem ("0.54681 " + tr ("(meters to fathoms)"));
  ZFactor->addItem ("1.82880 " + tr ("(fathoms to meters)"));
  ZFactor->addItem ("0.53333 " + tr ("(meters to fathoms (at 4800 ft/sec)"));
  zBoxLayout->addWidget (ZFactor);
  conBoxLeftLayout->addWidget (zBox);


  QGroupBox *offBox = new QGroupBox (tr ("Offset"), this);
  QHBoxLayout *offBoxLayout = new QHBoxLayout;
  offBox->setLayout (offBoxLayout);
  offset = new QDoubleSpinBox (offBox);
  offset->setDecimals (2);
  offset->setRange (-1000.0, 1000.0);
  offset->setSingleStep (10.0);
  offset->setToolTip (tr ("Change the contour/Z offset value"));
  offset->setWhatsThis (offsetText);
  offBoxLayout->addWidget (offset);
  conBoxLeftLayout->addWidget (offBox);


  QGroupBox *widBox = new QGroupBox (tr ("Contour width"), this);
  QHBoxLayout *widBoxLayout = new QHBoxLayout;
  widBox->setLayout (widBoxLayout);
  Width = new QComboBox (widBox);
  Width->setToolTip (tr ("Change the contour line width/thickness (pixels)"));
  Width->setWhatsThis (WidthText);
  Width->setEditable (true);
  Width->addItem ("1");
  Width->addItem ("2");
  Width->addItem ("3");
  widBoxLayout->addWidget (Width);
  conBoxRightLayout->addWidget (widBox);


  QGroupBox *zoomBox = new QGroupBox (tr ("Zoom percentage"), this);
  QHBoxLayout *zoomBoxLayout = new QHBoxLayout;
  zoomBox->setLayout (zoomBoxLayout);
  zoomPercent = new QSpinBox (zoomBox);
  zoomPercent->setRange (5, 50);
  zoomPercent->setSingleStep (5);
  zoomPercent->setToolTip (tr ("Change the zoom in/out percentage (10 - 50)"));
  zoomPercent->setWhatsThis (zoomPercentText);
  zoomBoxLayout->addWidget (zoomPercent);
  conBoxRightLayout->addWidget (zoomBox);


  QGroupBox *exagBox = new QGroupBox (tr ("Z exaggeration"), this);
  QHBoxLayout *exagBoxLayout = new QHBoxLayout;
  exagBox->setLayout (exagBoxLayout);
  exag = new QDoubleSpinBox (offBox);
  exag->setDecimals (1);
  exag->setRange (1.0, 10.0);
  exag->setSingleStep (1.0);
  exag->setToolTip (tr ("Change the Z exaggeration value"));
  exag->setWhatsThis (exagText);
  exagBoxLayout->addWidget (exag);
  conBoxRightLayout->addWidget (exagBox);


  QGroupBox *pixBox = new QGroupBox (tr ("Point size"), this);
  QHBoxLayout *pixBoxLayout = new QHBoxLayout;
  pixBox->setLayout (pixBoxLayout);
  pointSize = new QSpinBox (pixBox);
  pointSize->setRange (2, 10);
  pointSize->setSingleStep (1);
  pointSize->setToolTip (tr ("Change the point size (pixels) (2 - 10)"));
  pointSize->setWhatsThis (pointSizeText);
  pixBoxLayout->addWidget (pointSize);
  conBoxRightLayout->addWidget (pixBox);


  prefTab->addTab (conBox, tr ("Contour/Depth"));
  prefTab->setTabToolTip (2, tr ("Set the contour/depth options"));
  prefTab->setTabWhatsThis (2, conText);


  QGroupBox *miscBox = new QGroupBox (this);
  QHBoxLayout *miscBoxLayout = new QHBoxLayout;
  miscBox->setLayout (miscBoxLayout);
  QVBoxLayout *miscBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *miscBoxRightLayout = new QVBoxLayout;
  miscBoxLayout->addLayout (miscBoxLeftLayout);
  miscBoxLayout->addLayout (miscBoxRightLayout);


  QGroupBox *sparseBox = new QGroupBox (tr ("Sparse point limit"), this);
  QHBoxLayout *sparseBoxLayout = new QHBoxLayout;
  sparseBox->setLayout (sparseBoxLayout);

  sparse = new QSpinBox (this);
  sparse->setRange (50000, 1000000);
  sparse->setSingleStep (1000);
  sparse->setToolTip (tr ("Set the point limit at which we begin to plot sparse points"));
  sparse->setWhatsThis (sparseText);
  sparseBoxLayout->addWidget (sparse);

  miscBoxLeftLayout->addWidget (sparseBox);


  QGroupBox *rotBox = new QGroupBox (tr ("Rotation"), this);
  QHBoxLayout *rotBoxLayout = new QHBoxLayout;
  rotBox->setLayout (rotBoxLayout);
  rot = new QDoubleSpinBox (rotBox);
  rot->setDecimals (1);
  rot->setRange (1.0, 30.0);
  rot->setSingleStep (1.0);
  rot->setToolTip (tr ("Change the view rotation increment (1.0 - 30.0)"));
  rot->setWhatsThis (rotText);
  rotBoxLayout->addWidget (rot);
  miscBoxLeftLayout->addWidget (rotBox);


  QGroupBox *scaleBox = new QGroupBox (tr ("Draw scale"), this);
  QHBoxLayout *scaleBoxLayout = new QHBoxLayout;
  scaleBox->setLayout (scaleBoxLayout);
  scale = new QCheckBox (this);
  scale->setToolTip (tr ("If checked, the scale will be drawn on the screen"));
  scale->setWhatsThis (scaleText);
  scaleBoxLayout->addWidget (scale);
  miscBoxRightLayout->addWidget (scaleBox);


  QGroupBox *undoBox = new QGroupBox (tr ("Undo levels"), this);
  QHBoxLayout *undoBoxLayout = new QHBoxLayout;
  undoBox->setLayout (undoBoxLayout);
  undo = new QSpinBox (undoBox);
  undo->setRange (100, 1000000);
  undo->setSingleStep (100);
  undo->setToolTip (tr ("Change the maximum number of undo levels (100 - 1,000,000)"));
  undo->setWhatsThis (prefsUndoText);
  undoBoxLayout->addWidget (undo);
  miscBoxRightLayout->addWidget (undoBox);


  miscBoxLeftLayout->addStretch (1);
  miscBoxRightLayout->addStretch (1);


  prefTab->addTab (miscBox, tr ("Miscellaneous"));
  prefTab->setTabToolTip (3, tr ("Set miscellaneous options"));
  prefTab->setTabWhatsThis (3, miscText);



  QGroupBox *kbox = new QGroupBox (this);
  QVBoxLayout *kboxLayout = new QVBoxLayout;
  kbox->setLayout (kboxLayout);


  bHotKeys = new QPushButton (tr ("Main Buttons"), this);
  bHotKeys->setToolTip (tr ("Change hot key sequences for buttons"));
  bHotKeys->setWhatsThis (hotKeysText);
  connect (bHotKeys, SIGNAL (clicked ()), this, SLOT (slotHotKeys ()));
  kboxLayout->addWidget (bHotKeys);


  QGroupBox *iconBox = new QGroupBox (tr ("Main Button Icon Size"), this);
  QHBoxLayout *iconBoxLayout = new QHBoxLayout;
  iconBox->setLayout (iconBoxLayout);

  iconSize = new QComboBox (iconBox);
  iconSize->setToolTip (tr ("Set the size (in pixels) of the main button icons"));
  iconSize->setWhatsThis (iconSizeText);
  iconSize->setEditable (false);
  iconSize->addItem ("16");
  iconSize->addItem ("20");
  iconSize->addItem ("24");
  iconSize->addItem ("28");
  iconSize->addItem ("32");
  iconBoxLayout->addWidget (iconSize);


  kboxLayout->addWidget (iconBox);


  prefTab->addTab (kbox, tr ("Hot Keys"));
  prefTab->setTabToolTip (4, tr ("Set hot keys"));
  prefTab->setTabWhatsThis (4, kboxText);


  vbox->addWidget (prefTab);


  setFields ();


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  bRestoreDefaults = new QPushButton (tr ("Restore Defaults"), this);
  bRestoreDefaults->setToolTip (tr ("Restore all preferences to the default state"));
  bRestoreDefaults->setWhatsThis (restoreDefaultsText);
  connect (bRestoreDefaults, SIGNAL (clicked ()), this, SLOT (slotRestoreDefaults ()));
  actions->addWidget (bRestoreDefaults);

  QPushButton *applyButton = new QPushButton (tr ("OK"), this);
  applyButton->setToolTip (tr ("Accept changes and close dialog"));
  applyButton->setWhatsThis (applyPrefsText);
  connect (applyButton, SIGNAL (clicked ()), this, SLOT (slotApplyPrefs ()));
  actions->addWidget (applyButton);

  QPushButton *closeButton = new QPushButton (tr ("Cancel"), this);
  closeButton->setToolTip (tr ("Discard changes and close dialog"));
  closeButton->setWhatsThis (closePrefsText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotClosePrefs ()));
  actions->addWidget (closeButton);


  show ();
}



Prefs::~Prefs ()
{
}



void
Prefs::slotMarkerColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.marker_color, this, tr ("gridPointEdit3D Marker Color"),
                                QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.marker_color = clr;

  setFields ();
}



void
Prefs::slotContourColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.contour_color, this, tr ("pfmEdit Contour Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.contour_color = clr;

  setFields ();
}



void
Prefs::slotTrackerColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.tracker_color, this, tr ("gridPointEdit3D Tracker Color"),
                                QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.tracker_color = clr;

  setFields ();
}



void
Prefs::slotBackgroundColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.background_color, this, tr ("gridPointEdit3D Background Color"),
                                QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.background_color = clr;


  setFields ();
}



void
Prefs::slotScaleColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.scale_color, this, tr ("gridPointEdit3D Scale Color"),
                                QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.scale_color = clr;

  setFields ();
}



void
Prefs::slotHelp ()
{
  QWhatsThis::enterWhatsThisMode ();
}



void
Prefs::slotRestoreDefaults ()
{
  void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore);


  set_defaults (misc, options, NVTrue);


  misc->chrtr2_share->cint = 5.0;
  misc->chrtr2_share->num_levels = 0;


  //  Default contour levels

  misc->chrtr2_share->num_levels = 52;
  misc->chrtr2_share->contour_levels[0] = -30;
  misc->chrtr2_share->contour_levels[1] = -20;
  misc->chrtr2_share->contour_levels[2] = -10;
  misc->chrtr2_share->contour_levels[3] = -5;
  misc->chrtr2_share->contour_levels[4] = -2;
  misc->chrtr2_share->contour_levels[5] = 0;
  misc->chrtr2_share->contour_levels[6] = 2;
  misc->chrtr2_share->contour_levels[7] = 5;
  misc->chrtr2_share->contour_levels[8] = 10;
  misc->chrtr2_share->contour_levels[9] = 20;
  misc->chrtr2_share->contour_levels[10] = 30;
  misc->chrtr2_share->contour_levels[11] = 40;
  misc->chrtr2_share->contour_levels[12] = 50;
  misc->chrtr2_share->contour_levels[13] = 60;
  misc->chrtr2_share->contour_levels[14] = 70;
  misc->chrtr2_share->contour_levels[15] = 80;
  misc->chrtr2_share->contour_levels[16] = 90;
  misc->chrtr2_share->contour_levels[17] = 100;
  misc->chrtr2_share->contour_levels[18] = 120;
  misc->chrtr2_share->contour_levels[19] = 130;
  misc->chrtr2_share->contour_levels[20] = 140;
  misc->chrtr2_share->contour_levels[21] = 160;
  misc->chrtr2_share->contour_levels[22] = 180;
  misc->chrtr2_share->contour_levels[23] = 200;
  misc->chrtr2_share->contour_levels[24] = 250;
  misc->chrtr2_share->contour_levels[25] = 300;
  misc->chrtr2_share->contour_levels[26] = 350;
  misc->chrtr2_share->contour_levels[27] = 400;
  misc->chrtr2_share->contour_levels[28] = 450;
  misc->chrtr2_share->contour_levels[29] = 500;
  misc->chrtr2_share->contour_levels[30] = 600;
  misc->chrtr2_share->contour_levels[31] = 700;
  misc->chrtr2_share->contour_levels[32] = 800;
  misc->chrtr2_share->contour_levels[33] = 900;
  misc->chrtr2_share->contour_levels[34] = 1000;
  misc->chrtr2_share->contour_levels[35] = 1500;
  misc->chrtr2_share->contour_levels[36] = 2000;
  misc->chrtr2_share->contour_levels[37] = 2500;
  misc->chrtr2_share->contour_levels[38] = 3000;
  misc->chrtr2_share->contour_levels[39] = 3500;
  misc->chrtr2_share->contour_levels[40] = 4000;
  misc->chrtr2_share->contour_levels[41] = 4500;
  misc->chrtr2_share->contour_levels[42] = 5000;
  misc->chrtr2_share->contour_levels[43] = 5500;
  misc->chrtr2_share->contour_levels[44] = 6000;
  misc->chrtr2_share->contour_levels[45] = 6500;
  misc->chrtr2_share->contour_levels[46] = 7000;
  misc->chrtr2_share->contour_levels[47] = 7500;
  misc->chrtr2_share->contour_levels[48] = 8000;
  misc->chrtr2_share->contour_levels[49] = 8500;
  misc->chrtr2_share->contour_levels[50] = 9000;
  misc->chrtr2_share->contour_levels[51] = 10000;


  mod_options = *options;
  mod_share = *misc->chrtr2_share;


  setFields ();


  *misc->chrtr2_share = mod_share;
  *options = mod_options;


  //  Let the calling program (gridEdit) know that we've changed some things that it uses.

  if (misc->chrtr2_share != NULL)
    {
      misc->chrtr2_share->settings_changed = NVTrue;
      misc->chrtr2_share->position_form = options->position_form;
      misc->chrtr2_share->z_factor = options->z_factor;
      misc->chrtr2_share->z_offset = options->z_offset;
      misc->chrtr2_share->settings_changed = NVTrue;
    }


  emit dataChangedSignal ();

  close ();
}



void
Prefs::slotPositionClicked (int id)
{
  mod_options.position_form = id;
}



void
Prefs::slotCloseContours ()
{
  //  Get the values.

  QString string;
  int32_t status, j;
  float tmp;

  j = 0;
  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      string = contoursTable->item (i, 0)->text ();
      status = sscanf (string.toLatin1 (), "%f", &tmp);
      if (status == 1)
        {
          mod_share.contour_levels[j] = tmp;
          j++;
        }
    }

  mod_share.num_levels = j + 1;

  contoursD->close ();
}



void
Prefs::slotContourLocationClicked (int id)
{
  mod_options.contour_location = id;
}



void
Prefs::slotContourLevelsClicked ()
{
  if (contoursD) slotCloseContours ();


  QString string;

  mod_share.cint = 0.0;
  string.sprintf ("%.2f", mod_share.cint);
  contourInt->setText (string);

  
  contoursD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  contoursD->setWindowTitle (tr ("pfmEdit Contour Levels"));

  QVBoxLayout *vbox = new QVBoxLayout (contoursD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  contoursTable = new QTableWidget (MAX_CONTOUR_LEVELS, 1, this);
  contoursTable->setWhatsThis (contoursTableText);
  contoursTable->setAlternatingRowColors (true);
  QTableWidgetItem *chItem = new QTableWidgetItem (tr ("Contour level"));
  contoursTable->setHorizontalHeaderItem (0, chItem);

  QTableWidgetItem *cntItem[MAX_CONTOUR_LEVELS];

  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      if (i < mod_share.num_levels)
        {
          string.sprintf ("%f", mod_share.contour_levels[i]);
        }
      else
        {
          string.sprintf (" ");
        }
      cntItem[i] = new QTableWidgetItem (string); 
      contoursTable->setItem (i, 0, cntItem[i]);
    }
  contoursTable->resizeColumnsToContents ();


  vbox->addWidget (contoursTable, 1);

  int32_t w = qMin (800, contoursTable->columnWidth (0) + 60);
  contoursD->resize (w, 600);

  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (contoursD);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), contoursD);
  closeButton->setToolTip (tr ("Close the contour levels dialog"));
  closeButton->setWhatsThis (closeContoursText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseContours ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  contoursD->move (x () + width () / 2 - contoursD->width () / 2, y () + height () / 2 - contoursD->height () / 2);

  contoursD->show ();
}



void 
Prefs::slotHotKeys ()
{
  if (hotKeyD) slotCloseHotKeys ();


  hotKeyD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  hotKeyD->setWindowTitle (tr ("gridPointEdit3D Hot Keys"));

  QVBoxLayout *vbox = new QVBoxLayout (hotKeyD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  hotKeyTable = new QTableWidget (HOTKEYS, 2, this);
  hotKeyTable->setWhatsThis (hotKeyTableText);
  hotKeyTable->setAlternatingRowColors (true);
  QTableWidgetItem *bItemH = new QTableWidgetItem (tr ("Button"));
  hotKeyTable->setHorizontalHeaderItem (0, bItemH);
  QTableWidgetItem *kItemH = new QTableWidgetItem (tr ("Key"));
  hotKeyTable->setHorizontalHeaderItem (1, kItemH);

  QTableWidgetItem *bItem[HOTKEYS], *kItem[HOTKEYS];


  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      bItem[i] = new QTableWidgetItem (misc->buttonIcon[i], misc->buttonText[i]);
      bItem[i]->setFlags (Qt::ItemIsEnabled);

      hotKeyTable->setItem (i, 0, bItem[i]);
      kItem[i] = new QTableWidgetItem (mod_options.buttonAccel[i]);
      hotKeyTable->setItem (i, 1, kItem[i]);
    }
  hotKeyTable->resizeColumnsToContents ();
  hotKeyTable->resizeRowsToContents ();

  vbox->addWidget (hotKeyTable, 1);

  int32_t w = qMin (800, hotKeyTable->columnWidth (0) + hotKeyTable->columnWidth (1) + 40);
  int32_t h = qMin (600, hotKeyTable->rowHeight (0) * HOTKEYS + 75);
  hotKeyD->resize (w, h);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (hotKeyD);
  bHelp->setIcon (QIcon (":/icons/contextHelp"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), hotKeyD);
  closeButton->setToolTip (tr ("Close the hot key dialog"));
  closeButton->setWhatsThis (closeHotKeyText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseHotKeys ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  hotKeyD->move (x () + width () / 2 - hotKeyD->width () / 2, y () + height () / 2 - hotKeyD->height () / 2);

  hotKeyD->show ();
}



void 
Prefs::slotCloseHotKeys ()
{
  for (int32_t i = 0 ; i < HOTKEYS ; i++) mod_options.buttonAccel[i] = hotKeyTable->item (i, 1)->text ();

  hotKeyD->close ();
}



void 
Prefs::setFields ()
{
  QString string;

  bGrp->button (mod_options.position_form)->setChecked (true);


  int32_t hue, sat, val;

  mod_options.marker_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bMarkerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bMarkerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bMarkerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bMarkerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bMarkerPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.marker_color);
  bMarkerPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.marker_color);
  bMarkerColor->setPalette (bMarkerPalette);


  mod_options.contour_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bContourPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bContourPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bContourPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bContourPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bContourPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.contour_color);
  bContourPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.contour_color);
  bContourColor->setPalette (bContourPalette);


  mod_options.tracker_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bTrackerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bTrackerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bTrackerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bTrackerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bTrackerPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.tracker_color);
  bTrackerPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.tracker_color);
  bTrackerColor->setPalette (bTrackerPalette);


  mod_options.background_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bBackgroundPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.background_color);
  bBackgroundPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.background_color);
  bBackgroundColor->setPalette (bBackgroundPalette);


  mod_options.scale_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bScalePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bScalePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bScalePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bScalePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bScalePalette.setColor (QPalette::Normal, QPalette::Button, mod_options.scale_color);
  bScalePalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.scale_color);
  bScaleColor->setPalette (bScalePalette);


  string.sprintf ("%.2f", mod_share.cint);
  contourInt->setText (string);

  contourSm->setValue (mod_options.smoothing_factor);


  if (mod_options.contour_location)
    {
      cloudCheck->setChecked (true);
    }
  else
    {
      topCheck->setChecked (true);
    }


  string.sprintf ("%.5f", mod_options.z_factor);
  ZFactor->lineEdit ()->setText (string);

  offset->setValue (mod_options.z_offset);

  string.sprintf ("%d", mod_options.contour_width);
  Width->lineEdit ()->setText (string);

  pointSize->setValue (mod_options.point_size);
  sparse->setValue (mod_options.sparse_limit);

  switch (mod_options.main_button_icon_size)
    {
    case 16:
      iconSize->setCurrentIndex (0);
      break;

    case 20:
      iconSize->setCurrentIndex (1);
      break;

    case 24:
    default:
      iconSize->setCurrentIndex (2);
      break;

    case 28:
      iconSize->setCurrentIndex (3);
      break;

    case 32:
      iconSize->setCurrentIndex (4);
      break;
    }

      
  zoomPercent->setValue (mod_options.zoom_percent);
  rot->setValue (mod_options.rotation_increment);
  exag->setValue (mod_options.exaggeration);
  undo->setValue (mod_options.undo_levels);
  scale->setChecked (mod_options.draw_scale);
}



void
Prefs::slotApplyPrefs ()
{
  int32_t status, tmp_i;
  float tmp_f;

  if (mod_options.position_form != options->position_form) dataChanged = NVTrue;


  QString tmp = contourInt->text ();
  status = sscanf (tmp.toLatin1 (), "%f", &tmp_f);
  if (status == 1) mod_share.cint = tmp_f;
  if (mod_share.cint != misc->chrtr2_share->cint) dataChanged = NVTrue;

  tmp = contourSm->text ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.smoothing_factor = tmp_i;
  if (mod_options.smoothing_factor != options->smoothing_factor) dataChanged = NVTrue;

  if (mod_options.contour_location != options->contour_location) dataChanged = NVTrue;

  tmp = ZFactor->currentText ();
  status = sscanf (tmp.toLatin1 (), "%f", &tmp_f);
  if (status == 1) mod_options.z_factor = tmp_f;
  if (mod_options.z_factor != options->z_factor) dataChanged = NVTrue;


  mod_options.z_offset = offset->value ();
  if (mod_options.z_offset != options->z_offset) dataChanged = NVTrue;


  mod_options.point_size = pointSize->value ();
  if (mod_options.point_size != options->point_size) dataChanged = NVTrue;

  mod_options.sparse_limit = sparse->value ();
  if (mod_options.sparse_limit != options->sparse_limit) dataChanged = NVTrue;

  tmp = Width->currentText ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.contour_width = tmp_i;
  if (mod_options.contour_width != options->contour_width) dataChanged = NVTrue;


  switch (iconSize->currentIndex ())
    {
    case 0:
      mod_options.main_button_icon_size = 16;
      break;

    case 1:
      mod_options.main_button_icon_size = 20;
      break;

    case 2:
      mod_options.main_button_icon_size = 24;
      break;

    case 3:
      mod_options.main_button_icon_size = 28;
      break;

    case 4:
      mod_options.main_button_icon_size = 32;
      break;
    }
  if (mod_options.main_button_icon_size != options->main_button_icon_size) dataChanged = NVTrue;

  mod_options.rotation_increment = rot->value ();
  if (mod_options.rotation_increment != options->rotation_increment) dataChanged = NVTrue;

  mod_options.exaggeration = exag->value ();
  if (mod_options.exaggeration != options->exaggeration) dataChanged = NVTrue;

  mod_options.zoom_percent = zoomPercent->value ();
  if (mod_options.zoom_percent != options->zoom_percent) dataChanged = NVTrue;

  mod_options.undo_levels = undo->value ();
  if (mod_options.undo_levels != options->undo_levels && resize_undo (misc, options, mod_options.undo_levels)) dataChanged = NVTrue;

  mod_options.draw_scale = scale->isChecked ();
  if (mod_options.draw_scale != options->draw_scale) dataChanged = NVTrue;

  if (mod_options.contour_color != options->contour_color) dataChanged = NVTrue;
  if (mod_options.marker_color != options->marker_color) dataChanged = NVTrue;
  if (mod_options.tracker_color != options->tracker_color) dataChanged = NVTrue;
  if (mod_options.background_color != options->background_color) dataChanged = NVTrue;
  if (mod_options.scale_color != options->scale_color) dataChanged = NVTrue;


  //  Make sure we have no hotkey duplications.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = i + 1 ; j < HOTKEYS ; j++)
        {
          if (mod_options.buttonAccel[i].toUpper () == mod_options.buttonAccel[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("gridPointEdit3D preferences"),
                                    tr ("Hot key for <b>%1</b> button conflicts with hotkey for <b>%2</b> button!<br><br>").arg 
                                    (misc->buttonText[i]).arg (misc->buttonText[j]) +
                                    tr ("Resetting button hotkeys for these two buttons."));

              mod_options.buttonAccel[i] = options->buttonAccel[i];
              mod_options.buttonAccel[j] = options->buttonAccel[j];
              dataChanged = NVFalse;

              hotKeyD->close ();
              hotKeyD = NULL;

              return;
            }
        }
    }


  //  Now check for changes to the hotkeys.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      if (mod_options.buttonAccel[i] != options->buttonAccel[i])
        {
          dataChanged = NVTrue;
          emit hotKeyChangedSignal (i);
          break;
        }
    }


  if (mod_share.num_levels != misc->chrtr2_share->num_levels) dataChanged = NVTrue;


  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      if (mod_share.contour_levels[i] != misc->chrtr2_share->contour_levels[i])
        {
          dataChanged = NVTrue;
          break;
        }
    }


  setFields ();


  hide ();


  if (dataChanged)
    {
      *misc->chrtr2_share = mod_share;
      *options = mod_options;


      //  Let the calling program (gridEdit) know that we've changed some things that it uses.

      if (misc->chrtr2_share != NULL)
        {
          misc->chrtr2_share->settings_changed = NVTrue;
          misc->chrtr2_share->position_form = options->position_form;
          misc->chrtr2_share->z_factor = options->z_factor;
          misc->chrtr2_share->z_offset = options->z_offset;
          misc->chrtr2_share->settings_changed = NVTrue;
        }


      emit dataChangedSignal ();
    }

  close ();
}



void
Prefs::slotClosePrefs ()
{
  close ();
}
