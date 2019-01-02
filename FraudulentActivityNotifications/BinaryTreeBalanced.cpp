#include "stdafx.h"
#include "BinaryTreeBalanced.h"

BinaryTreeBalanced::BinaryTreeBalanced()
{

}

void BinaryTreeBalanced::Add(int value)
{
	if (!m_root)
	{
		m_root = std::make_unique<TreeNode>(value);
	}
	else
	{
		InsertValue(m_root.get(), value);
		RebalanceTree(m_root);
	}
}

void BinaryTreeBalanced::InsertValue(TreeNode* root, int value)
{
	if (value < root->m_value)
	{
		TreeNodePtr& leftBranch = root->m_left;
		if (leftBranch)
		{
			if (value > leftBranch->m_value)
			{
				// Check if we can do an in-between insert
				if (!leftBranch->m_right || (value <= leftBranch->m_right->m_value))
				{
					// Take its place
					TreeNodePtr oldLeftBranch = std::move(leftBranch);
					TreeNodePtr newLeftBranch = std::make_unique<TreeNode>(value);
					newLeftBranch->m_parent = root;
					SetTreeNodeBranch(newLeftBranch.get(), newLeftBranch->m_right, std::move(oldLeftBranch->m_right));
					SetTreeNodeBranch(newLeftBranch.get(), newLeftBranch->m_left, std::move(oldLeftBranch));

					leftBranch = std::move(newLeftBranch);
				}
				else
				{
					InsertValue(leftBranch.get(), value);
				}
			}
			else
			{
				InsertValue(leftBranch.get(), value);
			}
		}
		else
		{
			leftBranch = std::make_unique<TreeNode>(value);
			leftBranch->m_parent = root;
		}
	}
	else
	{
		TreeNodePtr& rightBranch = root->m_right;
		if (rightBranch)
		{
			if (value < rightBranch->m_value)
			{
				// Check if we can do an in-between insert
				if (!rightBranch->m_left || (value >= rightBranch->m_left->m_value))
				{
					// Take its place
					TreeNodePtr oldRightBranch = std::move(rightBranch);
					TreeNodePtr newRightBranch = std::make_unique<TreeNode>(value);
					newRightBranch->m_parent = root;
					SetTreeNodeBranch(newRightBranch.get(), newRightBranch->m_left, std::move(oldRightBranch->m_left));
					SetTreeNodeBranch(newRightBranch.get(), newRightBranch->m_right, std::move(oldRightBranch));

					rightBranch = std::move(newRightBranch);
				}
				else
				{
					InsertValue(rightBranch.get(), value);
				}
			}
			else
			{
				InsertValue(rightBranch.get(), value);
			}
		}
		else
		{
			rightBranch = std::make_unique<TreeNode>(value);
			rightBranch->m_parent = root;
		}
	}
}

void BinaryTreeBalanced::SetTreeNodeBranch(TreeNode* treeNode, TreeNodePtr& branchToReplace, TreeNodePtr newBranch)
{
	if (treeNode)
	{
		branchToReplace = std::move(newBranch);
		if (branchToReplace)
		{
			branchToReplace->m_parent = treeNode;
		}
	}
}

TreeNodePtr BinaryTreeBalanced::DetachTreeNodeBranch(TreeNodePtr& treeNodeBranch)
{
	TreeNodePtr detachedBranch;
	if (treeNodeBranch)
	{
		detachedBranch = std::move(treeNodeBranch);
		detachedBranch->m_parent = nullptr;
	}
	return detachedBranch;
}

void BinaryTreeBalanced::Remove(int value)
{
	TreeNode* nodeToRemove = FindTreeNode(m_root.get(), value);
	if (nodeToRemove)
	{
		RemoveTreeNode(nodeToRemove);
		RebalanceTree(m_root);
	}
}

TreeNode* BinaryTreeBalanced::FindTreeNode(TreeNode* root, int value)
{
	TreeNode* foundNode = nullptr;
	while (root)
	{
		if (root->m_value == value)
		{
			foundNode = root;
			break;
		}
		else if (root->m_value < value)
		{
			root = root->m_right.get();
		}
		else
		{
			root = root->m_left.get();
		}
	}
	return foundNode;
}

void BinaryTreeBalanced::RemoveTreeNode(TreeNode* nodeToRemove)
{
	TreeNode* originalParent = nullptr;
	TreeNodePtr* branchToReplace = nullptr;
	if (nodeToRemove == m_root.get())
	{
		branchToReplace = &m_root;
	}
	else
	{
		originalParent = nodeToRemove->m_parent;
		branchToReplace = (originalParent->m_left.get() == nodeToRemove) ?
			&originalParent->m_left : &originalParent->m_right;
	}

	if (nodeToRemove->m_left && nodeToRemove->m_right)
	{
		// Pick the replacement node from the branch that has the fewer nodes.
		TreeNodePtr replacementNode;
		int leftBranchNodesCount = GetNodesCount(nodeToRemove->m_left.get());
		int rightBranchNodesCount = GetNodesCount(nodeToRemove->m_right.get());
		if (leftBranchNodesCount < rightBranchNodesCount)
		{
			replacementNode = FetchSmallestRightNode(nodeToRemove);
		}
		else
		{
			replacementNode = FetchLargestLeftNode(nodeToRemove);
		}

		SetTreeNodeBranch(replacementNode.get(), replacementNode->m_left, std::move(nodeToRemove->m_left));
		SetTreeNodeBranch(replacementNode.get(), replacementNode->m_right, std::move(nodeToRemove->m_right));

		*branchToReplace = std::move(replacementNode);
	}
	else if (!nodeToRemove->m_left)
	{
		// Replace with right branch
		*branchToReplace = std::move(nodeToRemove->m_right);
	}
	else if (!nodeToRemove->m_right)
	{
		// Replace with left branch
		*branchToReplace = std::move(nodeToRemove->m_left);
	}
	else
	{
		// Else, just remove the node directly.
		branchToReplace->reset();
	}

	// Update with the new parent
	if (branchToReplace && *branchToReplace)
	{
		(*branchToReplace)->m_parent = originalParent;
	}
}

void BinaryTreeBalanced::RebalanceTree(TreeNodePtr& root)
{
	if (!root)
	{
		return;
	}

	int leftNodesCount = GetNodesCount(root->m_left.get());
	int rightNodesCount = GetNodesCount(root->m_right.get());
	int nodesDiff = std::abs(leftNodesCount - rightNodesCount);
	while(nodesDiff > 1)
	{
		TreeNode* rootParent = root->m_parent;
		TreeNodePtr newRoot;
		TreeNodePtr newLeftNode;
		TreeNodePtr newRightNode;
		if (leftNodesCount < rightNodesCount)
		{
			// Find smallest node in right root branch.
			// Make it the new root.
			newRoot = FetchSmallestRightNode(root.get());

			if (newRoot->m_right)
			{
				// If the newRoot already has a right node, then use that.
				newRightNode = std::move(newRoot->m_right);
			}
			else
			{
				// Otherwise keep the remainder of the right root branch.
				newRightNode = std::move(root->m_right);
			}

			// The old root becomes the new left branch
			newLeftNode = std::move(root);
		}
		else
		{
			// Find the largest node in left root branch.
			// Make it the new root.
			newRoot = FetchLargestLeftNode(root.get());

			if (newRoot->m_left)
			{
				// If the newRoot already has a left node, then use that.
				newLeftNode = std::move(newRoot->m_left);
			}
			else
			{
				// Otherwise keep the remainder of the left root branch.
				newLeftNode = std::move(root->m_left);
			}

			// The old root becomes the new right branch
			newRightNode = std::move(root);
		}

		// Update the tree hierarchy
		root = std::move(newRoot);
		root->m_left = std::move(newLeftNode);
		root->m_right = std::move(newRightNode);

		root->m_parent = rootParent;
		root->m_left->m_parent = root.get();
		root->m_right->m_parent = root.get();

		// Check the nodes count again
		leftNodesCount = GetNodesCount(root->m_left.get());
		rightNodesCount = GetNodesCount(root->m_right.get());
		nodesDiff = std::abs(leftNodesCount - rightNodesCount);
	}

	RebalanceTree(root->m_left);
	RebalanceTree(root->m_right);
}

TreeNodePtr BinaryTreeBalanced::FetchLargestLeftNode(TreeNode* root)
{
	TreeNodePtr largestLeftNode;
	if (root && root->m_left)
	{
		TreeNodePtr* foundNodeBranch = nullptr;
		if (root->m_left->m_right)
		{
			// The largest node will be the right-most node of the left-root-branch.
			TreeNode* parent = FindRightMostNodeParent(root->m_left.get());
			foundNodeBranch = &parent->m_right;
		}
		else
		{
			foundNodeBranch = &root->m_left;
		}

		TreeNode* foundNodeParent = (*foundNodeBranch)->m_parent;
		largestLeftNode = DetachTreeNodeBranch(*foundNodeBranch);

		// If the largestLeftNode has a left child, 
		// then put it in its place in the tree.
		// Note: it cannot have a right child because 
		// it's the right-most node.
		if (largestLeftNode->m_left)
		{
			SetTreeNodeBranch(foundNodeParent, *foundNodeBranch, std::move(largestLeftNode->m_left));
		}
	}
	return largestLeftNode;
}

TreeNodePtr BinaryTreeBalanced::FetchSmallestRightNode(TreeNode* root)
{
	TreeNodePtr smallestRightNode;
	if (root && root->m_right)
	{
		TreeNodePtr* foundNodeBranch = nullptr;
		if (root->m_right->m_left)
		{
			// The largest node will be the right-most node of the left-root-branch.
			TreeNode* parent = FindLeftMostNodeParent(root->m_right.get());
			foundNodeBranch = &parent->m_left;
		}
		else
		{
			foundNodeBranch = &root->m_right;
		}

		TreeNode* foundNodeParent = (*foundNodeBranch)->m_parent;
		smallestRightNode = DetachTreeNodeBranch(*foundNodeBranch);

		// If the smallestRightNode has a right child, 
		// then put it in its place in the tree.
		// Note: it cannot have a left child because 
		// it's the left-most node.
		if (smallestRightNode->m_right)
		{
			SetTreeNodeBranch(foundNodeParent, *foundNodeBranch, std::move(smallestRightNode->m_right));
		}
	}
	return smallestRightNode;
}

TreeNode* BinaryTreeBalanced::FindRightMostNodeParent(TreeNode* root)
{
	TreeNode* foundParent = nullptr;
	if (root && root->m_right)
	{
		TreeNode* rightNode = root->m_right.get();
		while (rightNode->m_right)
		{
			rightNode = rightNode->m_right.get();
		}
		foundParent = rightNode->m_parent;
	}
	return foundParent;
}

TreeNode* BinaryTreeBalanced::FindLeftMostNodeParent(TreeNode* root)
{
	TreeNode* foundParent = nullptr;
	if (root && root->m_left)
	{
		TreeNode* leftNode = root->m_left.get();
		while (leftNode->m_left)
		{
			leftNode = leftNode->m_left.get();
		}
		foundParent = leftNode->m_parent;
	}
	return foundParent;
}

bool BinaryTreeBalanced::IsEmpty() const
{
	return (m_root == nullptr);
}

size_t BinaryTreeBalanced::GetNodesCount() const
{
	return GetNodesCount(m_root.get());
}

int BinaryTreeBalanced::GetMidValue() const
{
	return m_root->m_value;
}

std::pair<int, int> BinaryTreeBalanced::GetMidValues() const
{
	size_t nodesCount = GetNodesCount();
	if (nodesCount < 2)
	{
		throw new std::exception("BinaryTreeBalanced::GetMidValues: tree has too few values.");
	}

	std::pair<int, int> midValues;
	if (nodesCount % 2 == 1)
	{
		midValues.first = m_root->m_value;
		midValues.second = m_root->m_right->m_value;
	}
	else
	{
		size_t nodesCountLeft = GetNodesCount(m_root->m_left.get());
		size_t nodesCountRight = GetNodesCount(m_root->m_right.get());
		if (nodesCountLeft < nodesCountRight)
		{
			// Right-leaning tree
			midValues.first = m_root->m_value;
			midValues.second = m_root->m_right->m_value;
		}
		else
		{
			// Left-leaning tree
			midValues.first = m_root->m_left->m_value;
			midValues.second = m_root->m_value;
		}
	}
	return midValues;
}

std::vector<int> BinaryTreeBalanced::GetValues_DepthTraversal() const
{
	std::vector<int> values;

	std::vector<TreeNode*> nodes;
	if (m_root)
	{
		nodes.push_back(m_root.get());
	}

	while (!nodes.empty())
	{
		std::vector<TreeNode*> childNodes;
		for (TreeNode* node: nodes)
		{
			values.push_back(node->m_value);
			if (node->m_left)
			{
				childNodes.push_back(node->m_left.get());
			}
			if (node->m_right)
			{
				childNodes.push_back(node->m_right.get());
			}
		}
		nodes = std::move(childNodes);
	}

	return values;
}

std::vector<int> BinaryTreeBalanced::GetValues_InOrder() const
{
	return GetValues_InOrder(m_root.get());
}

std::vector<int> BinaryTreeBalanced::GetValues_InOrder(TreeNode* root)
{
	std::vector<int> values;
	if (root)
	{
		if (root->m_left)
		{
			std::vector<int> leftValues = GetValues_InOrder(root->m_left.get());
			values.insert(values.end(), leftValues.begin(), leftValues.end());
		}
		
		values.push_back(root->m_value);
		
		if (root->m_right)
		{
			std::vector<int> rightValues = GetValues_InOrder(root->m_right.get());
			values.insert(values.end(), rightValues.begin(), rightValues.end());
		}
	}

	return values;
}

int BinaryTreeBalanced::GetNodesCount(TreeNode* root)
{
	// mvstan FIXME We need to optimize this.

	if (!root)
	{
		return 0;
	}
	
	int leftChildrenCount = 0;
	if (root->m_left)
	{
		leftChildrenCount = GetNodesCount(root->m_left.get());
	}

	int rightChildrenCount = 0;
	if (root->m_right)
	{
		rightChildrenCount = GetNodesCount(root->m_right.get());
	}

	int childrenCount = leftChildrenCount + 1 + rightChildrenCount;
	return childrenCount;
}