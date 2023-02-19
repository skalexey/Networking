// downloader.cpp

#include <exception>
#include <string>
#include <algorithm>
#include <type_traits>
#ifdef __cpp_lib_format
#include <format>
#endif
#include <fstream>
#include <tcp/client.h>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <tcp/client.h>
#include "downloader.h"

LOG_PREFIX("[downloader]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);
namespace ch = std::chrono;

namespace anp
{
	int downloader::download_file(
		const endpoint_t& ep
		, const query_t& q
		, const fs::path& target_path
		, const http_response_cb& on_response
	)
	{
		set_receive_file(target_path.string() + ".dwl");
		query_t query = q;
		if (query.method.empty())
			query.method = "GET";
		return this->query(ep, query, [=, this](
			const headers_t& headers
			, const char* data
			, std::size_t sz
			, int status
			) -> bool
			{
				if (on_response)
					if (!on_response(headers, data, sz, status))
						return false;
				return true;
			}
		);
	}
}