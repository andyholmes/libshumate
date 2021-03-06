#include <gtk/gtk.h>
#include <shumate/shumate.h>
#include "shumate/vector/shumate-vector-expression-literal-private.h"
#include "shumate/vector/shumate-vector-expression-interpolate-private.h"


static void
test_vector_expression_parse (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(JsonNode) node1 = json_from_string ("{\"stops\": [[12, 1], [13, 2], [14, 5], [16, 9]]}", NULL);
  g_autoptr(JsonNode) node2 = json_from_string ("1.0", NULL);
  g_autoptr(ShumateVectorExpression) expr1 = NULL;
  g_autoptr(ShumateVectorExpression) expr2 = NULL;
  g_autoptr(ShumateVectorExpression) expr3 = NULL;

  expr1 = shumate_vector_expression_from_json (node1, &error);
  g_assert_no_error (error);
  g_assert_true (SHUMATE_IS_VECTOR_EXPRESSION_INTERPOLATE (expr1));

  expr2 = shumate_vector_expression_from_json (node2, &error);
  g_assert_no_error (error);
  g_assert_true (SHUMATE_IS_VECTOR_EXPRESSION_LITERAL (expr2));

  expr3 = shumate_vector_expression_from_json (NULL, &error);
  g_assert_no_error (error);
  g_assert_true (SHUMATE_IS_VECTOR_EXPRESSION_LITERAL (expr3));
}


static void
test_vector_expression_literal (void)
{
  g_auto(ShumateVectorValue) value = SHUMATE_VECTOR_VALUE_INIT;
  g_autoptr(ShumateVectorExpression) expr = NULL;
  double result;

  shumate_vector_value_set_number (&value, 3.1415);
  expr = shumate_vector_expression_literal_new (&value);

  result = shumate_vector_expression_eval_number (expr, NULL, -10);
  g_assert_cmpfloat (3.1415, ==, result);
}


static void
test_vector_expression_interpolate (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(JsonNode) node = json_from_string ("{\"stops\": [[12, 1], [13, 2], [14, 5], [16, 9]]}", NULL);
  g_autoptr(ShumateVectorExpression) expression;
  ShumateVectorRenderScope scope;

  expression = shumate_vector_expression_from_json (node, &error);
  g_assert_no_error (error);

  /* Test that exact stop values work */
  scope.zoom_level = 12;
  g_assert_cmpfloat (1.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));
  scope.zoom_level = 13;
  g_assert_cmpfloat (2.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));
  scope.zoom_level = 14;
  g_assert_cmpfloat (5.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));
  scope.zoom_level = 16;
  g_assert_cmpfloat (9.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));

  /* Test that outlier values work */
  scope.zoom_level = 1;
  g_assert_cmpfloat (1.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));
  scope.zoom_level = 100;
  g_assert_cmpfloat (9.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));

  /* Test that in-between values work */
  scope.zoom_level = 12.5;
  g_assert_cmpfloat (1.5, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));
  scope.zoom_level = 15;
  g_assert_cmpfloat (7.0, ==, shumate_vector_expression_eval_number (expression, &scope, -10000.0));
}


static void
test_vector_expression_interpolate_color (void)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(JsonNode) node = json_from_string ("{\"stops\": [[12, \"#00224466\"], [13, \"#88AACCEE\"]]}", NULL);
  g_autoptr(ShumateVectorExpression) expression;
  ShumateVectorRenderScope scope;
  GdkRGBA color, correct_color;

  expression = shumate_vector_expression_from_json (node, &error);
  g_assert_no_error (error);

  /* Test that exact stop values work */
  scope.zoom_level = 12;
  shumate_vector_expression_eval_color (expression, &scope, &color);
  gdk_rgba_parse (&correct_color, "#00224466");
  g_assert_true (gdk_rgba_equal (&color, &correct_color));

  scope.zoom_level = 12.5;
  shumate_vector_expression_eval_color (expression, &scope, &color);
  gdk_rgba_parse (&correct_color, "#446688AA");
  g_assert_true (gdk_rgba_equal (&color, &correct_color));

  scope.zoom_level = 13;
  shumate_vector_expression_eval_color (expression, &scope, &color);
  gdk_rgba_parse (&correct_color, "#88AACCEE");
  g_assert_true (gdk_rgba_equal (&color, &correct_color));
}


static gboolean
filter_with_scope (ShumateVectorRenderScope *scope, const char *filter)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(JsonNode) node = json_from_string (filter, NULL);
  g_autoptr(ShumateVectorExpression) expression = shumate_vector_expression_from_json (node, &error);

  g_assert_no_error (error);

  return shumate_vector_expression_eval_boolean (expression, scope, FALSE);
}


static gboolean
filter (const char *filter)
{
  return filter_with_scope (NULL, filter);
}


static void
test_vector_expression_basic_filter (void)
{
  g_assert_true  (filter ("true"));
  g_assert_false (filter ("false"));
  g_assert_false (filter ("[\"!\", true]"));
  g_assert_true  (filter ("[\"!\", false]"));
  g_assert_true  (filter ("[\"any\", false, true]"));
  g_assert_false (filter ("[\"any\", false, false]"));
  g_assert_true  (filter ("[\"none\", false, false]"));
  g_assert_false (filter ("[\"none\", true, false]"));
  g_assert_true  (filter ("[\"all\", true, true]"));
  g_assert_false (filter ("[\"all\", false, true]"));

  g_assert_false (filter ("[\"any\"]"));
  g_assert_true  (filter ("[\"none\"]"));
  g_assert_true  (filter ("[\"all\"]"));

  g_assert_true  (filter ("[\"in\", 10, 20, 10, 13]"));
  g_assert_true  (filter ("[\"!in\", 10, 20, 0, 13]"));

  g_assert_true  (filter ("[\"==\", 10, 10]"));
  g_assert_false (filter ("[\"==\", 10, 20]"));
  g_assert_false (filter ("[\"==\", 10, \"10\"]"));
  g_assert_false (filter ("[\"!=\", 10, 10]"));
  g_assert_true  (filter ("[\"!=\", 10, 20]"));
  g_assert_true  (filter ("[\"!=\", 10, \"10\"]"));
  g_assert_true  (filter ("[\">\", 20, 10]"));
  g_assert_false (filter ("[\">\", 10, 10]"));
  g_assert_false (filter ("[\">\", 5, 10]"));
  g_assert_true  (filter ("[\"<\", 10, 20]"));
  g_assert_false (filter ("[\"<\", 10, 10]"));
  g_assert_false (filter ("[\"<\", 10, 5]"));
  g_assert_true  (filter ("[\">=\", 20, 10]"));
  g_assert_true  (filter ("[\">=\", 10, 10]"));
  g_assert_false (filter ("[\">=\", 5, 10]"));
  g_assert_true  (filter ("[\"<=\", 10, 20]"));
  g_assert_true  (filter ("[\"<=\", 10, 10]"));
  g_assert_false (filter ("[\"<=\", 10, 5]"));
}


static void
test_vector_expression_feature_filter (void)
{
  GError *error = NULL;
  g_autoptr(GBytes) vector_data = NULL;
  gconstpointer data;
  gsize len;
  ShumateVectorRenderScope scope;

  vector_data = g_resources_lookup_data ("/org/gnome/shumate/Tests/0.pbf", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
  g_assert_no_error (error);

  data = g_bytes_get_data (vector_data, &len);
  scope.tile = vector_tile__tile__unpack (NULL, len, data);
  g_assert_nonnull (scope.tile);

  scope.zoom_level = 10;

  g_assert_true (shumate_vector_render_scope_find_layer (&scope, "helloworld"));
  scope.feature = scope.layer->features[0];

  g_assert_true  (filter_with_scope (&scope, "[\"==\", \"name\", \"Hello, world!\"]"));
  g_assert_false (filter_with_scope (&scope, "[\"==\", \"name\", \"Goodbye, world!\"]"));
  g_assert_true  (filter_with_scope (&scope, "[\"has\", \"name\"]"));
  g_assert_false (filter_with_scope (&scope, "[\"!has\", \"name\"]"));
  g_assert_false (filter_with_scope (&scope, "[\"has\", \"name:en\"]"));
  g_assert_true  (filter_with_scope (&scope, "[\"!has\", \"name:en\"]"));
  g_assert_true  (filter_with_scope (&scope, "[\"==\", \"$type\", \"Point\"]"));
  g_assert_true  (filter_with_scope (&scope, "[\"==\", \"zoom\", 10]"));
}


static void
filter_expect_error (const char *filter)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(JsonNode) node = json_from_string (filter, NULL);
  g_autoptr(ShumateVectorExpression) expression = shumate_vector_expression_from_json (node, &error);

  g_assert_error (error, SHUMATE_STYLE_ERROR, SHUMATE_STYLE_ERROR_INVALID_EXPRESSION);
  g_assert_null (expression);
}

static void
test_vector_expression_filter_errors (void)
{
  filter_expect_error ("[\"not an operator\"]");
  filter_expect_error ("[\"in\"]");
  filter_expect_error ("[\"==\", 0, 1, 2]");
  filter_expect_error ("[]");
  filter_expect_error ("[[]]");
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/vector/expression/parse", test_vector_expression_parse);
  g_test_add_func ("/vector/expression/literal", test_vector_expression_literal);
  g_test_add_func ("/vector/expression/interpolate", test_vector_expression_interpolate);
  g_test_add_func ("/vector/expression/interpolate-color", test_vector_expression_interpolate_color);
  g_test_add_func ("/vector/expression/basic-filter", test_vector_expression_basic_filter);
  g_test_add_func ("/vector/expression/feature-filter", test_vector_expression_feature_filter);
  g_test_add_func ("/vector/expression/filter-errors", test_vector_expression_filter_errors);

  return g_test_run ();
}
