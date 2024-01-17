// uploader_decorator.h

#pragma once

#include <memory>
#include <utils/filesystem.h>
#include "http_client_decorator.h"
#include "uploader_interface.h"

namespace anp
{
	namespace http
	{
		class uploader_decorator : public http_client_decorator, public uploader_interface
		{
		public:
			uploader_decorator(uploader_interface* object) 
				: http_client_decorator(object)
			{
				try {
					m_object = std::dynamic_pointer_cast<uploader_interface>(object->shared_from_this());
				}
				catch (std::bad_weak_ptr const&) {
					m_object = uploader_interface_ptr(object);
				}
			}

			void upload_file_async(
				const tcp::endpoint_t& ep,
				const fs::path& target_path,
				const query_t& query,
				const utils::void_int_cb& cb = nullptr
			) override
			{
				m_object->upload_file_async(ep, target_path, query, cb);
			}

		private:
			uploader_interface_ptr m_object = nullptr;
		};
	}
}