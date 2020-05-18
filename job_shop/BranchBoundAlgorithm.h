#pragma once
#include "JobShopNode.h"
#include <vector>
#include <queue>

class BranchBoundAlgorithm
{
public:
	BranchBoundAlgorithm() : _queue() {}

	void run(JobShopNode root);
private:
	void _run(JobShopNode node, JobShopNode& best, int& UB);
	void _branching_process(const JobShopNode& alpha, int UB);
	void _branching_process(JobShopNode beta, size_t e, size_t s, int UB);
	void _update_best(const JobShopNode& node, JobShopNode& best, int& UB) const;
	int _fix_input(const JobShopNode& node, size_t e) const;
	int _fix_output(const JobShopNode& node, size_t s) const;

	std::priority_queue<JobShopNode, std::vector<JobShopNode>, std::less<JobShopNode> > _queue;
};

