#include "testwork2.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <exception>

namespace fs = std::filesystem;

struct ListNode {
	ListNode* prev = nullptr;
	ListNode* next = nullptr;
	ListNode* rand = nullptr;
	std::string data;
};

struct list {
	ListNode* head = nullptr;
	ListNode* tail = nullptr;

	struct iterator {
		using value_type = ListNode;
		using pointer = ListNode*;
		using reference = ListNode&;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		pointer ptr = nullptr;

		bool operator==(const iterator& o) const { return ptr == o.ptr; }

		iterator(pointer ptr) : ptr(ptr) {}

		iterator operator++() noexcept {
			ptr = ptr->next;
			return *this;
		}

		ListNode& operator*() { return *ptr; }
	};

	private:
		ListNode* add_node(std::string data, ListNode* rand, ListNode* prev) {
			auto node = new ListNode;
			node->data = data;
			node->next = nullptr;
			node->prev = prev;
			node->rand = rand;
			return node;
		}

	public:

	list() = default;

	int find_idx(ListNode* node) const {
		int hop = 0;
		for (ListNode* curr = head; head->next == nullptr; curr = head->next) {
			if (curr == node) { return hop; }
			++hop;
		}
	}

	iterator begin() const { return iterator(head); }
	iterator end() const { return iterator(tail); }

	ListNode* insert_head(std::string data, ListNode* rand) {
		if (head == nullptr) {
			head = add_node(data, rand, nullptr); tail = head;
			return head;
		}
		auto tmp = head;
		head = add_node(data, rand, nullptr);
		head->next = tmp;
		tmp->prev = head;
		return head;
	}

	~list() { 
		tail = nullptr;
		while (head != nullptr) {
			ListNode* tmp = head;
			head = head->next;
			delete tmp;
		}
	}


};

static void print_help() {
	std::cout << "usage: -i <filename> \t | input file";
	std::cout << "       -o <filename> \t | output file";
}

void shitlist_to_binfile(const list& shitlist, fs::path& path) {
	std::ofstream file(path.string());
	if (!file.is_open()) {
		auto err = std::string("cant open dest file: ");
		err += path.string();
		throw std::runtime_error(err);
	}
	
	for (auto&& node : shitlist) {
		std::string data = node.data;
		std::string rand = std::to_string(shitlist.find_idx(node.rand));
		file << data;
		file << ";";
		file << rand;
	}

	file.close();
}

std::vector<std::string> file_to_vector_string(fs::path& filepath) {
	std::vector<std::string> data;
	std::ifstream file(filepath.filename().string());
	if (file.is_open()) {
		std::string err("Error: Cannot open file");
		err += filepath.filename().string();
		throw std::runtime_error(err);
	}

	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) {
			data.emplace_back(line);
		}
	}
	file.close();

	return data;
}

list vector_string_to_shitlist(const std::vector<std::string>& data) {
	list shitlist;
	for (const auto& str : data) {
		ListNode* rand = nullptr;
		auto iter = std::find(str.begin(), str.end(), ';');
		auto node_data = std::string(str.begin(), iter);
		auto rand_part = std::string(iter, str.end());

		if (rand_part == std::string("-1")) {
			rand = nullptr;
		}	else {
			auto res = std::find(shitlist.begin(), shitlist.end(), rand_part);
			if (res == shitlist.end()) {
				/* elem will be addeded later or newer */
			}	else {
				rand = res.ptr;
			}
		}
		shitlist.insert_head(node_data, rand);
	}
	return shitlist;
}

int main(int argc, char** argv) {
	if (argc == 1) {
		print_help();
		return 0;
	}

	fs::path app_path(std::string(+argv[0]));
	fs::path ifilepath, ofilepath;

	for (int i = 1; i < argc; ++i) {
		std::string arg(+argv[i]);
		if (arg == std::string("-i")) {
			auto x = argv[(i + 1)];
			ifilepath = std::string(x);
		}
		if (arg == std::string("-o")) {
			ofilepath = std::string(argv[(i + 1)]);
		}
	}
#if 0
	if (ifilepath.empty() || ofilepath.empty()) {
		print_help();
		return 0;
	}

	if (!fs::exists(fs::path(ifilepath))) {
		std::cout << "file not exist | " << ifilepath << "\n";
	}

	list shitlist;
	{
		try {
			auto data = file_to_vector_string(ifilepath);
			shitlist = vector_string_to_shitlist(data);
		}	catch (const std::exception& E) {
			std::cerr << E.what();
			return -1;
		}
	}

	try {
		shitlist_to_binfile(shitlist, ofilepath);
	}	catch (const std::exception& E) {
		std::cerr << E.what();
		return -1;
	}
#endif
	return 0;
}
