// authenticator.cpp

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
#include <utils/file_utils.h>
#include <utils/datetime.h>
#include <tcp/client.h>
#include <http/authenticator.h>

LOG_PREFIX("[authenticator]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);
namespace fs = std::filesystem;
namespace ch = std::chrono;

namespace anp
{
	int authenticator::auth(const tcp::endpoint_t& ep, const std::string& path, const credentials& credentials)
	{
		query_t q = credentials.query();
		q.path = path;
		q.method = "GET";
		return query(ep, q);
	}

	void authenticator::auth_async(const tcp::endpoint_t& ep, const std::string& path, const credentials& credentials, const anp::result_cb& on_result)
	{
		query_t q = credentials.query();
		q.path = path;
		q.method = "GET";
		query_async(ep, q, [self = this, on_result](
				const http::headers_t&
				, const http_data_t* data
				, std::size_t sz
				, int http_status
			)
			{
				if (http_status != 200)
				{
					on_result(self->notify(http_client_base::erc::http_error));
					return false;
				}
				std::string_view s(data, sz);
				if (s.find("Authenticated successfully") == std::string::npos)
				{
					on_result(self->notify(erc::auth_error));
					return false;
				}
				on_result(self->notify(http_client_base::erc::no_error));
				return true;
			}
		);
	}

	query_t credentials::query() const
	{
		query_t q;
		q.add_value("u", user);
		q.add_value("t", token);
		return q;
	}
}