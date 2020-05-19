#pragma once
#include <vector>
#include <set>
#include <map>
#include <tuple>

class JobShopNode
{
public:
	static JobShopNode& get_inst()
	{
		static JobShopNode inst;
		return inst;
	}
	static JobShopNode create_initial_node();
public:
	JobShopNode();
	void calculate_LB();
	int get_LB() const { return _LB; }
	void get_E(int UB);
	void get_S(int UB);
	bool is_proposition_3(size_t k);
	bool is_proposition_11(size_t e, size_t s, int UB);
	bool is_feasible(int& C_max) const;
	const std::set<size_t>& C() const { return _C; }
	const std::set<size_t>& E() const { return _E; }
	const std::set<size_t>& S() const { return _S; }
	void erase_E(size_t e);
	void erase_S(size_t s);
	bool is_root() const { return _root;  }
	void set_not_root() { _root = false; }
	void add_input(size_t e);
	void add_output(size_t s);
	void eq4_and_5(size_t e);
	bool eq6_and_7(size_t s);
	void eq8_and_9();
	void eq8_and_9(size_t s);
	void eq10_and_11(size_t e);
	void eq11();
	void update_root_tail(size_t e);
	int get_max_l_p_q() const;
	void print() const;

	bool operator < (const JobShopNode& rhs) const
	{
		return _LB < rhs._LB;
	}
	bool operator > (const JobShopNode& rhs) const
	{
		return _LB > rhs._LB;
	}
private:
	struct Job {
		Job(size_t i, int rr, int pp, int qq);
		size_t idx;
		int r;
		int p; // processing time
		int q;
		int l;
		int u;
	};
	struct ArcFromTo
	{
		ArcFromTo(size_t f, size_t t) : from(f), to(t) {}
		size_t from;
		size_t to;

		bool operator < (const ArcFromTo& rhs) const
		{
			return std::tie(from, to) < std::tie(rhs.from, rhs.to);
		}
	};
	struct Arc {
		Arc(int ll, int uu) : W1(ll), W2(uu) {}
		int W1;
		int W2;
	};

private:
    void _eq8_and_9(size_t j, const std::set<size_t>& I);
	int get_H(const std::set<size_t>& C) const;
	int get_LB(const std::set<size_t>& C) const;
	int get_LB_ATSP(const std::set<size_t>& C) const;
	bool is_feasible(const std::vector<size_t>& seq, int& C_max) const;
	int _proposition_1() const;
	bool is_proposition_4_input(size_t k) const;
	bool is_proposition_4_output(size_t ryy) const;
	bool is_proposition_5_a(size_t k, int UB) const;
	bool is_proposition_5_b(size_t idx_r, int UB) const;
	bool is_proposition_7(size_t k, int UB) const;
	void proposition_11(int UB);

	bool has_arc(size_t i, size_t j) const;
	const Arc& get_arc(size_t i, size_t j) const;
private:

private:
	static void _initialize_C(JobShopNode& node);

	int _LB;
	bool _root;
	std::vector<size_t> _IS;
	std::vector<size_t> _OS;
	std::set<size_t> _C;
	std::set<size_t> _E;
	std::set<size_t> _S;
	std::vector<Job> _jobs;
	std::map<ArcFromTo, Arc> _arcs;
};

