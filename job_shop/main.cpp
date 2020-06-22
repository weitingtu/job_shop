#include <stdio.h>
#include "JobShopNode.h"
#include "BranchBoundAlgorithm.h"
#include <chrono>

int main()
{
	srand(0);
	size_t Machine = 5;
	size_t J = 10;
	size_t BatchSize = 2;
	size_t BatchMachine = 2;
	printf("Machine %zu Job %zu\n", Machine, J);

	std::vector<JobShopNode> roots;
	std::vector<std::pair<int, size_t>> omega_idx;
	for (size_t i = 0; i < Machine; ++i)
	{
		roots.push_back(JobShopNode::create_random_node(J));
		int omega = 0;
		if (i < BatchMachine)
		{
			omega = roots.back().get_omega(BatchSize);
		}
		else
		{
			omega = roots.back().get_omega();
		}
		omega_idx.push_back(std::make_pair(omega, i));
	}

	std::sort(omega_idx.begin(), omega_idx.end());

	__int64 run_time1 = 0;
	__int64 run_time2 = 0;
	int total_node1 = 0;
	int total_node2 = 0;
	int total_C_max = 0;
	for (size_t ii = 0; ii < Machine; ++ii)
	{
		size_t i = omega_idx.at(ii).second;

		if (i < BatchMachine)
		{
		    printf("Machine %zu (batch size %zu)\n", i, BatchSize);
		}
		else
		{
		    printf("Machine %zu\n", i);
		}
		JobShopNode root = roots.at(i);
		{
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			printf("Run Algorithm\n");
			int total_node = 0;
			BranchBoundAlgorithm bb;
			bb.run(root, total_node);
			total_node1 += total_node;

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			run_time1 += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
		}
		{
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			printf("Run Exhaustive\n");
			Permutation pp;
			std::vector<size_t> seq;
			int total_node = 0;
			int C_max = pp.get_min_C_max(root, seq, total_node);
			if (i < BatchMachine)
			{
				int C;
				//root.is_feasible(BatchSize, seq, C);
				//C_max = C;

			    std::vector<size_t> batch_seq;
				JobShopNode batch_node = JobShopNode::create_batch_node(root, seq, BatchSize, batch_seq);
				batch_node.is_feasible(batch_seq, C);
				C_max = C;
			}
			total_node2 += total_node;
			total_C_max += C_max;
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			run_time2 += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();

	        printf("C max = %d\n", C_max);
		}
	}

	printf("Run Algorithm\n");
	printf("Time difference = %lld [ms]\n", run_time1);
	printf("total run node  = %d\n", total_node1);
	printf("Run Exhaustive\n");
	printf("Time difference = %lld [ms]\n", run_time2);
	printf("total run node  = %d\n", total_node2);
	printf("total C max     = %d\n", total_C_max);

	return 0;
}