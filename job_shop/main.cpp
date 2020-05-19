#include <stdio.h>
#include "JobShopNode.h"
#include "BranchBoundAlgorithm.h"

int main()
{
	JobShopNode root = JobShopNode::create_initial_node();

	BranchBoundAlgorithm bb;
	bb.run(root);

	return 0;
}