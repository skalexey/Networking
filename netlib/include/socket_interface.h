#pragma once

#include <functional>
#include <system_error>
#include <cstddef>

namespace anp
{
	template <typename Buffer_t>
	class socket_interface
	{
	public:
		using buffer_t = Buffer_t;
		using buffer_value_type = buffer_t::template value_type;
		using response_cb_t = std::function<void(std::error_code, std::size_t)>;
		virtual void async_read_some(buffer_t& buf, const response_cb_t& cb) = 0;
		virtual void async_write(const buffer_value_type* begin, std::size_t size, const response_cb_t& cb) = 0;
		virtual bool is_open() const = 0;
		virtual void close() = 0;
	};
}
