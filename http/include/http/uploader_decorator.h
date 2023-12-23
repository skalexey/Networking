// uploader_decorator.h

#pragma once

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
				, m_object(object)
			{}
			int upload_file(
				const tcp::endpoint_t& ep,
				const fs::path& target_path,
				const query_t& query
			) override
			{
				return m_object->upload_file(ep, target_path, query);
			}

		private:
			uploader_interface* m_object = nullptr;
		};
	}
}