// url_uploader.h

#pragma once

#include <string>
#include <http/http_client.h>
#include <utils/filesystem.h>
#include <http/uploader_interface.h>

namespace anp
{
	namespace http
	{
		class url_uploader : public http_client, public uploader_interface
		{
			using base = uploader_interface;

		public:
			void upload_file_async(
				const tcp::endpoint_t& ep,
				const fs::path& target_path,
				const query_t& query,
				const utils::void_int_cb& cb = nullptr
			) override;
		};
	}
}