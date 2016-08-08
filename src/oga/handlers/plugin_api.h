
#include <oga/base/types.hpp>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif
	enum oga_api_error_codes {
		OGA_SUCCESS = 0,
		OGA_INVALID_TYPE,
		OGA_OUT_OF_MEMORY,
		OGA_INVALID_PARAMETER,
		OGA_OUT_OF_BOUNDS,
		OGA_NO_SUCH_ITEM,
		OGA_NOT_IMPLEMENTED,
		OGA_LAST_ERROR_CODE
	};

	enum oga_value_type {
		OGA_VALUE_TYPE_NULL = 0, // oga::proto::json::vt_null
		OGA_VALUE_TYPE_STRING = 1, //oga::proto::json::vt_string,
		OGA_VALUE_TYPE_DOUBLE = 2, // oga::proto::json::vt_double,
		OGA_VALUE_TYPE_INTEGER = 3, // oga::proto::json::vt_integer,
		OGA_VALUE_TYPE_BOOLEAN = 4, // oga::proto::json::vt_boolean,
		OGA_VALUE_TYPE_ARRAY = 5, // oga::proto::json::vt_array,
		OGA_VALUE_TYPE_OBJECT = 6, // oga::proto::json::vt_object
		OGA_VALUE_TYPE_LAST
	};

	typedef oga_int32_t oga_error_t;

	struct oga_handle_t;

	struct oga_plugin_t;

	struct oga_object_t;
	struct oga_array_t;
	struct oga_value_t;


	struct oga_timer_data {
		oga_int32_t interval;
		void(*callback)(oga_timer_data*);
		void(*free)(oga_timer_data*);
	};

	struct oga_command_data {
		char const * name;
		void(*callback)(oga_command_data*, oga_object_t * args);
		void(*free)(oga_command_data*);
	};


	struct oga_value_api {
		oga_error_t(*value_create_from_string)(oga_value_t ** result, oga_plugin_t *, char const * s, size_t length);
		oga_error_t(*value_create_from_double)(oga_value_t ** result, oga_plugin_t *, double value);
		oga_error_t(*value_create_from_int)(oga_value_t ** result, oga_plugin_t *, oga_int64_t value);

		oga_error_t(*value_create)(oga_value_t **, oga_plugin_t *); // Creates null
		oga_error_t(*value_free)(oga_value_t*);
		oga_error_t(*value_get_type)(oga_value_type * type, oga_value_t *);

		oga_error_t(*value_set_null)(oga_value_t*);
		oga_error_t(*value_set_string)(oga_value_t *, char const * s, size_t length);
		oga_error_t(*value_set_double)(oga_value_t *, double d);
		oga_error_t(*value_set_int)(oga_value_t *, oga_int64_t value);
		oga_error_t(*value_set_boolean)(oga_value_t *, oga_int8_t value);
		oga_error_t(*value_set_array)(oga_value_t *, oga_array_t* value);
		oga_error_t(*value_set_object)(oga_value_t *, oga_object_t* value);

		oga_error_t(*value_get_boolean)(oga_int8_t * result, oga_value_t *);
		oga_error_t(*value_get_int)(oga_int64_t * result, oga_value_t *);
		oga_error_t(*value_get_double)(double * result, oga_value_t *);
		oga_error_t(*value_get_string)(char const ** result, size_t * length, oga_value_t *);
		oga_error_t(*value_get_array)(oga_array_t ** result, oga_value_t *);
		oga_error_t(*value_get_object)(oga_object_t ** result, oga_value_t *);

		oga_error_t(*array_create)(oga_array_t **, oga_plugin_t *);
		oga_error_t(*array_append)(oga_array_t*, oga_value_t *);
		oga_error_t(*array_pop)(oga_array_t*);
		oga_error_t(*array_get_size)(size_t * result, oga_array_t*);
		oga_error_t(*array_get)(oga_value_t ** value, oga_array_t*, size_t index);

		oga_error_t(*object_create)(oga_object_t **, oga_plugin_t *);
		oga_error_t(*object_set)(oga_object_t*, char const * name, oga_value_t * value);
		oga_error_t(*object_has)(oga_object_t*, char const * name);
		oga_error_t(*object_get_size)(size_t * result, oga_object_t*);
		oga_error_t(*object_get)(oga_value_t ** result, oga_object_t*, char const * name);
		oga_error_t(*object_erase)(oga_object_t*, char const * name);
	};

	struct oga_plugin_api {
		oga_error_t(*register_timer)(oga_handle_t **, oga_plugin_t *, oga_timer_data*);
		oga_error_t(*register_command)(oga_handle_t **, oga_plugin_t *, oga_command_data*);
		oga_error_t(*unregister_handler)(oga_handle_t*);
	
		oga_error_t(*send_message)(oga_plugin_t*, char const * name, oga_object_t * args);
		oga_error_t(*log_message)(oga_plugin_t*, char const * message);

		oga_error_t(*get_api_version)(oga_int32_t * result, oga_plugin_t*);
		oga_error_t(*get_config)(oga_object_t ** result, oga_plugin_t*);

		oga_value_api(*get_value_api)();
	};


	oga_error_t oga_value_create_from_string(oga_value_t ** result, oga_plugin_t *, char const * s, size_t length);
	oga_error_t oga_value_create_from_double(oga_value_t ** result, oga_plugin_t *, double value);
	oga_error_t oga_value_create_from_int(oga_value_t ** result, oga_plugin_t *, oga_int64_t value);

	oga_error_t oga_value_create(oga_value_t **, oga_plugin_t *); // Creates null
	oga_error_t oga_value_free(oga_value_t*);
	oga_error_t oga_value_get_type(oga_value_type * type, oga_value_t *);

	oga_error_t oga_value_set_null(oga_value_t*);
	oga_error_t oga_value_set_string(oga_value_t *, char const * s, size_t length);
	oga_error_t oga_value_set_double(oga_value_t *, double d);
	oga_error_t oga_value_set_int(oga_value_t *, oga_int64_t value);
	oga_error_t oga_value_set_boolean(oga_value_t *, oga_int8_t value);
	oga_error_t oga_value_set_array(oga_value_t *, oga_array_t* value);
	oga_error_t oga_value_set_object(oga_value_t *, oga_object_t* value);
	
	oga_error_t oga_value_get_boolean(oga_int8_t * result, oga_value_t *);
	oga_error_t oga_value_get_int(oga_int64_t * result, oga_value_t *);
	oga_error_t oga_value_get_double(double * result, oga_value_t *);
	oga_error_t oga_value_get_string(char const ** result, size_t * length, oga_value_t *);	
	oga_error_t oga_value_get_array(oga_array_t ** result, oga_value_t *);
	oga_error_t oga_value_get_object(oga_object_t ** result, oga_value_t *);

	oga_error_t oga_array_create(oga_array_t **, oga_plugin_t *);
	oga_error_t oga_array_append(oga_array_t*, oga_value_t *);
	oga_error_t oga_array_pop(oga_array_t*);
	oga_error_t oga_array_get_size(size_t * result, oga_array_t*);
	oga_error_t oga_array_get(oga_value_t ** value, oga_array_t*, size_t index);

	oga_error_t oga_object_create(oga_object_t **, oga_plugin_t *);
	oga_error_t oga_object_set(oga_object_t*, char const * name, oga_value_t * value);
	oga_error_t oga_object_has(oga_object_t*, char const * name);
	oga_error_t oga_object_get_size(size_t * result, oga_object_t*);
	oga_error_t oga_object_get(oga_value_t ** result, oga_object_t*, char const * name);
	oga_error_t oga_object_erase(oga_object_t*, char const * name);

#if defined(__cplusplus)
}
#endif
