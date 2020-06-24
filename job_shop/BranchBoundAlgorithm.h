#pragma once
#include "JobShopNode.h"
#include <vector>
#include <queue>

class BranchBoundAlgorithm
{
public:
	BranchBoundAlgorithm() : 
		_queue(), 
		_removed_node(0),
		_removed_node1(0),
		_removed_node2(0),
		_removed_node3(0),
		_removed_node4(0),
		_removed_node5(0)
	{}

	void run(JobShopNode root, int& total_node, int& C_max);
private:
	void _run(JobShopNode node, JobShopNode& best, int& UB);
	void _branching_process(const JobShopNode& alpha, int UB);
	void _branching_process(JobShopNode beta, size_t e, size_t s, int UB);
	void _update_best(const JobShopNode& node, JobShopNode& best, int& UB) const;
	int _fix_input(const JobShopNode& node, size_t e) const;
	int _fix_output(const JobShopNode& node, size_t s) const;

	std::priority_queue<JobShopNode, std::vector<JobShopNode>, std::less<JobShopNode> > _queue;
	int _removed_node;
	int _removed_node1;
	int _removed_node2;
	int _removed_node3;
	int _removed_node4;
	int _removed_node5;
};

class Permutation
{
public:
	void run(const JobShopNode& root) const;
	int get_first_C_max(const JobShopNode& root, std::vector<size_t>& seq) const;
	int get_max_C_max(const JobShopNode& root, std::vector<size_t>& seq) const;
	int get_min_C_max(const JobShopNode& root, std::vector<size_t>& seq, int& total_node) const;
};
