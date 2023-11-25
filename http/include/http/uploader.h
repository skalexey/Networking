// uploader.h

#pragma once

#include <http/http_client.h>
#include <utils/filesystem.h>
#include "uploader_interface.h"

namespace anp
{
	class uploader : public http_client, public uploader_interface
	{
		using base = http_client;

	public:
		enum erc : int
		{
			file_not_exists = base::erc::count,
			transfer_error,
			auth_error
		};

		int upload_file(
			const tcp::endpoint_t& ep,
			const fs::path& target_path,
			const query_t& query
		) override;

	protected:
		void on_notify(int ec) override;
		void on_reset() override;
	};
}