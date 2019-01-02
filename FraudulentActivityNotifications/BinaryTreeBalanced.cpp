#include "stdafx.h"
#include "BinaryTreeBalanced.h"

int BinaryTreeBalanced::s_rebalanceCount = 0;

BinaryTreeBalanced::BinaryTreeBalanced()
{

}

void BinaryTreeBalanced::Add(int value)
{
	Add(value, /*rebalanceTree:*/true);
}

void BinaryTreeBalanced::Add(int value, bool rebalanceTree)
{
	if (!m_root)
	{
		m_root = std::make_unique<TreeNode>(value);
	}
	else
	{
		InsertValue(m_root.get(), value);
		RebalanceTree(m_root, rebalanceTree);
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
				if (!leftBranch->m_right || (value < leftBranch->m_right->m_value))
				{
					// Take its place
					TreeNodePtr oldLeftBranch = DetachTreeNodeBranch(leftBranch);
					TreeNodePtr newLeftBranch = std::make_unique<TreeNode>(value);
					SetTreeNodeBranch(newLeftBranch.get(), newLeftBranch->m_right, DetachTreeNodeBranch(oldLeftBranch->m_right));
					SetTreeNodeBranch(newLeftBranch.get(), newLeftBranch->m_left, std::move(oldLeftBranch));

					SetTreeNodeBranch(root, root->m_left, std::move(newLeftBranch));
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
			SetTreeNodeBranch(root, root->m_left, std::make_unique<TreeNode>(value));
		}
	}
	else if(value > root->m_value)
	{
		TreeNodePtr& rightBranch = root->m_right;
		if (rightBranch)
		{
			if (value < rightBranch->m_value)
			{
				// Check if we can do an in-between insert
				if (!rightBranch->m_left || (value > rightBranch->m_left->m_value))
				{
					// Take its place
					TreeNodePtr oldRightBranch = DetachTreeNodeBranch(rightBranch);
					TreeNodePtr newRightBranch = std::make_unique<TreeNode>(value);
					SetTreeNodeBranch(newRightBranch.get(), newRightBranch->m_left, DetachTreeNodeBranch(oldRightBranch->m_left));
					SetTreeNodeBranch(newRightBranch.get(), newRightBranch->m_right, std::move(oldRightBranch));

					SetTreeNodeBranch(root, root->m_right, std::move(newRightBranch));
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
			SetTreeNodeBranch(root, root->m_right, std::make_unique<TreeNode>(value));
		}
	}
	else
	{
		// Duplicate value. Increase ref count;
		++root->m_refCount;
	}
}

void BinaryTreeBalanced::SetTreeNodeBranch(TreeNode* treeNode, TreeNodePtr& branchToReplace, TreeNodePtr newBranch)
{
	branchToReplace = std::move(newBranch);
	if (branchToReplace)
	{
		branchToReplace->m_parent = treeNode;
	}

	UpdateNodesCountInHierarchy(treeNode);
}

TreeNodePtr BinaryTreeBalanced::DetachTreeNodeBranch(TreeNodePtr& treeNodeBranch)
{
	TreeNodePtr detachedBranch;
	if (treeNodeBranch)
	{
		detachedBranch = std::move(treeNodeBranch);
		UpdateNodesCountInHierarchy(detachedBranch->m_parent);
		detachedBranch->m_parent = nullptr;
	}
	return detachedBranch;
}

void BinaryTreeBalanced::UpdateNodesCountInHierarchy(TreeNode* parent)
{
	while (parent)
	{
		int leftNodesCount = GetNodesCount(parent->m_left.get());
		int rightNodesCount = GetNodesCount(parent->m_right.get());
		parent->m_nodesCount = leftNodesCount + 1 + rightNodesCount;
		
		int nodesDiff = std::abs(leftNodesCount - rightNodesCount);
		if (nodesDiff > 1)
		{
			parent->m_needsRebalance = true;
		}
		else if ((parent->m_left && parent->m_left->m_needsRebalance) ||
			(parent->m_right && parent->m_right->m_needsRebalance))
		{
			parent->m_needsRebalance = true;
		}
		else
		{
			parent->m_needsRebalance = false;
		}

		parent = parent->m_parent;
	}
}

bool BinaryTreeBalanced::Remove(int value)
{
	return Remove(value, /*rebalanceTree:*/true);
}

bool BinaryTreeBalanced::Remove(int value, bool rebalanceTree)
{
	bool wasFound = false;
	TreeNode* nodeToRemove = FindTreeNode(m_root.get(), value);
	if (nodeToRemove)
	{
		wasFound = true;
		--nodeToRemove->m_refCount;
		if (nodeToRemove->m_refCount <= 0)
		{
			RemoveTreeNode(nodeToRemove);
			if (rebalanceTree)
			{
				RebalanceTree(m_root);
			}
		}
	}
	return wasFound;
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

	TreeNodePtr replacementNode;
	if (nodeToRemove->m_left && nodeToRemove->m_right)
	{
		// Pick the replacement node from the branch that has the fewer nodes.
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
	}
	else if (!nodeToRemove->m_left)
	{
		// Replace with right branch
		replacementNode = std::move(nodeToRemove->m_right);
	}
	else if (!nodeToRemove->m_right)
	{
		// Replace with left branch
		replacementNode = std::move(nodeToRemove->m_left);
	}

	// Replace the branch in the hierarchy
	SetTreeNodeBranch(originalParent, *branchToReplace, std::move(replacementNode));
}

bool BinaryTreeBalanced::ReplaceValue(int valueToRemove, int valueToAdd)
{
	bool retValue = Remove(valueToRemove, /*rebalanceTree:*/false);
	Add(valueToAdd, /*rebalanceTree:*/false);
	return retValue;
}

void BinaryTreeBalanced::RebalanceTree(TreeNodePtr& root, bool balanceSubChildren)
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

			// Keep the remainder of the right root branch.
			newRightNode = DetachTreeNodeBranch(root->m_right);

			// The old root becomes the new left branch
			newLeftNode = DetachTreeNodeBranch(root);
		}
		else
		{
			// Find the largest node in left root branch.
			// Make it the new root.
			newRoot = FetchLargestLeftNode(root.get());

			// Keep the remainder of the left root branch.
			newLeftNode = DetachTreeNodeBranch(root->m_left);

			// The old root becomes the new right branch
			newRightNode = DetachTreeNodeBranch(root);
		}

		// Update the tree hierarchy
		SetTreeNodeBranch(newRoot.get(), newRoot->m_left, std::move(newLeftNode));
		SetTreeNodeBranch(newRoot.get(), newRoot->m_right, std::move(newRightNode));
		SetTreeNodeBranch(rootParent, root, std::move(newRoot));

		// Check the nodes count again
		leftNodesCount = GetNodesCount(root->m_left.get());
		rightNodesCount = GetNodesCount(root->m_right.get());
		nodesDiff = std::abs(leftNodesCount - rightNodesCount);
	}

	if (balanceSubChildren)
	{
		if (root->m_left && root->m_left->m_needsRebalance)
		{
			RebalanceTree(root->m_left);
		}
		if (root->m_right && root->m_right->m_needsRebalance)
		{
			RebalanceTree(root->m_right);
		}
	}
	
	root->m_needsRebalance = false;

	++s_rebalanceCount;
}

TreeNodePtr BinaryTreeBalanced::FetchLargestLeftNode(TreeNode* root)
{
	TreeNodePtr largestLeftNode;
	if (TreeNodePtr* foundNodeBranch = FindLargestLeftNode(root))
	{
		TreeNode* foundNodeParent = (*foundNodeBranch)->m_parent;
		largestLeftNode = DetachTreeNodeBranch(*foundNodeBranch);

		// If the largestLeftNode has a left child, 
		// then put it in its place in the tree.
		// Note: it cannot have a right child because 
		// it's the right-most node.
		if (largestLeftNode->m_left)
		{
			TreeNodePtr leftBranch = DetachTreeNodeBranch(largestLeftNode->m_left);
			SetTreeNodeBranch(foundNodeParent, *foundNodeBranch, std::move(leftBranch));
		}
	}
	return largestLeftNode;
}

TreeNodePtr BinaryTreeBalanced::FetchSmallestRightNode(TreeNode* root)
{
	TreeNodePtr smallestRightNode;
	if (TreeNodePtr* foundNodeBranch = FindSmallestRightNode(root))
	{
		TreeNode* foundNodeParent = (*foundNodeBranch)->m_parent;
		smallestRightNode = DetachTreeNodeBranch(*foundNodeBranch);

		// If the smallestRightNode has a right child, 
		// then put it in its place in the tree.
		// Note: it cannot have a left child because 
		// it's the left-most node.
		if (smallestRightNode->m_right)
		{
			TreeNodePtr rightBranch = DetachTreeNodeBranch(smallestRightNode->m_right);
			SetTreeNodeBranch(foundNodeParent, *foundNodeBranch, std::move(rightBranch));
		}
	}
	return smallestRightNode;
}

TreeNodePtr* BinaryTreeBalanced::FindLargestLeftNode(TreeNode* root)
{
	TreeNodePtr* foundNodeBranch = nullptr;
	if (root && root->m_left)
	{
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
	}
	return foundNodeBranch;
}

TreeNodePtr* BinaryTreeBalanced::FindSmallestRightNode(TreeNode* root)
{
	TreeNodePtr* foundNodeBranch = nullptr;
	if (root && root->m_right)
	{
		if (root->m_right->m_left)
		{
			// The smallest node will be the left-most node of the right-root-branch.
			TreeNode* parent = FindLeftMostNodeParent(root->m_right.get());
			foundNodeBranch = &parent->m_left;
		}
		else
		{
			foundNodeBranch = &root->m_right;
		}
	}
	return foundNodeBranch;
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

			TreeNodePtr* smallestRightNode = FindSmallestRightNode(m_root.get());
			midValues.second = (*smallestRightNode)->m_value;
		}
		else
		{
			// Left-leaning tree
			TreeNodePtr* largestLeftNode = FindLargestLeftNode(m_root.get());
			midValues.first = (*largestLeftNode)->m_value;
			
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

size_t BinaryTreeBalanced::GetNodesCount(TreeNode* root)
{
	size_t nodesCount = root ? root->m_nodesCount : 0;
	return nodesCount;
}