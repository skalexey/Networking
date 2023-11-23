// uploader_interface.h

#pragma once

#include <utils/filesystem.h>
#include "query.h"
#include <tcp/endpoint.h>
#include "http_client_interface.h"

namespace anp
{
	class uploader_interface : public virtual http_client_interface
	{
	public:
		virtual int upload_file(
			const tcp::endpoint_t& ep,
			const fs::path& target_path,
			const query_t& query
		) = 0;
	};
}