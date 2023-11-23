// downloader.h

#pragma once

#include <memory>
#include <utils/filesystem.h>
#include "http_client.h"
#include "downloader_interface.h"

namespace anp
{
	class downloader : public http_client, public virtual downloader_interface
	{
		using base = http_client;

	public:
		enum erc : int
		{
			parse_date_error = http_client::erc::count,
			no_date,
			file_error,
			store_download_error,
			count
		};

		int download_file(
			const tcp::endpoint_t& ep
			, const query_t& query = {}
			, const fs::path& target_path = {}
			, const http_response_cb& on_response = {}
		) override;
		
		void download_file_async(
			const tcp::endpoint_t& ep
			, const result_cb& on_result = {}
			, const query_t& query = {}
			, const fs::path& target_path = {}
			, const http_response_cb& on_response = {}
		) override;

	private:
		void before_download(const fs::path& target_path);

	};
	using downloader_ptr = std::shared_ptr<downloader>;
}