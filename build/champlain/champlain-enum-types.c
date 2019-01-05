
/* This file is generated by glib-mkenums, do not modify it. This code is licensed under the same license as the containing project. Note that it links to GLib, so must comply with the LGPL linking clauses. */

#include "champlain-enum-types.h"
#include "champlain.h"
#include "champlain-defines.h"
#include "champlain-point.h"
#include "champlain-custom-marker.h"
#include "champlain-view.h"
#include "champlain-layer.h"
#include "champlain-marker-layer.h"
#include "champlain-path-layer.h"
#include "champlain-location.h"
#include "champlain-coordinate.h"
#include "champlain-marker.h"
#include "champlain-label.h"
#include "champlain-scale.h"
#include "champlain-license.h"
#include "champlain-tile.h"
#include "champlain-map-source.h"
#include "champlain-map-source-chain.h"
#include "champlain-tile-source.h"
#include "champlain-tile-cache.h"
#include "champlain-memory-cache.h"
#include "champlain-network-tile-source.h"
#include "champlain-file-cache.h"
#include "champlain-map-source-factory.h"
#include "champlain-map-source-desc.h"
#include "champlain-renderer.h"
#include "champlain-image-renderer.h"
#include "champlain-error-tile-renderer.h"
#include "champlain-file-tile-source.h"
#include "champlain-null-tile-source.h"
#include "champlain-network-bbox-tile-source.h"
#include "champlain-adjustment.h"
#include "champlain-kinetic-scroll-view.h"
#include "champlain-viewport.h"
#include "champlain-bounding-box.h"
#include "champlain-exportable.h"

#define C_ENUM(v) ((gint) v)
#define C_FLAGS(v) ((guint) v)

/* enumerations from "champlain-map-source.h" */

GType
champlain_map_projection_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(CHAMPLAIN_MAP_PROJECTION_MERCATOR), "CHAMPLAIN_MAP_PROJECTION_MERCATOR", "mercator" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_enum_register_static ("ChamplainMapProjection", values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* enumerations from "champlain-marker-layer.h" */

GType
champlain_selection_mode_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(CHAMPLAIN_SELECTION_NONE), "CHAMPLAIN_SELECTION_NONE", "none" },
    { C_ENUM(CHAMPLAIN_SELECTION_SINGLE), "CHAMPLAIN_SELECTION_SINGLE", "single" },
    { C_ENUM(CHAMPLAIN_SELECTION_MULTIPLE), "CHAMPLAIN_SELECTION_MULTIPLE", "multiple" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_enum_register_static ("ChamplainSelectionMode", values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* enumerations from "champlain-scale.h" */

GType
champlain_unit_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(CHAMPLAIN_UNIT_KM), "CHAMPLAIN_UNIT_KM", "km" },
    { C_ENUM(CHAMPLAIN_UNIT_MILES), "CHAMPLAIN_UNIT_MILES", "miles" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_enum_register_static ("ChamplainUnit", values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* enumerations from "champlain-tile.h" */

GType
champlain_state_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(CHAMPLAIN_STATE_NONE), "CHAMPLAIN_STATE_NONE", "none" },
    { C_ENUM(CHAMPLAIN_STATE_LOADING), "CHAMPLAIN_STATE_LOADING", "loading" },
    { C_ENUM(CHAMPLAIN_STATE_LOADED), "CHAMPLAIN_STATE_LOADED", "loaded" },
    { C_ENUM(CHAMPLAIN_STATE_DONE), "CHAMPLAIN_STATE_DONE", "done" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_enum_register_static ("ChamplainState", values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* Generated data ends here */

