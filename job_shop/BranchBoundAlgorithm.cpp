#include "BranchBoundAlgorithm.h"
#include "type.h"
#include <numeric>
#include <algorithm>
#include <stdarg.h>
#include <iterator>

static bool print_step = false;

// debug print
void _printf(const char* format, ...)
{
	if (!print_step)
	{
		return;
	}
	va_list arglist;

	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
}

// algorithm 2.
void BranchBoundAlgorithm::run(JobShopNode root, int& total_node, int& C_max)
{
	// Step 0. Initialization
	JobShopNode best = root;
	int UB = M;
	{
		// get initial solution
		Permutation pp;
		std::vector<size_t> seq;
		UB = pp.get_first_C_max(root, seq);
		best.set_seq(seq);
	}
	root.calculate_LB();

	int count = 0;
	_queue.push(root);
	// Step 3.
	while (!_queue.empty())
	{
		if (count > 0 && count % 10000 == 0)
		{
			printf("  Run %d nodes...\n", count);
		}
		_printf("Algorithm 2. Step 1. Select node count %d\n", ++count);
		// Step 1. Selection of a node with the lowest lower bound
		JobShopNode node = _queue.top();
		_queue.pop();

		_run(node, best, UB);
	}

	//printf("UB = %d, total run node %d\n", UB, count);
	total_node = count;
	printf("total run node %d\n", count);
	printf("removed node   %d(%d)\n", _removed_node, _w_removed_node);
	printf("removed node   %d(%d) (algorithm 2 step 1.2 E or S reduce, proposition 4, 5, 8, 9)\n", _removed_node7, _w_removed_node7);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 E reduce, proposition 4)\n", _removed_node_E_empty_prop_4, _w_removed_node_E_empty_prop_4);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 E reduce, proposition 5)\n", _removed_node_E_empty_prop_5, _w_removed_node_E_empty_prop_5);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 E reduce, proposition 7)\n", _removed_node_E_empty_prop_7, _w_removed_node_E_empty_prop_7);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 E reduce, proposition 9)\n", _removed_node_E_empty_prop_9, _w_removed_node_E_empty_prop_9);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 S reduce, proposition 4)\n", _removed_node_S_empty_prop_4, _w_removed_node_S_empty_prop_4);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 S reduce, proposition 5)\n", _removed_node_S_empty_prop_5, _w_removed_node_S_empty_prop_5);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 S reduce, proposition 7)\n", _removed_node_S_empty_prop_7, _w_removed_node_S_empty_prop_7);
	printf("removed node     %d(%d) (algorithm 2 step 1.2 S reduce, proposition 8)\n", _removed_node_S_empty_prop_8, _w_removed_node_S_empty_prop_8);
	printf("removed node   %d(%d) (algorithm 2 step 1.2 E or S empty, proposition 4, 5, 8, 9)\n", _removed_node1, _w_removed_node1);
	printf("removed node   %d(%d) (algorithm 2 step 1.3 E or S empty, propositoin 13)\n", _removed_node2, _w_removed_node2);
	printf("removed node     %d(%d) (algorithm 2 step 1.3 E or S empty: fix input eq 4, 5, 8, 9, 10, 11 prop 1)\n", _removed_node2_fix_input, _w_removed_node2_fix_input);
	printf("removed node     %d(%d) (algorithm 2 step 1.3 E or S empty: fix output eq 6, 7, 8, 9, 11 prop 1)\n", _removed_node2_fix_output, _w_removed_node2_fix_output);
	printf("removed node   %d(%d) (algorithm 2 step 2 infeasibility (algorithm 1) or UB > C max\n", _removed_node6, _w_removed_node6);
	printf("removed node   %d(%d) (algorithm 3 (branching) eq 6 and 7)\n", _removed_node3, _w_removed_node3);
	printf("removed node   %d(%d) (algorithm 3 (branching) proposition 3)\n", _removed_node4, _w_removed_node4);
	printf("removed node   %d(%d) (algorithm 3 (branching) LB1 or LB2 > UB, proposition 12)\n", _removed_node5, _w_removed_node5);
	
	C_max = M;
	if (!best.is_feasible(C_max))
	{
	//	printf("%s %d, error best is not feasible\n", __func__, __LINE__);
	//	return;
	}
}

void BranchBoundAlgorithm::_run(JobShopNode node, JobShopNode& best, int& UB)
{
	// skip 1.1, already initialized in step 0
    // 1.2
	_printf("Algorithm 2. step 1.2 C size %zu\n", node.C().size());
	if (node.C().size() > 2)
	{
		// get E and S by proposition 4, 5, 8 and 9
		node.get_E(UB);
		node.get_S(UB);
		if (!node.E().empty() && !node.S().empty())
		{
			size_t size_all = 0;
			size_t size_alg = 0;
			std::set<std::pair<size_t, size_t>> v;
			{
				std::set<std::pair<size_t, size_t>> set_all;
				for (size_t i : node.C())
				{
					for (size_t j : node.C())
					{
						if (i == j)
						{
							continue;
						}
						set_all.insert(std::make_pair(i, j));
					}
				}
				size_all = set_all.size();
				std::set<std::pair<size_t, size_t>> set_alg;
				for (size_t i : node.E())
				{
					for (size_t j : node.S())
					{
						if (i == j)
						{
							continue;
						}
						set_alg.insert(std::make_pair(i, j));
					}
				}
				size_alg = set_alg.size();
				std::set_difference(set_all.begin(), set_all.end(), set_alg.begin(), set_alg.end(), std::inserter(v, v.begin()));
			}
			if (size_alg > size_all)
			{
				printf("%s %d, error %zu > %zu\n", __func__, __LINE__, size_alg, size_all);
			}
			_removed_node += size_all - size_alg;
			_removed_node7 += size_all - size_alg;
			_w_removed_node += (size_all - size_alg) * node.get_permutation_count(2);
			_w_removed_node7 += (size_all - size_alg) * node.get_permutation_count(2);
			int permutation_count = node.get_permutation_count(2);
			if (!v.empty())
			{
				for (const std::pair<size_t, size_t>& p : v)
				{
					if (node.E().find(p.first) == node.E().end())
					{
						if (!node.is_proposition_4_input(p.first))
						{
							++_removed_node_E_empty_prop_4;
							_w_removed_node_E_empty_prop_4 += permutation_count;
						}
						if (!node.is_proposition_7(p.first, UB))
						{
							++_removed_node_E_empty_prop_7;
							_w_removed_node_E_empty_prop_7 += permutation_count;
						}
					    ++_removed_node_E_empty_prop_5;
					    ++_removed_node_E_empty_prop_9;
				    	_w_removed_node_E_empty_prop_5 += permutation_count;
				    	_w_removed_node_E_empty_prop_9 += permutation_count;
					}
					if (node.S().find(p.second) == node.S().end())
					{
						if (!node.is_proposition_4_input(p.second))
						{
							++_removed_node_S_empty_prop_4;
							_w_removed_node_S_empty_prop_4 += permutation_count;
						}
						if (!node.is_proposition_7(p.second, UB))
						{
							++_removed_node_S_empty_prop_7;
							_w_removed_node_S_empty_prop_7 += permutation_count;
						}
					    ++_removed_node_S_empty_prop_5;
					    ++_removed_node_S_empty_prop_8;
				    	_w_removed_node_S_empty_prop_5 += permutation_count;
				    	_w_removed_node_S_empty_prop_8 += permutation_count;
					}
				}
			}
		}
		if(node.E().empty() || node.S().empty())
		{
			// remove node
			// go to step 3
			++_removed_node;
			++_removed_node1;
			_w_removed_node += node.get_permutation_count();
			_w_removed_node1 += node.get_permutation_count();
			return;
		}
		else 
		{
			_printf("step 1.3\n");
			// go step 1.3
			// Removal of input and output jobs which satisfy Proposition 13.
			std::vector<size_t> to_erase;
			for (size_t e : node.E())
			{
				int tempLB = _fix_input(node, e);
				if (tempLB >= UB)
				{
					to_erase.push_back(e);
				}
			}
			for (size_t e : to_erase)
			{
				node.erase_E(e);
			}
			to_erase.clear();
			for (size_t s : node.S())
			{
				int tempLB = _fix_output(node, s);
				if (tempLB >= UB)
				{
					to_erase.push_back(s);
				}
			}
			for (size_t s : to_erase)
			{
				node.erase_S(s);
			}
			to_erase.clear();
			if (node.E().empty() || node.S().empty())
			{
			    // remove node
			    // go to step 3
			    _printf("%s %d, remove node, go to step 3\n", __func__, __LINE__);
			    ++_removed_node;
			    ++_removed_node2;
			    _w_removed_node += node.get_permutation_count();
			    _w_removed_node2 += node.get_permutation_count();
				if (node.E().empty())
				{
    			    ++_removed_node2_fix_input;
    			    _w_removed_node2_fix_input += node.get_permutation_count();
				}
				if (node.S().empty())
				{
    			    ++_removed_node2_fix_output;
    			    ++_w_removed_node2_fix_output += node.get_permutation_count();
				}
				return;
			}
			// go to step 1.4
		}
	}
	else if (node.C().size() == 2)
	{
	    std::vector<size_t> C(node.C().begin(), node.C().end());
	    size_t i = C.front();
	    size_t j = C.back();
		if (node.is_proposition_11(i, j, UB) )
		{
			// go to step 2
			JobShopNode beta = node;
			// jobs are already assigned into input/output in is_proposition_11 function
	        //beta.add_input(i);
	        //beta.add_output(j);
	        _update_best(beta, best, UB);
			return;
		}
		else if (node.is_proposition_11(j, i, UB) )
		{
			// go to step 2
			JobShopNode beta = node;
			// jobs are already assigned into input/output in is_proposition_11 function
	        //beta.add_input(j);
	        //beta.add_output(i);
	        _update_best(beta, best, UB);
			return;
		}
		else
		{
			// go to step 1.4
			return;
		}
	}
	else if (node.C().size() == 1)
	{
		// go to step 2
		for (size_t i : node.C())
		{
			JobShopNode beta = node;
	        beta.add_output(i);
			_update_best(beta, best, UB);
		}
		return;
	}
	else 
    {
		// how about empty C?
		printf("error, empty C\n");
		return;
	}
	// 1.3
	// 1.4
	_printf("step 1.4\n");
	_branching_process(node, UB);
	// 1.5
	if (node.C().empty())
	{
		// go to step 2
	    _update_best(node, best, UB);
		return;
	}
	else
	{
		// go to step 1?
	}
	// step 2
	//_update_best(node, best, UB);
}

void BranchBoundAlgorithm::_branching_process(const JobShopNode& alpha, int UB)
{
	_printf("%s %d, Branch\n", __func__, __LINE__);
	for (size_t e : alpha.E())
	{
		for (size_t s : alpha.S())
		{
			if (e == s)
			{
				continue;
			}
			// for each pair of e,s
			_branching_process(alpha, e, s, UB);
		}
	}
}

void BranchBoundAlgorithm::_branching_process(JobShopNode beta, size_t e, size_t s, int UB)
{
	// step 1
	// step 2
	beta.add_input(e);
	beta.add_output(s);

	// step 3
	beta.eq4_and_5(e);
	if (!beta.eq6_and_7(s))
	{
		// if ls > us, remove node beta
		++_removed_node;
		++_removed_node3;
		_w_removed_node += beta.get_permutation_count();
		_w_removed_node3 += beta.get_permutation_count();
		return;
	}

	// step 4
	if (beta.is_proposition_3(e))
	{
		// remove node beta
		++_removed_node;
		++_removed_node4;
		_w_removed_node += beta.get_permutation_count();
		_w_removed_node4 += beta.get_permutation_count();
		return;
	}

	// step 5
	if (!beta.is_root())
	{
		beta.eq8_and_9(s);
	}

	// step 6
	beta.eq10_and_11(e);

	// step 7
	beta.calculate_LB();
	int LB1 = beta.get_LB();
	int LB2 = beta.get_max_l_p_q();

	if (LB1 >= UB
		|| LB2 >= UB)
	{
		// remove node beta
		++_removed_node;
		++_removed_node5;
		_w_removed_node += beta.get_permutation_count();
		_w_removed_node5 += beta.get_permutation_count();
		return;
	}

	beta.clear_E_S();
	_queue.push(beta);
}

void BranchBoundAlgorithm::_update_best(const JobShopNode& node, JobShopNode& best, int& UB) 
{
	_printf("Algorithm 2. step 2. update best\n");
	// algorithm 2.
	// Step 2
	int C_max = 0;
	if (!node.is_feasible(C_max))
	{
		++_removed_node;
		++_removed_node6;
		_w_removed_node += node.get_permutation_count();
		_w_removed_node6 += node.get_permutation_count();
		return;
	}
	if (UB <= C_max)
	{
		++_removed_node;
		++_removed_node6;
		_w_removed_node += node.get_permutation_count();
		_w_removed_node6 += node.get_permutation_count();
		return;
	}

	best = node;
	UB = C_max;
}

int BranchBoundAlgorithm::_fix_input(const JobShopNode& node, size_t e) const
{
	// step 0
	JobShopNode temp = node;
	temp.add_input(e);

	// step 1
	temp.eq4_and_5(e);

	// step 2
	if (!temp.is_root())
	{
		temp.eq8_and_9();
	}

	// step 3
	if (temp.is_root())
	{
		temp.update_root_tail(e);
	}
	else 
	{
		temp.eq10_and_11(e);
	}

	// prop 1
	temp.calculate_LB();
	int LB1 = temp.get_LB();
	int LB2 = temp.get_max_l_p_q();

	return std::max(LB1, LB2);
}

int BranchBoundAlgorithm::_fix_output(const JobShopNode& node, size_t s) const
{
	// step 0
	JobShopNode temp = node;
	temp.add_output(s);

	// step 1
	if (!temp.eq6_and_7(s))
	{
		// if ls > us, remove s from S
		return M;
	}

	// step 2
	if (!temp.is_root())
	{
		temp.eq8_and_9(s);
	}

	// step 3
	temp.eq11();

	// step 4
	temp.calculate_LB();
	int LB1 = temp.get_LB();
	int LB2 = temp.get_max_l_p_q();

	return std::max(LB1, LB2);
}

void Permutation::run(const JobShopNode& root) const
{
	// min C max
	size_t size = root.get_job_size();
	std::vector<size_t> seq(size);
	std::iota(seq.begin(), seq.end(), 0);
	int C_max = M;
	do {
		int C = 0;
		if (!root.is_feasible(seq, C))
		{
			continue;
		}
		if (C < C_max)
		{
			C_max = C;
		}
	} while (std::next_permutation(seq.begin() + 1, seq.end() - 1));
	printf("C max = %d\n", C_max);
}

int Permutation::get_first_C_max(const JobShopNode& root, std::vector<size_t>& seq) const
{
	// get first feasible solution from permutation
	size_t size = root.get_job_size();
	seq.clear();
	seq.resize(size);
	std::iota(seq.begin(), seq.end(), 0);
	int C_max = M;
	do {
		int C;
		if (!root.is_feasible(seq, C))
		{
			continue;
		}
		C_max = C;
		break;
	} while (std::next_permutation(seq.begin() + 1, seq.end() - 1));

	return C_max;
}

int Permutation::get_max_C_max(const JobShopNode& root, std::vector<size_t>& seq) const
{
	// get max C max feasible solution from permutation
	size_t size = root.get_job_size();
	seq.clear();
	seq.resize(size);
	std::iota(seq.begin(), seq.end(), 0);
	int C_max = 0;
	std::vector<size_t> seq_max;
	do {
		int C;
		if (!root.is_feasible(seq, C))
		{
			continue;
		}
		if (C > C_max)
		{
			C_max = C;
			seq_max = seq;
		}
	} while (std::next_permutation(seq.begin() + 1, seq.end() - 1));

	printf("C max = %d\n", C_max);
	seq = seq_max;
	return C_max;
}

int Permutation::get_min_C_max(const JobShopNode& root, std::vector<size_t>& seq, int& total_node) const
{
	// get min C max feasible solution from permutation
	size_t size = root.get_job_size();
	seq.clear();
	seq.resize(size);
	std::iota(seq.begin(), seq.end(), 0);
	int C_max = std::numeric_limits<int>::max();
	std::vector<size_t> seq_max;
	int count = 0;
	do {
		if (count > 0 && count % 10000 == 0)
		{
			printf("  Run %d nodes...\n", count);
		}
		++count;
		int C;
		if (!root.is_feasible(seq, C))
		{
			continue;
		}
		if (C < C_max)
		{
			C_max = C;
			seq_max = seq;
		}
	} while (std::next_permutation(seq.begin() + 1, seq.end() - 1));

	total_node = count;
	printf("total run node = %d\n", count);
	seq = seq_max;
	return C_max;
}

