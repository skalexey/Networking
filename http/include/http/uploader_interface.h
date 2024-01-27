// uploader_interface.h

#pragma once

#include <memory>
#include <utils/filesystem.h>
#include <tcp/endpoint.h>
#include <utils/common.h>
#include <http/query.h>
#include <http/http_client_interface.h>
namespace anp
{
	namespace http
	{
		class uploader_interface : public virtual http_client_interface
		{
		public:
			using base = http_client_interface;

			virtual int upload_file(
				const tcp::endpoint_t& ep,
				const fs::path& target_path,
				const query_t& query
			) {
				upload_file_async(ep, target_path, query);
				wait();
				return errcode();
			}

			virtual void upload_file_async(
				const tcp::endpoint_t& ep,
				const fs::path& target_path,
				const query_t& query,
				const utils::void_int_cb& cb = nullptr
			) = 0;

		protected:
			int parse_upload_result_code(const std::string& s);
		};
		using uploader_interface_ptr = std::shared_ptr<uploader_interface>;
	}
}