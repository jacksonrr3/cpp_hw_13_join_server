#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <map>

namespace data_base {

	class Database {
		std::map<int, std::string> A;
		std::map<int, std::string> B;
		std::string res_str;

		void insert(std::stringstream& ss) { 
			std::string table;
			ss >> table;
			int id;
			ss >> id;
			std::string name;
			ss >> name;
			res_str = "OK\n";
			if ((table != "A" && table != "B") || ss.fail()) {
				res_str = "Error_args_INSERT\n";
			}
			if (table == "A") {
				if (!A.emplace(id, name).second) {
					res_str = "Error_id_duplicate\n";
				}
			}
			if (table == "B"){
				if (!B.emplace(id, name).second) {
					res_str = "Error_id_duplicate\n";
				}
			}
		}

		void truncate(std::stringstream& ss) {
			std::string table;
			ss >> table;
			if (table == "A") {
				A.clear();
				res_str = "OK\n";
			}
			else if (table == "B"){
				B.clear();
				res_str = "OK\n";
			}
			else { res_str = "Error_table_name_TRUNCATE\n"; }
		}

		void print(std::stringstream& ss) {
			res_str.clear();
			std::string table;
			ss >> table;
			if (table == "A") {
				std::for_each(A.begin(), A.end(), [this](auto it){
					res_str += std::to_string(it.first) + it.second + "\n";
					});
				res_str += "OK\n";
			}
			else if (table == "B") {
				std::for_each(B.begin(), B.end(), [this](auto it) {
					res_str += std::to_string(it.first) + it.second + "\n";
					});
				res_str += "OK\n";
			}
		}

		void intersection() { 
			res_str.clear();
			auto it_a = A.begin();
			auto it_b = B.begin();
			int a_id;
			int b_id;
			while (it_a != A.end() && it_b != B.end()) {
				int a_id = it_a->first;
				int b_id = it_b->first;
				if (a_id > b_id) { 
					++it_b; 
				}
				else if (a_id < b_id) { 
					++it_a; 
				}
				else { 
					res_str += std::to_string(a_id) + "," +
						it_a->second + "," +
						it_b->second + "\n";
					++it_a;
					++it_b;
				}
			}
			res_str += "OK\n";
		}

		void symmetric_difference() { 
			res_str.clear(); 
			auto it_a = A.begin();
			auto it_b = B.begin();
			int a_id = it_a->first;
			int b_id = it_b->first;
			while (it_a != A.end() && it_b != B.end()) {
				a_id = it_a->first;
				b_id = it_b->first;
				if (b_id < a_id) {
					res_str += std::to_string(b_id) + "," +
						"," +
						it_b->second + "\n";
					++it_b;
				}
				else if (a_id < b_id) {
					res_str += std::to_string(a_id) + "," +
						it_a->second + "," +
						"\n";
					++it_a;
				}
				else {
					++it_a;
					++it_b;
				}
			}

			if (it_a != A.end()) {
				while (it_a != A.end()) {
					a_id = it_a->first;
					res_str += std::to_string(a_id) + "," +
						it_a->second + "," +
						"\n";
					++it_a;
				}
			}
			else if (it_b != B.end()) {
				while (it_b != B.end()) {
					b_id = it_b->first;
					res_str += std::to_string(b_id) + "," +
						"," +
						it_b->second + "\n";
					++it_b;
				}
			}
			else {}
			res_str += "OK\n";
		}

	public:

		Database() {}
		~Database() {}

		std::string query(std::string&& q) {
			std::stringstream ss(q);
			std::string command;
			ss >> command;
			if (command == "INSERT") { insert(ss); }
			else if (command == "TRUNCATE") { truncate(ss); }
			else if (command == "PRINT") { print(ss); } //дополнительная команда для отладки
			else if (command == "INTERSECTION") { intersection(); }
			else if (command == "SYMMETRIC_DIFFERENCE") { symmetric_difference(); }
			else { res_str = "ERR_COMMAND!TRY_AGAIN!\n"; }
			return std::move(res_str);
		}

	};

}
