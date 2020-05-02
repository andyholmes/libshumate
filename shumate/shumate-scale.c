/*
 * Copyright (C) 2011-2013 Jiri Techet <techet@gmail.com>
 * Copyright (C) 2019 Marcus Lundblad <ml@update.uu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * SECTION:shumate-scale
 * @short_description: An actor displaying a scale.
 *
 * An actor displaying a scale.
 */

#include "config.h"

#include "shumate-scale.h"
#include "shumate-defines.h"
#include "shumate-marshal.h"
#include "shumate-private.h"
#include "shumate-enum-types.h"
#include "shumate-view.h"

#include <glib.h>
#include <glib-object.h>
#include <cairo.h>
#include <math.h>
#include <string.h>


enum
{
  /* normal signals */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SCALE_UNIT,
  PROP_MAX_SCALE_WIDTH,
};

/* static guint shumate_scale_signals[LAST_SIGNAL] = { 0, }; */

struct _ShumateScale
{
  GObject parent_instance;

  ShumateUnit scale_unit;
  guint max_scale_width;
  gfloat text_height;
  //ClutterContent *canvas;

  ShumateView *view;
  gboolean redraw_scheduled;
};

G_DEFINE_TYPE (ShumateScale, shumate_scale, G_TYPE_OBJECT);

#define SCALE_HEIGHT  5
#define GAP_SIZE 2
#define SCALE_INSIDE_PADDING 10
#define SCALE_LINE_WIDTH 2


static void
shumate_scale_get_property (GObject *object,
    guint prop_id,
    GValue *value,
    GParamSpec *pspec)
{
  ShumateScale *scale = SHUMATE_SCALE (object);

  switch (prop_id)
    {
    case PROP_MAX_SCALE_WIDTH:
      g_value_set_uint (value, scale->max_scale_width);
      break;

    case PROP_SCALE_UNIT:
      g_value_set_enum (value, scale->scale_unit);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
shumate_scale_set_property (GObject *object,
    guint prop_id,
    const GValue *value,
    GParamSpec *pspec)
{
  ShumateScale *scale = SHUMATE_SCALE (object);

  switch (prop_id)
    {
    case PROP_MAX_SCALE_WIDTH:
      shumate_scale_set_max_width (scale, g_value_get_uint (value));
      break;

    case PROP_SCALE_UNIT:
      shumate_scale_set_unit (scale, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
shumate_scale_dispose (GObject *object)
{
  ShumateScale *scale = SHUMATE_SCALE (object);

  if (scale->view)
    {
      shumate_scale_disconnect_view (SHUMATE_SCALE (object));
    }

  /*
  if (priv->canvas)
    {
      g_object_unref (priv->canvas);
      priv->canvas = NULL;
    }
   */

  G_OBJECT_CLASS (shumate_scale_parent_class)->dispose (object);
}

static void
shumate_scale_class_init (ShumateScaleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = shumate_scale_dispose;
  object_class->get_property = shumate_scale_get_property;
  object_class->set_property = shumate_scale_set_property;

  /**
   * ShumateScale:max-width:
   *
   * The size of the map scale on screen in pixels.
   */
  g_object_class_install_property (object_class,
      PROP_MAX_SCALE_WIDTH,
      g_param_spec_uint ("max-width",
          "The width of the scale",
          "The max width of the scale"
          "on screen",
          1,
          2000,
          100,
          G_PARAM_READWRITE));

  /**
   * ShumateScale:unit:
   *
   * The scale's units.
   */
  g_object_class_install_property (object_class,
      PROP_SCALE_UNIT,
      g_param_spec_enum ("unit",
          "The scale's unit",
          "The map scale's unit",
          SHUMATE_TYPE_UNIT,
          SHUMATE_UNIT_KM,
          G_PARAM_READWRITE));
}


static gboolean
redraw_scale (/*ClutterCanvas *canvas,*/
    cairo_t *cr,
    int w,
    int h,
    ShumateScale *scale)
{
  gboolean is_small_unit = TRUE;  /* indicates if using meters */
  //ClutterActor *text;
  gfloat width, height;
  gfloat m_per_pixel;
  gfloat scale_width = scale->max_scale_width;
  gchar *label;
  gfloat base;
  gfloat factor;
  gboolean final_unit = FALSE;
  gint zoom_level;
  gdouble lat, lon;
  gfloat offset;
  ShumateMapSource *map_source;

  if (!scale->view)
    return FALSE;

  zoom_level = shumate_view_get_zoom_level (scale->view);
  map_source = shumate_view_get_map_source (scale->view);
  lat = shumate_view_get_center_latitude (scale->view);
  lon = shumate_view_get_center_longitude (scale->view);
  m_per_pixel = shumate_map_source_get_meters_per_pixel (map_source,
        zoom_level, lat, lon);

  if (scale->scale_unit == SHUMATE_UNIT_MILES)
    m_per_pixel *= 3.28;  /* m_per_pixel is now in ft */

  /* This loop will find the pretty value to display on the scale.
   * It will be run once for metric units, and twice for imperials
   * so that both feet and miles have pretty numbers.
   */
  do
    {
      /* Keep the previous power of 10 */
      base = floor (log (m_per_pixel * scale_width) / log (10));
      base = pow (10, base);

      /* How many times can it be fitted in our max scale width */
      g_assert (base > 0);
      g_assert (m_per_pixel * scale_width / base > 0);
      scale_width /= m_per_pixel * scale_width / base;
      g_assert (scale_width > 0);
      factor = floor (scale->max_scale_width / scale_width);
      base *= factor;
      scale_width *= factor;

      if (scale->scale_unit == SHUMATE_UNIT_KM)
        {
          if (base / 1000.0 >= 1)
            {
              base /= 1000.0; /* base is now in km */
              is_small_unit = FALSE;
            }
          final_unit = TRUE; /* Don't need to recompute */
        }
      else if (scale->scale_unit == SHUMATE_UNIT_MILES)
        {
          if (is_small_unit && base / 5280.0 >= 1)
            {
              m_per_pixel /= 5280.0; /* m_per_pixel is now in miles */
              is_small_unit = FALSE;
              /* we need to recompute the base because 1000 ft != 1 mile */
            }
          else
            final_unit = TRUE;
        }
    } while (!final_unit);

  //text = clutter_container_find_child_by_name (CLUTTER_CONTAINER (scale), "scale-far-label");
  label = g_strdup_printf ("%g", base);
  /* Get only digits width for centering */
  //clutter_text_set_text (CLUTTER_TEXT (text), label);
  g_free (label);
  //clutter_actor_get_size (text, &width, NULL);
  /* actual label with unit */
  label = g_strdup_printf ("%g %s", base,
        scale->scale_unit == SHUMATE_UNIT_KM ?
        (is_small_unit ? "m" : "km") :
        (is_small_unit ? "ft" : "miles"));
  //clutter_text_set_text (CLUTTER_TEXT (text), label);
  g_free (label);
  //clutter_actor_set_position (text, ceil (scale_width - width / 2) + SCALE_INSIDE_PADDING, SCALE_INSIDE_PADDING);

  //text = clutter_container_find_child_by_name (CLUTTER_CONTAINER (scale), "scale-mid-label");
  label = g_strdup_printf ("%g", base / 2.0);
  /*
  clutter_text_set_text (CLUTTER_TEXT (text), label);
  clutter_actor_get_size (text, &width, &height);
  clutter_actor_set_position (text, ceil ((scale_width - width) / 2) + SCALE_INSIDE_PADDING, SCALE_INSIDE_PADDING);
   */
  g_free (label);

  /* Draw the line */
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_width (cr, SCALE_LINE_WIDTH);

  offset = SCALE_INSIDE_PADDING + scale->text_height + GAP_SIZE;

  /* First tick */
  cairo_move_to (cr, SCALE_INSIDE_PADDING, offset);
  cairo_line_to (cr, SCALE_INSIDE_PADDING, offset + SCALE_HEIGHT);
  cairo_stroke (cr);

  /* Line */
  cairo_move_to (cr, SCALE_INSIDE_PADDING, offset + SCALE_HEIGHT);
  cairo_line_to (cr, scale_width + SCALE_INSIDE_PADDING, offset + SCALE_HEIGHT);
  cairo_stroke (cr);

  /* Middle tick */
  cairo_move_to (cr, scale_width / 2 + SCALE_INSIDE_PADDING, offset);
  cairo_line_to (cr, scale_width / 2 + SCALE_INSIDE_PADDING, offset + SCALE_HEIGHT);
  cairo_stroke (cr);

  /* Last tick */
  cairo_move_to (cr, scale_width + SCALE_INSIDE_PADDING, offset);
  cairo_line_to (cr, scale_width + SCALE_INSIDE_PADDING, offset + SCALE_HEIGHT);
  cairo_stroke (cr);

  return FALSE;
}


static gboolean
invalidate_canvas (ShumateScale *layer)
{
  //clutter_content_invalidate (priv->canvas);
  layer->redraw_scheduled = FALSE;

  return FALSE;
}


static void
schedule_redraw (ShumateScale *layer)
{
  if (!layer->redraw_scheduled)
    {
      layer->redraw_scheduled = TRUE;
      g_idle_add_full (G_PRIORITY_HIGH + 50,
          (GSourceFunc) invalidate_canvas,
          g_object_ref (layer),
          (GDestroyNotify) g_object_unref);
    }
}


static void
create_scale (ShumateScale *scale)
{
  /*
  ClutterActor *text, *scale_actor;
  gfloat width, height;
  ShumateScalePrivate *priv = scale->priv;

  clutter_actor_destroy_all_children (CLUTTER_ACTOR (scale));

  text = clutter_text_new_with_text ("Sans 9", "X km");
  clutter_actor_set_name (text, "scale-far-label");
  clutter_actor_add_child (CLUTTER_ACTOR (scale), text);

  text = clutter_text_new_with_text ("Sans 9", "X");
  clutter_actor_set_name (text, "scale-mid-label");
  clutter_actor_add_child (CLUTTER_ACTOR (scale), text);

  text = clutter_text_new_with_text ("Sans 9", "0");
  clutter_actor_add_child (CLUTTER_ACTOR (scale), text);
  clutter_actor_get_size (text, &width, &priv->text_height);
  clutter_actor_set_position (text, SCALE_INSIDE_PADDING - ceil (width / 2), SCALE_INSIDE_PADDING);

  width = priv->max_scale_width + 2 * SCALE_INSIDE_PADDING;
  height = SCALE_HEIGHT + priv->text_height + GAP_SIZE + 2 * SCALE_INSIDE_PADDING;

  priv->canvas = clutter_canvas_new ();
  clutter_canvas_set_size (CLUTTER_CANVAS (priv->canvas), width, height);
  g_signal_connect (priv->canvas, "draw", G_CALLBACK (redraw_scale), scale);

  scale_actor = clutter_actor_new ();
  clutter_actor_set_size (scale_actor, width, height);
  clutter_actor_set_content (scale_actor, priv->canvas);
  clutter_actor_add_child (CLUTTER_ACTOR (scale), scale_actor);

  clutter_actor_set_opacity (CLUTTER_ACTOR (scale), 200);
   */

  schedule_redraw (scale);
}


static void
shumate_scale_init (ShumateScale *scale)
{
  scale->scale_unit = SHUMATE_UNIT_KM;
  scale->max_scale_width = 100;
  scale->view = NULL;
  scale->redraw_scheduled = FALSE;

  create_scale (scale);
}


/**
 * shumate_scale_new:
 *
 * Creates an instance of #ShumateScale.
 *
 * Returns: a new #ShumateScale.
 */
ShumateScale *
shumate_scale_new (void)
{
  return SHUMATE_SCALE (g_object_new (SHUMATE_TYPE_SCALE, NULL));
}


/**
 * shumate_scale_set_max_width:
 * @scale: a #ShumateScale
 * @value: the number of pixels
 *
 * Sets the maximum width of the scale on the screen in pixels
 */
void
shumate_scale_set_max_width (ShumateScale *scale,
    guint value)
{
  g_return_if_fail (SHUMATE_IS_SCALE (scale));

  scale->max_scale_width = value;
  create_scale (scale);
  g_object_notify (G_OBJECT (scale), "max-width");
}


/**
 * shumate_scale_set_unit:
 * @scale: a #ShumateScale
 * @unit: a #ShumateUnit
 *
 * Sets the scale unit.
 */
void
shumate_scale_set_unit (ShumateScale *scale,
    ShumateUnit unit)
{
  g_return_if_fail (SHUMATE_IS_SCALE (scale));

  scale->scale_unit = unit;
  g_object_notify (G_OBJECT (scale), "unit");
  schedule_redraw (scale);
}


/**
 * shumate_scale_get_max_width:
 * @scale: a #ShumateScale
 *
 * Gets the maximum scale width.
 *
 * Returns: The maximum scale width in pixels.
 */
guint
shumate_scale_get_max_width (ShumateScale *scale)
{
  g_return_val_if_fail (SHUMATE_IS_SCALE (scale), FALSE);

  return scale->max_scale_width;
}


/**
 * shumate_scale_get_unit:
 * @scale: a #ShumateScale
 *
 * Gets the unit used by the scale.
 *
 * Returns: The unit used by the scale
 */
ShumateUnit
shumate_scale_get_unit (ShumateScale *scale)
{
  g_return_val_if_fail (SHUMATE_IS_SCALE (scale), FALSE);

  return scale->scale_unit;
}


static void
redraw_scale_cb (G_GNUC_UNUSED GObject *gobject,
    G_GNUC_UNUSED GParamSpec *arg1,
    ShumateScale *scale)
{
  schedule_redraw (scale);
}


/**
 * shumate_scale_connect_view:
 * @scale: a #ShumateScale
 * @view: a #ShumateView
 *
 * This method connects to the necessary signals of #ShumateView to make the
 * scale adapt to the current latitude and longitude.
 */
void
shumate_scale_connect_view (ShumateScale *scale,
    ShumateView *view)
{
  g_return_if_fail (SHUMATE_IS_SCALE (scale));

  scale->view = g_object_ref (view);
  g_signal_connect (view, "notify::latitude",
      G_CALLBACK (redraw_scale_cb), scale);
  schedule_redraw (scale);
}


/**
 * shumate_scale_disconnect_view:
 * @scale: a #ShumateScale
 *
 * This method disconnects from the signals previously connected by shumate_scale_connect_view().
 */
void
shumate_scale_disconnect_view (ShumateScale *scale)
{
  g_return_if_fail (SHUMATE_IS_SCALE (scale));

  g_signal_handlers_disconnect_by_func (scale->view,
      redraw_scale_cb,
      scale);
  g_clear_object (&scale->view);
}
