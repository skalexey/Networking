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
#include <http/downloader.h>

LOG_PREFIX("[downloader]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);
namespace ch = std::chrono;

namespace anp
{
	int downloader::download_file(
		const tcp::endpoint_t& ep
		, const query_t& q
		, const fs::path& target_path
		, const http_response_cb& on_response
	)
	{
		before_download(target_path);

		query_t query = q;
		if (query.method.empty())
			query.method = "GET";
		return this->query(ep, query, [this, on_response](
			const http::headers_t& headers
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

	void downloader::download_file_async(
		const tcp::endpoint_t& ep
		, const result_cb& on_result
		, const query_t& q
		, const fs::path& target_path
		, const http_response_cb& on_response
	)
	{
		before_download(target_path);

		query_t query = q;
		if (query.method.empty())
			query.method = "GET";
		query_async(ep, query, [this, on_result, on_response](
			const http::headers_t& headers
			, const char* data
			, std::size_t sz
			, int status
			) -> bool
			{
				bool result = true;
				if (on_response)
					if (!on_response(headers, data, sz, status))
						result = false;
				if (on_result)
					on_result(errcode());
				return result;
			}
		);
	}

	void downloader::before_download(const fs::path& target_path)
	{
		set_receive_file(target_path.string() + ".dwl");
		on_before_download(target_path);
	}
}