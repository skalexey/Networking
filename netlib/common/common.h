#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <system_error>
#include <asio.hpp>

namespace anp
{
	typedef std::function<void(
		const std::vector<char>&	// Buffer
		, std::size_t				// Size
		, int						// Id
	)> data_cb;

	typedef std::function<void()> void_cb;
	typedef std::function<void(const std::error_code&)> error_cb;
	typedef std::function<void(asio::error_code&) > asio_operation_cb;
}