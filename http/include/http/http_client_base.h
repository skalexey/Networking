// http_client_base.h

#pragma once

#include <memory>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <tcp/fwd.h>
#include <anp/common.h>
#include <http/query.h>
#include <http/http_client_interface.h>

namespace anp
{
	class http_client_base : public virtual http_client_interface
	{
		using base = http_client_interface;

	public:
		enum receive_mode : int
		{
			memory_full_payload = int(base::receive_mode::receive_mode_count),
			file,
			receive_mode_count,
		};

		enum erc : int
		{
			unknown = -1,
			no_error = 0,
			connection_error,
			connection_process_error,
			http_error,
			user_error,
			erc_count
		};

		http_client_base();
		~http_client_base();

		int request(
			const tcp::endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		) override;

		void request_async(
			const tcp::endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		) override;

		int errcode() override {
			return m_error_code.load();
		}

		void wait() override;

		int notify(int ec) override;

		void set_receive_file(const fs::path& file_path) override;

		void set_receive_mode(int mode) override {
			m_receive_mode = receive_mode(mode);
		}

		const fs::path& get_file_path() const override {
			return m_file_path;
		}

		bool remove_received_file();

	protected:
		void reset(client_type c = client_type::http) override;

	
	// http_client_base own interface
	protected:
		void set_on_response(const http_response_cb& cb) {
			m_on_response = cb;
		}

		const http_response_cb& get_on_response() const {
			return m_on_response;
		}

		virtual bool on_packet_receive(const std::vector<http_data_t>& data, std::size_t sz, int id) = 0;
		virtual const http::headers_t& get_headers() const = 0;
		virtual int get_status() const = 0;

		receive_mode get_receive_mode() const {
			return m_receive_mode;
		}
	private:
		std::atomic<int> m_error_code = erc::unknown;
		std::mutex m_cv_mtx;
		std::unique_lock<std::mutex> m_cv_ul;
		std::condition_variable m_cv;
		std::unique_ptr<anp::tcp::client_base> m_client;
		receive_mode m_receive_mode = receive_mode::memory_full_payload;
		fs::path m_file_path;
		http_response_cb m_on_response;
	};
}
