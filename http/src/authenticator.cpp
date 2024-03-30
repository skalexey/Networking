// authenticator.cpp

#include <string>
#include <algorithm>
#include <type_traits>
#ifdef __cpp_lib_format
#include <format>
#endif
#include <fstream>
#include <tcp/client.h>
#include <utils/log.h>
#include <utils/string_utils.h>
#include <utils/file_utils.h>
#include <utils/datetime.h>
#include <tcp/client.h>
#include <http/authenticator.h>

LOG_TITLE("authenticator");
SET_LOCAL_LOG_LEVEL(debug);
namespace fs = std::filesystem;
namespace ch = std::chrono;

namespace anp
{
	int authenticator::auth(const tcp::endpoint_t& ep, const std::string& path, const credentials& credentials)
	{
		auth_async(ep, path, credentials, nullptr);
		wait();
		return errcode();
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
				auto finalize = [self, on_result](erc e) -> bool
				{
					self->notify(e);
					if (on_result)
						on_result(e);
					return e == erc::no_error;
				};
				if (http_status != 200)
					return finalize(erc::http_error);
				std::string_view s(data, sz);
				if (s.find("Authenticated successfully") == std::string::npos)
					return finalize(erc::auth_error);
				return finalize(erc::no_error);
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