// uploader.h

#pragma once

#include <http/http_client.h>
#include <utils/filesystem.h>
#include "uploader_interface.h"

namespace anp
{
	namespace http
	{
		class uploader : public http_client, public uploader_interface
		{
			using base = http_client;

		public:
			void upload_file_async(
				const tcp::endpoint_t& ep,
				const fs::path& target_path,
				const query_t& query,
				const utils::void_int_cb& cb = nullptr
			) override;

		protected:
			void on_notify(int ec) override;
			void on_reset() override;
		};
	}
}