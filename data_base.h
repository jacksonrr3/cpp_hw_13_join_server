#pragma once
#include <string>
#include <sstream>
#include <map>

namespace data_base {

	class Database {
		std::map<int, std::string> A;
		std::map<int, std::string> B;

		void insert(const std::string& table, int id, std::string&& value) {}
		void trincate(const std::string& table) {

		}
		void intersection() {}
		void symmetric_difference() {}

	public:

		Database() {}
		~Database() {}

		std::string query(std::string&& q) {
			return q;
		}

	};

}

