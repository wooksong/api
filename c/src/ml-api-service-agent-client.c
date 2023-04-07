/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright (c) 2022 Samsung Electronics Co., Ltd. All Rights Reserved.
 *
 * @file ml-api-service-agent-client.c
 * @date 20 Jul 2022
 * @brief agent (dbus) implementation of NNStreamer/Service C-API
 * @see https://github.com/nnstreamer/nnstreamer
 * @author Yongjoo Ahn <yongjoo1.ahn@samsung.com>
 * @bug No known bugs except for NYI items
 */

#include <glib/gstdio.h>
#include <json-glib/json-glib.h>

#include "ml-api-internal.h"
#include "ml-api-service.h"
#include "ml-api-service-private.h"
#include "../../daemon/includes/ml-agent-dbus-interface.h"

/**
 * @brief TBU
 */
int
ml_service_set_pipeline (const char *name, const char *pipeline_desc)
{
  int ret = ML_ERROR_NONE;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name) {
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string.");
  }

  if (!pipeline_desc) {
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'pipeline_desc' is NULL. It should be a valid string.");
  }

  ret = ml_agent_dbus_interface_set_pipeline (name, pipeline_desc, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method set_pipeline (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Get the pipeline description with a given name.
 */
int
ml_service_get_pipeline (const char *name, char **pipeline_desc)
{
  int ret = ML_ERROR_NONE;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name) {
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL, It should be a valid string");
  }

  if (!pipeline_desc) {
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter 'pipeline_desc'. It should be a valid char**");
  }

  g_critical
      ("[ml-api-service-agent-client:ml_service_get_pipeline] ml_agent_dbus_interface_get_pipeline\n");
  ret = ml_agent_dbus_interface_get_pipeline (name, pipeline_desc, &err);
  g_critical
      ("[ml-api-service-agent-client:ml_service_get_pipeline] ret = %d\n", ret);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method get_pipeline (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Delete the pipeline description with a given name.
 */
int
ml_service_delete_pipeline (const char *name)
{
  int ret = ML_ERROR_NONE;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name) {
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL, It should be a valid string");
  }

  ret = ml_agent_dbus_interface_delete_pipeline (name, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method delete_pipeline (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Launch the pipeline of given service.
 */
int
ml_service_launch_pipeline (const char *name, ml_service_h * h)
{
  int ret = ML_ERROR_NONE;
  GError *err = NULL;
  ml_service_s *mls;
  _ml_service_server_s *server;
  gint64 out_id;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!h)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'h' is NULL. It should be a valid ml_service_h");

  ret = ml_agent_dbus_interface_launch_pipeline (name, &out_id, &err);
  if (ret < 0) {
    g_autofree gchar *msg_detail;

    msg_detail =
        g_strdup_printf ("Failed to invoke the method launch_pipeline (%s).",
        (err ? err->message : "Unknown error"));
    g_clear_error (&err);
    _ml_error_report_return (ret, "%s", msg_detail);
  }

  mls = g_new0 (ml_service_s, 1);
  server = g_new0 (_ml_service_server_s, 1);
  if (server == NULL || mls == NULL) {
    g_free (mls);
    g_free (server);
    _ml_error_report_return (ML_ERROR_OUT_OF_MEMORY,
        "Failed to allocate memory for the service_server. Out of memory?");
  }

  server->id = out_id;
  server->service_name = g_strdup (name);

  mls->type = ML_SERVICE_TYPE_SERVER_PIPELINE;
  mls->priv = server;
  *h = mls;

  return ML_ERROR_NONE;
}

/**
 * @brief Start the pipeline of given ml_service_h
 */
int
ml_service_start_pipeline (ml_service_h h)
{
  int ret = ML_ERROR_NONE;
  ml_service_s *mls = (ml_service_s *) h;
  _ml_service_server_s *server;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!h)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'h' is NULL. It should be a valid ml_service_h");

  server = (_ml_service_server_s *) mls->priv;
  ret = ml_agent_dbus_interface_start_pipeline (server->id, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method start_pipeline (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Stop the pipeline of given ml_service_h
 */
int
ml_service_stop_pipeline (ml_service_h h)
{
  int ret = ML_ERROR_NONE;
  ml_service_s *mls = (ml_service_s *) h;
  _ml_service_server_s *server;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!h)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'h' is NULL. It should be a valid ml_service_h");

  server = (_ml_service_server_s *) mls->priv;
  ret = ml_agent_dbus_interface_stop_pipeline (server->id, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method stop_pipeline (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Return state of given ml_service_h
 */
int
ml_service_get_pipeline_state (ml_service_h h, ml_pipeline_state_e * state)
{
  int ret = ML_ERROR_NONE;
  gint _state = ML_PIPELINE_STATE_UNKNOWN;
  ml_service_s *mls = (ml_service_s *) h;
  _ml_service_server_s *server;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!h)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'h' is NULL. It should be a valid ml_service_h");

  if (!state)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'state' is NULL. It should be a valid ml_pipeline_state_e pointer");

  server = (_ml_service_server_s *) mls->priv;
  ret = ml_agent_dbus_interface_get_pipeline_state (server->id, &_state, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method get_state (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  *state = (ml_pipeline_state_e) _state;
  return ret;
}

/**
 * @brief Registers new information of a neural network model.
 */
int
ml_service_model_register (const char *name, const char *path,
    const bool activate, const char *description, unsigned int *version)
{
  int ret = ML_ERROR_NONE;

  g_autofree gchar *dir_name = NULL;
  GError *err = NULL;
  GStatBuf statbuf;

  check_feature_state (ML_FEATURE_SERVICE);

  g_critical ("####### namn");

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  g_critical ("####### path");
  if (!path)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'path' is NULL. It should be a valid string");

  g_critical ("####### version");
  if (!version)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'version' is NULL. It should be a valid unsigned int pointer");

  g_critical ("ml_service_model_register >>>>>>>>>>>>>> dir_name");
  dir_name = g_path_get_dirname (path);
  ret = g_stat (dir_name, &statbuf);
  if (ret != 0) {
    _ml_error_report_return (ML_ERROR_PERMISSION_DENIED,
        "Failed to get the information of the model file '%s'.", path);
  }

  if (!g_path_is_absolute (path) ||
      !g_file_test (path, (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) ||
      g_file_test (path, G_FILE_TEST_IS_SYMLINK))
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The model file '%s' is not a regular file.", path);
  g_critical
      ("ml_service_model_register >>>>>>>>>>>>>> ml_agent_dbus_interface_model_register");

  ret =
      ml_agent_dbus_interface_model_register (name, path,
      activate, description ? description : "", version, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method register (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Updates the description of neural network model with given @a name and @a version.
 */
int
ml_service_model_update_description (const char *name,
    const unsigned int version, const char *description)
{
  int ret = ML_ERROR_NONE;

  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  if (version == 0U)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'version' is 0. It should be a valid unsigned int");

  if (!description)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'description' is NULL. It should be a valid string");

  ret =
      ml_agent_dbus_interface_model_update_description (name, version,
      description, &err);

  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method update_description (%s).",
        err ? err->message : "Unknown error");
  }
  g_clear_error (&err);

  return ret;
}

/**
 * @brief Activates a neural network model with given @a name and @a version.
 */
int
ml_service_model_activate (const char *name, const unsigned int version)
{
  int ret = ML_ERROR_NONE;

  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  if (version == 0U)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'version' is 0. It should be a valid unsigned int");

  ret = ml_agent_dbus_interface_model_activate(name, version, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method activate (%s).",
        err ? err->message : "Unknown error");
  }

  g_clear_error (&err);

  return ret;
}

/**
 * @brief Gets the information of neural network model with given @a name and @a version.
 */
int
ml_service_model_get (const char *name, const unsigned int version,
    ml_option_h * info)
{
  int ret = ML_ERROR_NONE;

  ml_option_h _info = NULL;
  GError *err = NULL;
  gchar *description = NULL;

  JsonParser *parser = NULL;
  JsonObjectIter iter;
  JsonNode *root_node;
  JsonNode *member_node;
  const gchar *member_name;
  JsonObject *j_object;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  if (!info)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'info' is NULL. It should be a valid pointer to ml_info_h");

  ret = ml_agent_dbus_interface_model_get(name, version, &description, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method get_activated (%s).",
        err ? err->message : "Unknown error");
    g_clear_error (&err);
    goto error;
  }

  if (ML_ERROR_NONE != ret || !description) {
    goto error;
  }

  ret = ml_option_create (&_info);
  if (ML_ERROR_NONE != ret) {
    goto error;
  }

  /* fill ml_info */
  parser = json_parser_new ();
  if (!parser) {
    _ml_error_report
        ("Failed to allocate memory for JsonParser. Out of memory?");
    ret = ML_ERROR_OUT_OF_MEMORY;
    goto error;
  }

  if (!json_parser_load_from_data (parser, description, -1, &err)) {
    _ml_error_report ("Failed to parse the json string. %s", err->message);
    g_clear_error (&err);
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  root_node = json_parser_get_root (parser);
  if (!root_node) {
    _ml_error_report ("Failed to get the root node of json string.");
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  j_object = json_node_get_object (root_node);
  json_object_iter_init_ordered (&iter, j_object);
  while (json_object_iter_next_ordered (&iter, &member_name, &member_node)) {
    const gchar *value = json_object_get_string_member (j_object, member_name);
    ml_option_set (_info, member_name, g_strdup (value), g_free);
  }

  *info = _info;

error:
  if (parser)
    g_object_unref (parser);
  g_free (description);

  if (ret != ML_ERROR_NONE) {
    if (_info)
      ml_option_destroy (_info);
  }

  return ret;
}

/**
 * @brief Gets the information of activated neural network model with given @a name.
 */
int
ml_service_model_get_activated (const char *name, ml_option_h * info)
{
  int ret = ML_ERROR_NONE;

  ml_option_h _info = NULL;
  GError *err = NULL;
  gchar *description = NULL;

  JsonParser *parser = NULL;
  JsonObjectIter iter;
  JsonNode *root_node;
  JsonNode *member_node;
  const gchar *member_name;
  JsonObject *j_object;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  if (!info)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'info' is NULL. It should be a valid pointer to ml_info_h");

  ret = ml_agent_dbus_interface_model_get_activated(name, &description, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method get_activated (%s).",
        err ? err->message : "Unknown error");
    g_clear_error (&err);
    goto error;
  }

  if (ML_ERROR_NONE != ret || !description) {
    goto error;
  }

  ret = ml_option_create (&_info);
  if (ML_ERROR_NONE != ret) {
    goto error;
  }

  /* fill ml_info */
  parser = json_parser_new ();
  if (!parser) {
    _ml_error_report
        ("Failed to allocate memory for JsonParser. Out of memory?");
    ret = ML_ERROR_OUT_OF_MEMORY;
    goto error;
  }

  if (!json_parser_load_from_data (parser, description, -1, &err)) {
    _ml_error_report ("Failed to parse the json string. %s", err->message);
    g_clear_error (&err);
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  root_node = json_parser_get_root (parser);
  if (!root_node) {
    _ml_error_report ("Failed to get the root node of json string.");
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  j_object = json_node_get_object (root_node);
  json_object_iter_init_ordered (&iter, j_object);
  while (json_object_iter_next_ordered (&iter, &member_name, &member_node)) {
    const gchar *value = json_object_get_string_member (j_object, member_name);
    ml_option_set (_info, member_name, g_strdup (value), g_free);
  }

  *info = _info;

error:
  if (parser)
    g_object_unref (parser);
  g_free (description);

  if (ret != ML_ERROR_NONE) {
    if (_info)
      ml_option_destroy (_info);
  }

  return ret;
}

/**
 * @brief Gets the list of neural network model with given @a name.
 */
int
ml_service_model_get_all (const char *name, ml_option_h * info_list[],
    unsigned int *num)
{
  int ret = ML_ERROR_NONE;

  ml_option_h *_info_list = NULL;
  GError *err = NULL;
  gchar *description = NULL;
  guint i, n;

  JsonParser *parser = NULL;
  JsonArray *array;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  if (!info_list)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'info' is NULL. It should be a valid pointer to array of ml_info_h");

  if (!num)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'num' is NULL. It should be a valid pointer to unsigned int");

  *num = 0;

  ret = ml_agent_dbus_interface_model_get_all(name, &description, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method get_activated (%s).",
        err ? err->message : "Unknown error");
    g_clear_error (&err);
    goto error;
  }

  if (ML_ERROR_NONE != ret || !description) {
    goto error;
  }

  parser = json_parser_new ();
  if (!parser) {
    _ml_error_report
        ("Failed to allocate memory for JsonParser. Out of memory?");
    ret = ML_ERROR_OUT_OF_MEMORY;
    goto error;
  }

  if (!json_parser_load_from_data (parser, description, -1, &err)) {
    _ml_error_report ("Failed to parse the json string. %s", err->message);
    g_clear_error (&err);
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  array = json_node_get_array (json_parser_get_root (parser));
  if (!array) {
    _ml_error_report ("Failed to get array from json string.");
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  n = json_array_get_length (array);
  if (n == 0U) {
    _ml_error_report ("Failed to get array from json string.");
    ret = ML_ERROR_INVALID_PARAMETER;
    goto error;
  }

  _info_list = g_new0 (ml_option_h, n);
  if (!_info_list) {
    _ml_error_report
        ("Failed to allocate memory for list of ml_info_h. Out of memory?");
    ret = ML_ERROR_OUT_OF_MEMORY;
    goto error;
  }

  for (i = 0; i < n; i++) {
    JsonObjectIter iter;
    const gchar *member_name;
    JsonNode *member_node;
    JsonObject *object;

    if (ml_option_create (&_info_list[i]) != ML_ERROR_NONE) {
      _ml_error_report
          ("Failed to allocate memory for ml_option_h. Out of memory?");
      n = i;
      ret = ML_ERROR_OUT_OF_MEMORY;
      goto error;
    }

    object = json_array_get_object_element (array, i);
    json_object_iter_init_ordered (&iter, object);
    while (json_object_iter_next_ordered (&iter, &member_name, &member_node)) {
      const gchar *value = json_object_get_string_member (object, member_name);
      ml_option_set (_info_list[i], member_name, g_strdup (value), g_free);
    }
  }

  *info_list = _info_list;
  *num = n;

error:
  if (parser)
    g_object_unref (parser);
  g_free (description);

  if (ret != ML_ERROR_NONE) {
    if (_info_list) {
      for (i = 0; i < n; i++) {
        if (_info_list[i])
          ml_option_destroy (_info_list[i]);
      }

      g_free (_info_list);
    }
  }

  return ret;
}

/**
 * @brief Deletes a model information with given @a name and @a version from machine learning service.
 */
int
ml_service_model_delete (const char *name, const unsigned int version)
{
  int ret = ML_ERROR_NONE;
  GError *err = NULL;

  check_feature_state (ML_FEATURE_SERVICE);

  if (!name)
    _ml_error_report_return (ML_ERROR_INVALID_PARAMETER,
        "The parameter, 'name' is NULL. It should be a valid string");

  ret = ml_agent_dbus_interface_model_delete(name, version, &err);
  if (ret < 0) {
    _ml_error_report ("Failed to invoke the method delete (%s).",
        err ? err->message : "Unknown error");
  }

  g_clear_error (&err);

  return ret;
}
