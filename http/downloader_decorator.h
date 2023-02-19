// downloader_decorator.h

#pragma once

#include <utils/filesystem.h>
#include "http_client_decorator.h"
#include "downloader_interface.h"

namespace anp
{
	class downloader_decorator : public http_client_decorator, public downloader_interface
	{
	public:
		downloader_decorator(downloader_interface* object)
			: http_client_decorator(object)
			, m_object(object)
		{}
		int download_file(
			const endpoint_t& ep
			, const query_t& query = {}
			, const std::filesystem::path& target_path = {}
			, const http_response_cb& on_response = {}
		) override
		{
			return m_object->download_file(ep, query, target_path, on_response);
		}

	private:
		downloader_interface* m_object = nullptr;
	};
}