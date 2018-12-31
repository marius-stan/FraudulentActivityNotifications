#include "stdafx.h"
#include "BinaryTreeBalanced.h"

#include <sstream>

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
					SetTreeNodeBranchRight(newLeftBranch.get(), std::move(oldLeftBranch->m_right));
					SetTreeNodeBranchLeft(newLeftBranch.get(), std::move(oldLeftBranch));

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
					SetTreeNodeBranchLeft(newRightBranch.get(), std::move(oldRightBranch->m_left));
					SetTreeNodeBranchRight(newRightBranch.get(), std::move(oldRightBranch));

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

void BinaryTreeBalanced::SetTreeNodeBranchLeft(TreeNode* treeNode, TreeNodePtr newLeftBranch)
{
	if (treeNode)
	{
		treeNode->m_left = std::move(newLeftBranch);
		if (treeNode->m_left)
		{
			treeNode->m_left->m_parent = treeNode;
		}
	}
}

void BinaryTreeBalanced::SetTreeNodeBranchRight(TreeNode* treeNode, TreeNodePtr newRightBranch)
{
	if (treeNode)
	{
		treeNode->m_right = std::move(newRightBranch);
		if (treeNode->m_right)
		{
			treeNode->m_right->m_parent = treeNode;
		}
	}
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
			root = root->m_left.get();
		}
		else
		{
			root = root->m_right.get();
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
		// Pick the replacement node from the branch
		// that has the fewer nodes.
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

		if (nodeToRemove->m_left)
		{
			replacementNode->m_left = std::move(nodeToRemove->m_left);
			replacementNode->m_left->m_parent = replacementNode.get();
		}
		if (nodeToRemove->m_right)
		{
			replacementNode->m_right = std::move(nodeToRemove->m_right);
			replacementNode->m_right->m_parent = replacementNode.get();
		}

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
	if (branchToReplace)
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
		if (root->m_left->m_right)
		{
			TreeNode* rightNode = root->m_left->m_right.get();
			while (rightNode->m_right)
			{
				rightNode = rightNode->m_right.get();
			}
			largestLeftNode = std::move(rightNode->m_parent->m_right);
		}
		else
		{
			largestLeftNode = std::move(root->m_left);
		}
		largestLeftNode->m_parent = nullptr;
	}
	return largestLeftNode;
}

TreeNodePtr BinaryTreeBalanced::FetchSmallestRightNode(TreeNode* root)
{
	TreeNodePtr smallestRightNode;
	if (root && root->m_right)
	{
		if (root->m_right->m_left)
		{
			TreeNode* leftNode = root->m_right->m_left.get();
			while (leftNode->m_left)
			{
				leftNode = leftNode->m_left.get();
			}
			smallestRightNode = std::move(leftNode->m_parent->m_left);
		}
		else
		{
			smallestRightNode = std::move(root->m_right);
		}
		smallestRightNode->m_parent = nullptr;
	}
	return smallestRightNode;
}

bool BinaryTreeBalanced::IsEmpty() const
{
	return (GetNodesCount() == 0);
}

size_t BinaryTreeBalanced::GetNodesCount() const
{
	return GetNodesCount(m_root.get());
}

int BinaryTreeBalanced::GetMidValue() const
{
	return m_root->m_value;
}

int BinaryTreeBalanced::GetMidValueLeft() const
{
	return GetMidValue();
}

int BinaryTreeBalanced::GetMidValueRight() const
{
	return m_root->m_right->m_value;
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