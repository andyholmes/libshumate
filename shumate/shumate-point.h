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

#if !defined (__SHUMATE_SHUMATE_H_INSIDE__) && !defined (SHUMATE_COMPILATION)
#error "Only <shumate/shumate.h> can be included directly."
#endif

#ifndef SHUMATE_POINT_H
#define SHUMATE_POINT_H

#include <shumate/shumate-marker.h>

#include <gdk/gdk.h>
#include <cairo-gobject.h>

G_BEGIN_DECLS

#define SHUMATE_TYPE_POINT shumate_point_get_type ()
G_DECLARE_DERIVABLE_TYPE (ShumatePoint, shumate_point, SHUMATE, POINT, ShumateMarker)

/**
 * ShumatePoint:
 *
 * The #ShumatePoint structure contains only private data
 * and should be accessed using the provided API
 */

struct _ShumatePointClass
{
  ShumateMarkerClass parent_class;
};


ShumatePoint *shumate_point_new (void);

ShumatePoint *shumate_point_new_full (gdouble size,
    const GdkRGBA *color);

void shumate_point_set_color (ShumatePoint *point,
    const GdkRGBA *color);
GdkRGBA *shumate_point_get_color (ShumatePoint *point);

void shumate_point_set_size (ShumatePoint *point,
    gdouble size);
gdouble shumate_point_get_size (ShumatePoint *point);

cairo_surface_t *shumate_point_get_surface (ShumatePoint *point);
void shumate_point_set_surface (ShumatePoint *point,
    cairo_surface_t *surface);

G_END_DECLS

#endif
