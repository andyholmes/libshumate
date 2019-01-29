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

#ifndef __SHUMATE_LOCATION_H__
#define __SHUMATE_LOCATION_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define SHUMATE_TYPE_LOCATION (shumate_location_get_type ())

#define SHUMATE_LOCATION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SHUMATE_TYPE_LOCATION, ShumateLocation))

#define SHUMATE_IS_LOCATION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SHUMATE_TYPE_LOCATION))

#define SHUMATE_LOCATION_GET_IFACE(inst) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), SHUMATE_TYPE_LOCATION, ShumateLocationIface))

typedef struct _ShumateLocation ShumateLocation; /* Dummy object */
typedef struct _ShumateLocationIface ShumateLocationIface;

/**
 * ShumateLocation:
 *
 * An interface common to objects having latitude and longitude.
 */

/**
 * ShumateLocationIface:
 * @get_latitude: virtual function for obtaining latitude.
 * @get_longitude: virtual function for obtaining longitude.
 * @set_location: virtual function for setting position.
 *
 * An interface common to objects having latitude and longitude.
 */
struct _ShumateLocationIface
{
  /*< private >*/
  GTypeInterface g_iface;

  /*< public >*/
  gdouble (*get_latitude)(ShumateLocation *location);
  gdouble (*get_longitude)(ShumateLocation *location);
  void (*set_location)(ShumateLocation *location,
      gdouble latitude,
      gdouble longitude);
};

GType shumate_location_get_type (void) G_GNUC_CONST;

void shumate_location_set_location (ShumateLocation *location,
    gdouble latitude,
    gdouble longitude);
gdouble shumate_location_get_latitude (ShumateLocation *location);
gdouble shumate_location_get_longitude (ShumateLocation *location);

G_END_DECLS

#endif /* __SHUMATE_LOCATION_H__ */