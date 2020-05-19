#include "BranchBoundAlgorithm.h"
#include "type.h"

// algorithm 2.
void BranchBoundAlgorithm::run(JobShopNode root)
{
	int UB = M;
	JobShopNode best = root;
	root.calculate_LB();

	int count = 0;
	_queue.push(root);
	// Step 3.
	while (!_queue.empty())
	{
		printf("Algorithm 2. Step 1. Select node count %d\n", ++count);
		// Step 1. Selection of a node with the lowest lower bound
		JobShopNode node = _queue.top();
		_queue.pop();

		_run(node, best, UB);
	}

	printf("UB = %d\n", UB);
	best.print();
}

void BranchBoundAlgorithm::_run(JobShopNode node, JobShopNode& best, int& UB)
{
	// skip 1.1
    // 1.2
	printf("Algorithm 2. step 1.2 C size %zu\n", node.C().size());
	if (node.C().size() > 2)
	{
		// get E and S by proposition 4, 5, 8 and 9
		node.get_E(UB);
		node.get_S(UB);
		if(node.E().empty() || node.S().empty())
		{
			// remove node
			// go to step 3
			printf("%s %d, E or S is empty, remove node, go to step 3\n", __func__, __LINE__);
			return;
		}
		else 
		{
			printf("step 1.3\n");
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
			    printf("%s %d, remove node, go to step 3\n", __func__, __LINE__);
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
		if (node.is_proposition_11(i, j, UB) 
			|| node.is_proposition_11(j, i, UB))
		{
			// go to step 2
	        _update_best(node, best, UB);
			return;
		}
		else
		{
			// go to step 1.4
		}
	}
	else if (node.C().size() == 1)
	{
		// go to step 2
	    _update_best(node, best, UB);
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
	printf("step 1.4\n");
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
	_update_best(node, best, UB);
}

void BranchBoundAlgorithm::_branching_process(const JobShopNode& alpha, int UB)
{
	for (size_t e : alpha.E())
	{
		for (size_t s : alpha.S())
		{
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
		return;
	}

	// step 4
	if (beta.is_proposition_3(e))
	{
		// remove node beta
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
		return;
	}

	_queue.push(beta);
}

void BranchBoundAlgorithm::_update_best(const JobShopNode& node, JobShopNode& best, int& UB) const
{
	printf("Algorithm 2. step 2. update best\n");
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
