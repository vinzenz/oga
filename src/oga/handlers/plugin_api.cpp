
#include <oga/handlers/plugin_api.h>
#include <oga/proto/json/json.hpp>
#include <oga/core/agent.hpp>
#include <oga/handlers/handler.hpp>

extern "C" {

	struct oga_plugin_t {
		std::string plugin_name;
		std::string plugin_path;
		std::map<void*, oga::util::shared_ptr<oga_value_t> > allocations;
		oga::core::agent * agent;
		std::map<void*, oga::util::shared_ptr<oga_handle_t> > handles;
	};


	struct oga_value_t
	{
		oga_plugin_t * plugin;
		oga::proto::json::value value;
	};

	struct oga_object_t : oga_value_t
	{
	};

	struct oga_array_t : oga_value_t
	{
	};

	static oga_value_t * create_value(oga_plugin_t * plugin) {
		oga_value_t * ptr = new (std::nothrow) oga_value_t();
		if (ptr == 0) {
			return 0;
		}
		ptr->plugin = plugin;
		plugin->allocations[ptr].reset(ptr);
		return ptr;
	}

	oga_error_t oga_value_create_from_string(
		oga_value_t ** result,
		oga_plugin_t * plugin,
		char const * s,
		size_t length) {
		oga_value_t * val = create_value(plugin);
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		*result = val;
		return oga_value_set_string(*result, s, length);
	}

	oga_error_t oga_value_create_from_double(
		oga_value_t ** result,
		oga_plugin_t * plugin,
		double value) {
		oga_value_t * val = create_value(plugin);
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		*result = val;
		return oga_value_set_double(*result, value);
	}

	oga_error_t oga_value_create_from_int(
		oga_value_t ** result,
		oga_plugin_t * plugin,
		oga_int64_t value) {
		oga_value_t * val = create_value(plugin);
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		*result = val;
		return oga_value_set_int(*result, value);
	}

	oga_error_t oga_value_create(
		oga_value_t ** result,
		oga_plugin_t * plugin) {
		oga_value_t * val = create_value(plugin);
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		*result = val;
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_free(
		oga_value_t* value) {
		if (!value) {
			return OGA_INVALID_PARAMETER;
		}
		if (value->plugin->allocations.count(value) > 0) {
			value->plugin->allocations.erase(value);
		}
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_get_type(
		oga_value_type * type,
		oga_value_t * value) {
		if (value) {
			*type = oga_value_type(value->value.type());
			return OGA_SUCCESS;
		}
		return OGA_INVALID_PARAMETER;
	}

	oga_error_t oga_value_set_null(oga_value_t* value) {
		if (!value) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = oga::proto::json::null();
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_set_string(
		oga_value_t * value,
		char const * s,
		size_t length) {
		if (!value) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = std::string(s, s + length);
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_set_double(
		oga_value_t * value,
		double d) {
		if (!value) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = d;
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_set_int(
		oga_value_t * value,
		oga_int64_t i) {
		if (!value) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = i;
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_set_boolean(
		oga_value_t * value,
		oga_int8_t b) {
		if (!value) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = b != 0;
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_set_array(
		oga_value_t * value,
		oga_array_t* arr) {
		if (!value || !arr) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = arr->value;
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_set_object(
		oga_value_t * value,
		oga_object_t* obj) {
		if (!value || !obj) {
			return OGA_INVALID_PARAMETER;
		}
		value->value = obj->value;
		return OGA_SUCCESS;
	}

	oga_error_t oga_value_get_boolean(
		oga_int8_t * result,
		oga_value_t * value) {
		if (!value || !result) {
			return OGA_INVALID_PARAMETER;
		}
		if (is_boolean(value->value)) {
			*result = value->value.get_boolean();
			return OGA_SUCCESS;
		}
		return OGA_INVALID_TYPE;
	}

	oga_error_t oga_value_get_int(
		oga_int64_t * result,
		oga_value_t * value) {
		if (!value || !result) {
			return OGA_INVALID_PARAMETER;
		}
		if (is_integer(value->value)) {
			*result = value->value.get_integer();
			return OGA_SUCCESS;
		}
		return OGA_INVALID_TYPE;
	}

	oga_error_t oga_value_get_double(
		double * result,
		oga_value_t * value) {
		if (!value || !result) {
			return OGA_INVALID_PARAMETER;
		}
		if(is_double(value->value)) {
			*result = value->value.get_double();
			return OGA_SUCCESS;
		}
		return OGA_INVALID_TYPE;
	}

	oga_error_t oga_value_get_string(
		char const ** result,
		size_t * length,
		oga_value_t * value) {
		if (!value || !result || !length) {
			return OGA_INVALID_PARAMETER;
		}
		if (is_string(value->value)) {
			std::string const & s = value->value.get_string();
			*result = s.c_str();
			*length = s.size();
			return OGA_SUCCESS;
		}
		return OGA_INVALID_TYPE;
	}

	oga_error_t oga_value_get_array(
		oga_array_t ** result,
		oga_value_t * value) {
		if (!value || !result) {
			return OGA_INVALID_PARAMETER;
		}
		if (is_array(value->value)) {
			*result = static_cast<oga_array_t*>(value);
			return OGA_SUCCESS;
		}
		return OGA_INVALID_TYPE;
	}

	oga_error_t oga_value_get_object(
		oga_object_t ** result,
		oga_value_t * value) {
		if (!value || !result) {
			return OGA_INVALID_PARAMETER;
		}
		if (is_object(value->value)) {
			*result = static_cast<oga_object_t*>(value);
			return OGA_SUCCESS;
		}
		return OGA_INVALID_TYPE;
	}

	oga_error_t oga_array_create(
		oga_array_t ** result,
		oga_plugin_t * plugin) {
		oga_array_t * val = static_cast<oga_array_t*>(create_value(plugin));
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		*result = val;
		val->value = oga::proto::json::array();
		return OGA_SUCCESS;
	}

	oga_error_t oga_array_append(
		oga_array_t * array,
		oga_value_t * value) {
		if (!array || !value) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_array(array->value)) {
			return OGA_INVALID_TYPE;
		}
		array->value.get_array().push_back(value->value);
		return OGA_SUCCESS;
	}

	oga_error_t oga_array_pop(oga_array_t * array) {
		if (!array) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_array(array->value)) {
			return OGA_INVALID_TYPE;
		}
		array->value.get_array().pop_back();
		return OGA_SUCCESS;
	}

	oga_error_t oga_array_get_size(
		size_t * result,
		oga_array_t * array) {
		if (!array) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_array(array->value)) {
			return OGA_INVALID_TYPE;
		}
		*result = array->value.get_array().size();
		return OGA_SUCCESS;
	}

	oga_error_t oga_array_get(
		oga_value_t ** value,
		oga_array_t * array,
		size_t index) {
		size_t size = 0;
		oga_error_t err = OGA_SUCCESS;
		if ((err = oga_array_get_size(&size, array)) != OGA_SUCCESS) {
			return err;
		}
		if (size <= index) {
			return OGA_OUT_OF_BOUNDS;
		}
		oga_value_t * val = create_value(array->plugin);
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		val->value = array->value.get_array()[index];
		return OGA_SUCCESS;
	}

	oga_error_t oga_object_create(
		oga_object_t ** result,
		oga_plugin_t * plugin) {
		oga_object_t * val = static_cast<oga_object_t*>(create_value(plugin));
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		*result = val;
		val->value = oga::proto::json::object();
		return OGA_SUCCESS;
	}

	oga_error_t oga_object_set(
		oga_object_t* object,
		char const * name,
		oga_value_t * value) {
		if (!object || !name || !value) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_object(object->value)) {
			return OGA_INVALID_TYPE;
		}
		object->value.get_object()[name] = value->value;
		return OGA_SUCCESS;
	}

	oga_error_t oga_object_has(
		oga_object_t* object,
		char const * name) {
		if (!object || !name) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_object(object->value)) {
			return OGA_INVALID_TYPE;
		}
		if (object->value.get_object().count(name) == 0) {
			return OGA_NO_SUCH_ITEM;
		}
		return OGA_SUCCESS;
	}

	oga_error_t oga_object_get_size(
		size_t * result,
		oga_object_t * object) {
		if (!object || !result) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_object(object->value)) {
			return OGA_INVALID_TYPE;
		}
		*result = object->value.get_object().size();
		return OGA_SUCCESS;
	}
	
	oga_error_t oga_object_get(
		oga_value_t ** result,
		oga_object_t * object,
		char const * name) {
		oga_error_t err = oga_object_has(object, name);
		if (err != OGA_SUCCESS) {
			return err;
		}
		oga_value_t * val = create_value(object->plugin);
		if (!val) {
			return OGA_OUT_OF_MEMORY;
		}
		val->value = object->value.get_object()[name];
		return OGA_SUCCESS;
	}
	
	oga_error_t oga_object_erase(
		oga_object_t * object,
		char const * name) {
		if (!object || !name) {
			return OGA_INVALID_PARAMETER;
		}
		if (!is_object(object->value)) {
			return OGA_INVALID_TYPE;
		}
		if (object->value.get_object().count(name) > 0) {
			object->value.get_object().erase(name);
		}
		return OGA_SUCCESS;
	}

	static oga_value_api OGA_VALUE_API = {
		&oga_value_create_from_string,
		&oga_value_create_from_double,
		&oga_value_create_from_int,

		&oga_value_create,
		&oga_value_free,
		&oga_value_get_type,

		&oga_value_set_null,
		&oga_value_set_string,
		&oga_value_set_double,
		&oga_value_set_int,
		&oga_value_set_boolean,
		&oga_value_set_array,
		&oga_value_set_object,

		&oga_value_get_boolean,
		&oga_value_get_int,
		&oga_value_get_double,
		&oga_value_get_string,
		&oga_value_get_array,
		&oga_value_get_object,

		&oga_array_create,
		&oga_array_append,
		&oga_array_pop,
		&oga_array_get_size,
		&oga_array_get,

		&oga_object_create,
		&oga_object_set,
		&oga_object_has,
		&oga_object_get_size,
		&oga_object_get,
		&oga_object_erase
	};


	struct oga_handle_t {
		oga_plugin_t * plugin;
		oga::util::shared_ptr<oga_timer_data> timer;
		oga::util::shared_ptr<oga_command_data> command;
	};

	class oga_handler_wrapper : public oga_handle_t, public oga::handler {
	public:
		oga_handler_wrapper(oga_plugin_t * p, oga_timer_data * t)
		: oga_handle_t()
		, oga::handler()
		{
			plugin = p;
			timer.reset(t, t->free);
		}

		oga_handler_wrapper(oga_plugin_t * p, oga_command_data * c)
		: oga_handle_t()
		, oga::handler()
		{
			plugin = p;
			command.reset(c, c->free);
		}

		oga::error_type call(oga::proto::json::object const & args) {
			if (command) {
				oga_object_t * o = 0;
				oga_object_create(&o, plugin);
				command->callback(command.ptr(), o);
			}
			else if (timer) {
				timer->callback(timer.ptr());
			}
			return oga::success();
		}
	};
	

	oga_error_t register_timer(
		oga_handle_t ** handle,
		oga_plugin_t * plugin,
		oga_timer_data * data) {
		if (!handle || !plugin || !data) {
			return OGA_INVALID_PARAMETER;
		}
		oga::util::shared_ptr<oga_handler_wrapper> handler_ptr(
			new (std::nothrow) oga_handler_wrapper(plugin, data));
		if (handler_ptr) {
			*handle = handler_ptr.ptr();
			plugin->handles[handler_ptr.ptr()] = handler_ptr;
			plugin->agent->loop().add_timer(data->interval, handler_ptr);
		}

		return OGA_NOT_IMPLEMENTED;
	}

	oga_error_t register_command(
		oga_handle_t ** handle,
		oga_plugin_t * plugin,
		oga_command_data* data) {
		if (!handle || !plugin || !data) {
			return OGA_INVALID_PARAMETER;
		}
		oga::util::shared_ptr<oga_handler_wrapper> handler_ptr(
			new (std::nothrow) oga_handler_wrapper(plugin, data));
		if (handler_ptr) {
			*handle = handler_ptr.ptr();
			plugin->handles[handler_ptr.ptr()] = handler_ptr;
			plugin->agent->loop().add_command(data->name, handler_ptr);
		}


		return OGA_NOT_IMPLEMENTED;
	}

	oga_error_t unregister_handler(oga_handle_t* handle) {
		if (!handle) {
			return OGA_INVALID_PARAMETER;
		}

		if (handle->command) {
			handle->plugin->agent->loop().del_command(handle->command->name);
		}
		else if (handle->timer) {
			handle->plugin->agent->loop().del_timer(
				static_cast<oga_handler_wrapper*>(handle));
		}
		return OGA_NOT_IMPLEMENTED;
	}

	oga_error_t send_message(oga_plugin_t*, char const * name, oga_object_t * args) {
		return OGA_NOT_IMPLEMENTED;
	}
	oga_error_t log_message(oga_plugin_t* plugin, char const * message) {
		OGA_LOG_INFO(oga::log::get("plugin"), message);
		return OGA_SUCCESS;
	}

	oga_error_t get_api_version(oga_int32_t * result, oga_plugin_t*) {
		return OGA_NOT_IMPLEMENTED;
	}
	oga_error_t get_config(oga_object_t ** result, oga_plugin_t*) {
		return OGA_NOT_IMPLEMENTED;
	}

}