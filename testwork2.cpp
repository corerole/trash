#include <iostream>
#include <array>
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

	struct rev_iter {
		using value_type = ListNode;
		using pointer = ListNode*;
		using reference = ListNode&;
		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		pointer ptr = nullptr;

		bool operator==(const rev_iter& o) const { return ptr == o.ptr; }

		rev_iter(pointer ptr) : ptr(ptr) {}

		rev_iter operator++() noexcept {
			ptr = ptr->prev;
			return *this;
		}
		ListNode& operator*() { return *ptr; }
	};

	private:
		ListNode* add_node(const std::string& data, ListNode* rand, ListNode* next, ListNode* prev) {
			auto node = new ListNode;
			node->data = data;
			node->next = next;
			node->prev = prev;
			node->rand = rand;
			return node;
		}

	public:

	list() = default;

	int find_hop_by_node_from_head(ListNode* node) const {
		int hop = 0;
		for (ListNode* curr = head; head->next == nullptr; curr = head->next) {
			if (curr == node) { return hop; }
			++hop;
		}
		return hop;
	}

	ListNode* find_node_by_hop_from_tail(int hop) {
		if (hop < 0) { return nullptr; }
		auto tmp = tail;
		while ((hop > 0) || tmp != nullptr) {
			tmp = tmp->prev;
			--hop;
		}
		return tmp;
	}

	iterator begin() const { return iterator(head); }
	iterator end() const { return iterator(nullptr); }
	rev_iter rbegin() const { return rev_iter(tail); }
	rev_iter rend() const { return rev_iter(nullptr); }


	ListNode* insert_front(std::string data, ListNode* rand) {
		if (head == nullptr) {
			head = add_node(data, rand, nullptr, nullptr); tail = head;
			return head;
		}

		auto tmp = head;
		head = add_node(data, rand, nullptr, nullptr);
		head->next = tmp;
		tmp->prev = head;
		return head;
	}

	ListNode* insert_back(std::string data, ListNode* rand) {
		if (tail == nullptr) {
			head = add_node(data, rand, nullptr, nullptr); tail = head;
			return head;
		}
		auto tmp = tail;
		tail = add_node(data, rand, nullptr, tmp);
		tmp->next = tail;
		return tail;
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

static std::vector<std::string> file_to_vector_string(fs::path& filepath) {
	std::vector<std::string> data;
	std::ifstream file(filepath.string());
	if (!file.is_open()) {
		std::string err("Error: Cannot open file: ");
		err += filepath.string();
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

using PairOfStrings = std::optional<std::pair<std::string, std::string>>;

static PairOfStrings string_to_pair(const std::string& line) {
	/*
	* wrong cases:
	* ;
	* ;...
	* ...;
	* ...;...;...
	*/
	auto divpoint = std::find(line.begin(), line.end(), ';');

	if (
				 line.size() < 3
			|| divpoint == line.end()
			|| divpoint == (line.end() - 1)
			|| (std::find(divpoint, line.end(), ';') != line.end()) 
		)
	{
		return std::nullopt;
	}

	return PairOfStrings({
						std::string(line.begin(), (divpoint - 1)),
						std::string(divpoint + 1, line.end())
	});
}

static std::optional<int> get_number(const std::string& str) {
	size_t n = 0;
	int num = std::numeric_limits<int>::min();
	try {
		num = std::stoi(str, &n, 10);
	} catch (const std::exception& E) {
		std::cout << E.what() << std::endl;
		return std::nullopt;
	}
	return (n == str.size() || (num < -1)) ? std::optional<int>(num) : std::nullopt;
}

static list vector_string_to_shitlist(const std::vector<std::string>& data) {
	list shitlist;
	size_t ctr = 1;
	for (const auto& str : data) {
		auto x = string_to_pair(str);
		if (!x.has_value()) {
			std::cout << "wrong data before \";\" " << " | line N" << ctr << " | this data will not be addeded!\n";
			++ctr;
			continue;
		}

		auto index = get_number(x.value().second);
		if (!index.has_value()) {
			std::cout << "wrong rand value at line N" << ctr << " | this data will not be addeded!\n";
			++ctr;
			continue;
		}

		auto val = index.value();
		ListNode* node = nullptr;
		if (val != -1) { auto node = shitlist.find_node_by_hop_from_tail(val); }
		
		shitlist.insert_front(x.value().first, node);
		++ctr;
	}
	return shitlist;
}


static void shitlist_to_binfile(const list& shitlist, fs::path& path) {
	std::ofstream file(path.string());
	if (!file.is_open()) {
		auto err = std::string("cant open dest file: ");
		err += path.string();
		throw std::runtime_error(err);
	}
	
	auto b = shitlist.rbegin();
	auto e = shitlist.rend();
	for (;e != b; ++b) {
		auto& node = *b;
		std::string data = node.data;
		std::string rand = std::to_string(shitlist.find_hop_by_node_from_head(node.rand));
		auto res = data + std::string(";") + rand + std::string("\n");
		for (const auto& c : res) {
			file.write(&c, 1);
		}
	}
	file.close();
}

int main(int argc, char** argv) {
	if (argc == 1) {
		print_help();
		return 0;
	}

	fs::path ifilepath;
	fs::path ofilepath = "./outlet.out";

	for (int i = 1; i < argc; ++i) {
		std::string arg(argv[i]);
		if (arg == std::string("-i")) {
			auto x = argv[(i + 1)];
			ifilepath = std::string(x);
		}
		if (arg == std::string("-o")) {
			ofilepath = std::string(argv[(i + 1)]);
		}
	}

	if (ifilepath.empty() || ofilepath.empty()) {
		print_help();
		return 0;
	}

	if (!fs::exists(ifilepath)) {
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

	return 0;
}
