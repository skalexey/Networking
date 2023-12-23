// uploader.cpp

#ifdef __cpp_lib_format
#include <format>
#endif
#include <fstream>
#include <utils/Log.h>
#include <utils/string_utils.h>
#include <utils/file_utils.h>
#include <utils/datetime.h>
#include <http/uploader.h>

LOG_PREFIX("[uploader]: ");
LOG_POSTFIX("\n");
SET_LOG_DEBUG(true);

namespace anp
{
	namespace http
	{
		void uploader::upload_file_async(
			const tcp::endpoint_t& ep,
			const fs::path& local_path,
			const query_t& query,
			const utils::void_int_cb& cb
		)
		{
			LOG_DEBUG("Uploading file: " << local_path.string() << " to " << query.uri());

			std::string fname = local_path.filename().string();
			query_t q = query;
			std::ifstream f(local_path);
			if (!f.is_open())
			{
				if (cb)
					cb(errcode());
				notify(erc::file_not_exists);
				return;
			}
			std::string file_data = utils::file::contents(local_path);

			q.headers.add({ "Content-Type", "multipart/form-data; boundary=dsfjiofadsio"});

			q.body = utils::format_str(
				"--dsfjiofadsio\r\n"
				"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
				"Content-Type: text/plain\r\n\r\n"
				"%s\r\n"
				"--dsfjiofadsio--"
				, fname.c_str(), file_data.c_str()
			);
			q.headers.add({ "Content-Length", std::to_string(q.body.size()) });
			q.method = "POST";
			this->query_async(ep, q, [self = this, cb](
				const http::headers_t& headers
				, const char* data
				, std::size_t sz
				, int status
				) -> bool
			{
				std::string s(data, data + sz);
				int ec = 0;
				if (s.find("uploaded successfully") != std::string::npos)
				{
					LOG_DEBUG("Upload has been completed");
					ec = http_client_base::erc::no_error;
				}
				else if (s.find("Auth error") != std::string::npos)
				{
					ec = erc::auth_error;
				}
				else
				{
					LOG_DEBUG("Upload failed");
					ec = erc::transfer_error;
				}
				if (cb)
					cb(self->errcode());
				self->notify(ec);
				return self->errcode() == http_client_base::erc::no_error;
			});
		}

		void uploader::on_notify(int ec)
		{
		}

		void uploader::on_reset()
		{
		}
	}
}