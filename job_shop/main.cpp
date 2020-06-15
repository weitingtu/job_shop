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
	printf("Machine %zu Job %zu\n", Machine, J);
	__int64 run_time1 = 0;
	__int64 run_time2 = 0;
	for (size_t i = 0; i < Machine; ++i)
	{
		JobShopNode root = JobShopNode::create_random_node(J);
		{
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			printf("Run Algorithm\n");
			BranchBoundAlgorithm bb;
			bb.run(root);

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			run_time1 += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
			//printf("Time difference = %lld [ms]\n", std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count());
		}

		{
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			printf("Run Exhaustive\n");
			Permutation pp;
			std::vector<size_t> seq;
			pp.get_min_C_max(root, seq);
			//for (size_t i = 0; i < seq.size(); ++i)
			//{
				//printf("%zu ", seq.at(i));
			//}
			//printf("\n");
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			run_time2 += std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
			//printf("Time difference = %lld [ms]\n", std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count());
		}
	}

	printf("Run Algorithm\n");
	printf("Time difference = %lld [ms]\n", run_time1);
	printf("Run Exhaustive\n");
	printf("Time difference = %lld [ms]\n", run_time2);

	return 0;
}