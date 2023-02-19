// downloader_interface.h

#pragma once

#include <utils/filesystem.h>
#include "query.h"
#include "endpoint.h"
#include "http_client_interface.h"

namespace anp
{
	class downloader_interface : public virtual http_client_interface
	{
	public:
		virtual int download_file(
			const endpoint_t& ep
			, const query_t& query = {}
			, const fs::path& target_path = {}
			, const http_response_cb& on_response = {}
		) = 0;
	};
}