// authenticator.h

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <filesystem>
#include "http_client.h"

namespace anp
{
	class authenticator : public http_client
	{
		using base = http_client;

	public:
		enum erc : int
		{
			auth_error = http_client::erc::count,
		};

		int auth(
			const std::string& host,
			int port,
			const std::string& user,
			const std::string& token
		);

	protected:
		void on_notify(int ec) override;
		void on_reset() override;

	private:
	};
}