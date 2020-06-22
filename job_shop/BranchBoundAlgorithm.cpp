#include "BranchBoundAlgorithm.h"
#include "type.h"
#include <numeric>
#include <algorithm>
#include <stdarg.h>

static bool print_step = false;

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
void BranchBoundAlgorithm::run(JobShopNode root, int& total_node)
{
	JobShopNode best = root;
	int UB = M;
	{
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
		_printf("Algorithm 2. Step 1. Select node count %d\n", ++count);
		// Step 1. Selection of a node with the lowest lower bound
		JobShopNode node = _queue.top();
		_queue.pop();

		_run(node, best, UB);
	}

	//printf("UB = %d, total run node %d\n", UB, count);
	total_node = count;
	printf("total run node %d\n", count);
	printf("removed node   %d\n", _removed_node);
	
	//int C_max = M;
	//if (!best.is_feasible(C_max))
	//{
	//	printf("%s %d, error best is not feasible\n", __func__, __LINE__);
	//	return;
	//}
	//printf("C max = %d\n", C_max);
	//best.print();
}

void BranchBoundAlgorithm::_run(JobShopNode node, JobShopNode& best, int& UB)
{
	// skip 1.1
    // 1.2
	_printf("Algorithm 2. step 1.2 C size %zu\n", node.C().size());
	if (node.C().size() > 2)
	{
		// get E and S by proposition 4, 5, 8 and 9
		node.get_E(UB);
		node.get_S(UB);
		if(node.E().empty() || node.S().empty())
		{
			// remove node
			// go to step 3
			//printf("%s %d, E or S is empty, remove node, go to step 3\n", __func__, __LINE__);
			++_removed_node;
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
				//printf("fix input temp LB %d UB %d\n", tempLB, UB);
				if (tempLB >= UB)
				{
					//printf("tempLB %d > UB %d, erase e %d\n", tempLB, UB, e);
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
				//printf("fix output temp LB %d UB %d\n", tempLB, UB);
				if (tempLB >= UB)
				{
					//printf("erase s %d\n", s);
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
	        //beta.add_input(i);
	        //beta.add_output(j);
	        _update_best(beta, best, UB);
			return;
		}
		else if (node.is_proposition_11(j, i, UB) )
		{
			// go to step 2
			JobShopNode beta = node;
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
		return;
	}

	// step 4
	if (beta.is_proposition_3(e))
	{
		// remove node beta
		++_removed_node;
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
		return;
	}

	beta.clear_E_S();
	_queue.push(beta);
}

void BranchBoundAlgorithm::_update_best(const JobShopNode& node, JobShopNode& best, int& UB) const
{
	_printf("Algorithm 2. step 2. update best\n");
	// algorithm 2.
	// step 2
	int C_max = 0;
	if (!node.is_feasible(C_max))
	{
		return;
	}
	if (UB <= C_max)
	{
		return;
	}
	//printf("update best from %d -> %d\n", UB, C_max);
	//best.print();
	//best.print_internal();

	//node.print();
	//node.print_internal();

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
		//for (size_t i : seq)
		//{
		//	printf("%zu ", i);
		//}
		//printf("\n");

		if (!root.is_feasible(seq, C))
		{
			continue;
		}
		//printf("feasible!\n");
		if (C < C_max)
		{
			C_max = C;
		}
	} while (std::next_permutation(seq.begin() + 1, seq.end() - 1));
	printf("C max = %d\n", C_max);
}

int Permutation::get_first_C_max(const JobShopNode& root, std::vector<size_t>& seq) const
{
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

	//printf("C max = %d\n", C_max);
	return C_max;
}

int Permutation::get_max_C_max(const JobShopNode& root, std::vector<size_t>& seq) const
{
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
	size_t size = root.get_job_size();
	seq.clear();
	seq.resize(size);
	std::iota(seq.begin(), seq.end(), 0);
	int C_max = std::numeric_limits<int>::max();
	std::vector<size_t> seq_max;
	int count = 0;
	do {
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
	//printf("C max = %d\n", C_max);
	seq = seq_max;
	return C_max;
}

