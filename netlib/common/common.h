#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <system_error>
#include <asio.hpp>

namespace anp
{
	using data_cb = std::function<bool(
		const std::vector<char>&	// Buffer
		, std::size_t				// Size
		, int						// Id
	)>;

	using void_cb = std::function<void()>;
	using void_int_arg_cb = std::function<void(int)>;
	using error_cb = std::function<void(const std::error_code&)>;
	using asio_operation_cb = std::function<void(asio::error_code&) >;
}