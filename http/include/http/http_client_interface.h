﻿// http_client_interface.h

#pragma once

// Turning off inheritance view dominance warning
#pragma warning( disable : 4250 )

#include <memory>
#include <functional>
#include <string>
#include <anp/common.h>
#include <utils/filesystem.h>
#include <tcp/endpoint.h>
#include <http/query.h>
#include <http/headers.h>

namespace anp
{
	using http_data_t = char;
	using http_response_cb = std::function<bool(
		const http::headers_t&		// HTTP Headers
		, const http_data_t*	// Buffer
		, std::size_t			// Size
		, int					// HTTP Status
	)>;

	class http_client_interface : public std::enable_shared_from_this<http_client_interface>
	{
	public:
		enum erc : int
		{
			unknown = -1,
			no_error = 0,
			connection_error,
			connection_process_error,
			http_error,
			user_error,
			auth_error,
			parse_date_error,
			no_date,
			file_error,
			store_download_error,
			uncommitted_changes,
			uncommitted_old_changes,
			newer_version_on_server,
			no_file,
			backup_error,
			update_last_version_error,
			parse_headers_error,
			data_receiver_error,
			file_not_exists,
			transfer_error,
			beast_error,
			erc_count
		};
		
		enum receive_mode : int
		{
			memory_tcp_packet,
			receive_mode_count
		};

		virtual ~http_client_interface() = default;
		
		virtual int query(
			const tcp::endpoint_t& endpoint,
			const query_t& query,
			const http_response_cb& on_receive = http_response_cb()
		);

		virtual void query_async(
			const tcp::endpoint_t& endpoint,
			const query_t& query,
			const http_response_cb& on_receive = http_response_cb()
		);

		virtual int request(
			const tcp::endpoint_t& endpoint,
			const anp::sock_data_t& request,
			const http_response_cb& on_receive
		) = 0;

		virtual void request_async(
			const tcp::endpoint_t& endpoint,
			const anp::sock_data_t& request,
			const http_response_cb& on_receive
		) = 0;

		virtual int errcode() = 0;

		// Alternative interface
		virtual int query(
			const tcp::endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const http::headers_t& m_headers = http::headers_t(),
			const anp::sock_data_t& body = {}
		);

		virtual void query_async(
			const tcp::endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const http::headers_t& m_headers = http::headers_t(),
			const anp::sock_data_t& body = {}
		) = 0;
		// End of Alternative interface
		
		virtual void wait() = 0;

		virtual int notify(int ec) = 0;

		virtual void set_receive_file(const fs::path& file_path) = 0;
		virtual void set_receive_mode(int mode) = 0;

		virtual const fs::path& get_file_path() const = 0;

		virtual bool remove_received_file() = 0;

		enum class client_type
		{
			http,
			https
		};
		virtual void reset(client_type c = client_type::http) = 0;
				
	protected:
		virtual void on_before_notify(int ec) {};
		virtual void on_notify(int ec) {};
		virtual void on_reset() {};
	};
	using http_client_interface_ptr = std::shared_ptr<http_client_interface>;
}