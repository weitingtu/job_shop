#include <stdio.h>
#include "JobShopNode.h"
#include "BranchBoundAlgorithm.h"
#include <chrono>

int main()
{
	srand(0);
	//JobShopNode root = JobShopNode::create_initial_node();
	size_t Machine = 5;
	size_t J = 10;
	size_t BatchSize = 2;
	size_t BatchMachine = 2;
	printf("Machine %zu Job %zu\n", Machine, J);
	__int64 run_time1 = 0;
	__int64 run_time2 = 0;
	int total_node1 = 0;
	int total_node2 = 0;
	int total_C_max = 0;
	for (size_t i = 0; i < Machine; ++i)
	{
		if (i < BatchMachine)
		{
		    printf("Machine %zu (batch size %zu)\n", i, BatchSize);
		}
		else
		{
		    printf("Machine %zu\n", i);
		}
		JobShopNode root = JobShopNode::create_random_node(J);
		{
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			printf("Run Algorithm\n");
			int total_node = 0;
			BranchBoundAlgorithm bb;
			bb.run(root, total_node);
			total_node1 += total_node;

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			run_time1 += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
			//printf("Time difference = %lld [ms]\n", std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count());
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
				root.is_feasible(BatchSize, seq, C);
				C_max = C;
			}
			total_node2 += total_node;
			total_C_max = std::max(total_C_max, C_max);
			//for (size_t i = 0; i < seq.size(); ++i)
			//{
				//printf("%zu ", seq.at(i));
			//}
			//printf("\n");
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			run_time2 += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
			//printf("Time difference = %lld [ms]\n", std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count());

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