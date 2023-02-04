// http_client.h

#pragma once

#include <memory>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <optional>
#include <condition_variable>
#include <tcp/fwd.h>
#include <common/common.h>
#include <utils/ordered_map.h>
#include <utils/filesystem.h>

namespace anp
{
	class http_client
	{
	public:
		struct headers_t;
		using data_t = char;
		using http_response_cb = std::function<bool(
			const headers_t& m_headers
			, const data_t*			// Buffer
			, std::size_t			// Size
			, int status
		)>;

		enum erc : int
		{
			unknown = -1,
			no_error = 0,
			connection_error,
			connection_process_error,
			parse_headers_error,
			count
		};

		enum receive_mode : int
		{
			memory_full_payload,
			memory_tcp_packet,
			file
		};

		struct header {
			std::string name;
			std::string value;
			header(const std::string_view& name, const std::string_view& valule) : name(name), value(value) {}
			std::string to_string() const;
			operator std::string() const;
		};

		struct headers_t
		{
			using data_t = utils::ordered_map<std::string, header>;
			headers_t() = default;
			headers_t(const data_t& m_headers) : data(m_headers) {}
			std::string to_string() const;
			operator std::string() const;
			void add(const header& h);
			const std::string& get(const std::string& name) const;
			const data_t& data() const { return m_data; }
			void clear() { m_data.clear(); }

		private:
			data_t m_data;
		};

		struct endpoint_t
		{
			const std::string& host;
			int port;
		};

		// --- headers_parser ---
		class headers_parser
		{
		public:
			enum erc : int
			{
				initial = -2,
				in_progress = -1,
				done = 0,
				receive_size_error,
				status_parse_error,
				no_content_length,
				parse_size_error,
				count
			};

		public:
			int parse(const std::string_view& s);
			int error_code() const { return m_error_code; }
			int status() const { return m_status; }
			std::size_t content_length() const { return m_content_length.has_value() ? m_content_length.value() : 0; }
			std::size_t cursor() const { return m_cursor; }
			const headers_t& headers() const { return m_headers; }
			inline void reset();

		private:
			int m_error_code = 0;
			headers_t m_headers;
			std::optional<std::size_t> m_content_length;
			int m_status = -1;
			std::string m_last_data;
			std::size_t m_cursor = 0;
		};

		// --- data_receiver ---
		template <typename Data_element_t>
		class data_receiver_base
		{
		public:
			virtual bool receive(const Data_element_t* data, const std::size_t& size) = 0;
			virtual const Data_element_t* data() const = 0;
			virtual size_t size() const = 0;
		};

		// --- data_reciever_memory ---
		template <typename Data_element_t>
		class data_reciever_memory : public data_receiver_base<Data_element_t>
		{
		public:
			using base = data_receiver<Data_element_t>;
			using data_t = std::vector<Data_element_t>;

		public:
			data_reciever_memory(const std::size_t& size) { m_data.reserve(size); }
			bool receive(const Data_element_t* data, const std::size_t& size) override{
				assert(size + m_data.size() <= m_data.capacity());
				m_data.insert(m_data.end(), data, data + size);
				return m_data.size() == m_data.capacity();
			}
			const Data_element_t* data() const override { return m_data.data(); }
			size_t size() const override { return m_data.size(); }
			const data_t& data_container() const { return m_data; }

		private:
			data_t m_data;
		};

		// --- file_receiver ---
		template <typename Data_element_t>
		class file_receiver : public data_receiver_base<Data_element_t>
		{
			using base = data_receiver<Data_element_t>;
			std::string m_file_name;
			std::ofstream m_file;

		public:
			file_receiver(std::size_t size, const std::string& file_name = "") : m_file_name(file_name) {
				m_file.open(m_file_name, std::ios::binary);
				fs::resize_file(m_file, size);
			}
		};

		http_client();
		~http_client();

		int query(
			const endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const headers_t& m_headers = headers_t(),
			const std::string& body = ""
		);

		void query_async(
			const endpoint_t& endpoint,
			const std::string& method,
			const std::string& query,
			const http_response_cb& on_receive = http_response_cb(),
			const headers_t& m_headers = headers_t(),
			const std::string& body = ""
		);

		int request(
			const endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		);

		void request_async(
			const endpoint_t& endpoint,
			const std::string& request,
			const http_response_cb& on_receive
		);

		int errcode() {
			return m_error_code.load();
		}

		void wait();

		int notify(int ec);

	protected:
		std::string parse_header(const std::string& response, const std::string& header);

	protected:
		virtual void on_before_notify(int ec) {};
		virtual void on_notify(int ec) {};
		void reset();
		virtual void on_reset() {};

	private:
		std::atomic<int> m_error_code = erc::unknown;
		std::mutex m_cv_mtx;
		std::unique_lock<std::mutex> m_cv_ul;
		std::condition_variable m_cv;
		std::unique_ptr<tcp::client> m_client;
		headers_parser m_headers_parser;
		receive_mode m_receive_mode = receive_mode::memory_full_payload;
		std::unique_ptr<data_receiver<data_t>> m_data_receiver;
	};
}
