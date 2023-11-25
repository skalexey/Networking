// downloader_interface.h

#pragma once

#include <memory>
#include <utils/filesystem.h>
#include "query.h"
#include <tcp/endpoint.h>
#include "http_client_interface.h"

namespace anp
{
	class downloader_interface : public virtual http_client_interface
	{
	public:
		virtual int download_file(
			const tcp::endpoint_t& ep
			, const query_t& query = {}
			, const fs::path& target_path = {}
			, const http_response_cb& on_response = {}
		) = 0;
		
		virtual void download_file_async(
			const tcp::endpoint_t& ep
			, const result_cb& on_result = {}
			, const query_t& query = {}
			, const fs::path& target_path = {}
			, const http_response_cb& on_response = {}
		) = 0;

		using on_before_download_t = std::function<void(const fs::path&)>;
		void set_on_before_download(const on_before_download_t& on_before_download) {
			m_on_before_download = on_before_download;
		}

	protected:
		virtual void on_before_download(const fs::path& target_path) {
			if (m_on_before_download)
				m_on_before_download(target_path);
		}

	private:
		on_before_download_t m_on_before_download;

	};
	using downloader_interface_ptr = std::shared_ptr<downloader_interface>;
}