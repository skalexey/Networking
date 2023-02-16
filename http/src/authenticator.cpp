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
#include "authenticator.h"

LOG_PREFIX("[authenticator]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);
namespace fs = std::filesystem;
namespace ch = std::chrono;

namespace anp
{
	int authenticator::auth(const endpoint_t& ep, const std::string& path, const credentials& credentials)
	{
		query_t q = credentials.query();
		q.path = path;
		q.method = "GET";
		return query(ep, q);
	}

	void authenticator::on_notify(int ec)
	{
	}

	void authenticator::on_reset()
	{
	}

	query_t credentials::query() const
	{
		query_t q;
		q.values.add("u", user);
		q.values.add("t", token);
		return q;
	}
}