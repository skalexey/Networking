// downloader_decorator.h

#pragma once

#include <memory>
#include <utils/filesystem.h>
#include "http_client_decorator.h"
#include "downloader_interface.h"

namespace anp
{
	class downloader_decorator : public http_client_decorator, public virtual downloader_interface
	{
	public:
		downloader_decorator(downloader_interface* object)
			: http_client_decorator(object)
		{
			try {
				m_object = std::dynamic_pointer_cast<downloader_interface>(object->shared_from_this());
			}
			catch (std::bad_weak_ptr const&) {
				m_object = downloader_interface_ptr(object);
			}
		}
		int download_file(
			const tcp::endpoint_t& ep
			, const query_t& query = {}
			, const std::filesystem::path& target_path = {}
			, const http_response_cb& on_response = {}
		) override
		{
			before_download(target_path);
			return m_object->download_file(ep, query, target_path, on_response);
		}

		void download_file_async(
			const tcp::endpoint_t& ep
			, const result_cb& on_result = {}
			, const query_t& query = {}
			, const fs::path& target_path = {}
			, const http_response_cb& on_response = {}
		) override
		{
			before_download(target_path);
			m_object->download_file_async(ep, on_result, query, target_path, on_response);
		}

	private:
		void before_download(const fs::path& target_path) {
			m_object->set_on_before_download([self = this](const fs::path& target_path) {
				self->on_before_download(target_path);
			});
		}

	private:
		downloader_interface_ptr m_object = nullptr;
	};
}