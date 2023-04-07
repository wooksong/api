/**
 * @file    ml-agent-dbus-interface.h
 * @date    5 April 2023
 * @brief
 * @see     https://github.com/nnstreamer/api
 * @author  Wook Song <wook16.song@samsung.com>
 * @bug     No known bugs except for NYI items
 *
 * @details
 *
 */

#ifndef __ML_AGENT_DBUS_INTERFACE_H__
#define __ML_AGENT_DBUS_INTERFACE_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <glib.h>

gint ml_agent_dbus_interface_set_pipeline (const gchar *name, const gchar *pipeline_desc, GError **err);
gint ml_agent_dbus_interface_get_pipeline (const gchar *name, gchar **pipeline_desc, GError **err);
gint ml_agent_dbus_interface_delete_pipeline (const gchar * name, GError ** err);
gint ml_agent_dbus_interface_launch_pipeline(const gchar *name, gint64 *id, GError ** err);
gint ml_agent_dbus_interface_start_pipeline (gint64 id, GError ** err);
gint ml_agent_dbus_interface_stop_pipeline (gint64 id, GError ** err);
gint ml_agent_dbus_interface_destroy_pipeline (gint64 id, GError ** err);
gint ml_agent_dbus_interface_get_pipeline_state (gint64 id, gint * state, GError ** err);

gint ml_agent_dbus_interface_model_register(const gchar *name, const gchar *path, const gboolean activate, const gchar *description, guint *version, GError ** err);
gint ml_agent_dbus_interface_model_update_description (const gchar *name, const guint version, const gchar *description, GError ** err);
gint ml_agent_dbus_interface_model_activate(const gchar *name, const guint version, GError ** err);
gint ml_agent_dbus_interface_model_get(const gchar *name, const guint version, gchar ** description, GError ** err);
gint ml_agent_dbus_interface_model_get_activated(const gchar *name, gchar ** description, GError ** err);
gint ml_agent_dbus_interface_model_get_all(const gchar *name, gchar ** description, GError ** err);
gint ml_agent_dbus_interface_model_delete(const gchar *name, const guint version, GError ** err);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __ML_AGENT_DBUS_INTERFACE_H__ */
