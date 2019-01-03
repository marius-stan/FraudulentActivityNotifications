#include "stdafx.h"
#include "BinaryTreeBalanced.h"

BinaryTreeBalanced::BinaryTreeBalanced()
{

}

int BinaryTreeBalanced::Add(int value)
{
	return Add(value, /*rebalanceTree:*/true);
}

int BinaryTreeBalanced::Add(int value, bool rebalanceTree)
{
	int rebalanceCount = 0; // mvstan FIXME This is for debug only
	if (!m_root)
	{
		m_root = std::make_unique<TreeNode>(value);
	}
	else
	{
		InsertValue(m_root.get(), value);
		if (rebalanceTree)
		{
			rebalanceCount = RebalanceTree(m_root);
		}
	}
	return rebalanceCount;
}

void BinaryTreeBalanced::InsertValue(TreeNode* root, int value)
{
	bool wasInserted = false;
	while (!wasInserted)
	{
		if (value < root->m_value)
		{
			TreeNodePtr& leftBranch = root->m_left;
			if (leftBranch)
			{
				if (value > leftBranch->m_value)
				{
					// Check if we can do an in-between insert
					TreeNodePtr* smallestRightNode = FindSmallestRightNode(leftBranch.get());
					if (!smallestRightNode || (value < (*smallestRightNode)->m_value))
					{
						// Take its place
						TreeNodePtr oldLeftBranch = DetachTreeNodeBranch(leftBranch);
						TreeNodePtr newLeftBranch = std::make_unique<TreeNode>(value);
						SetTreeNodeBranch(newLeftBranch.get(), newLeftBranch->m_right, DetachTreeNodeBranch(oldLeftBranch->m_right));
						SetTreeNodeBranch(newLeftBranch.get(), newLeftBranch->m_left, std::move(oldLeftBranch));

						SetTreeNodeBranch(root, root->m_left, std::move(newLeftBranch));
						wasInserted = true;
					}
					else
					{
						root = leftBranch.get();
					}
				}
				else
				{
					root = leftBranch.get();
				}
			}
			else
			{
				SetTreeNodeBranch(root, root->m_left, std::make_unique<TreeNode>(value));
				wasInserted = true;
			}
		}
		else if (value > root->m_value)
		{
			TreeNodePtr& rightBranch = root->m_right;
			if (rightBranch)
			{
				if (value < rightBranch->m_value)
				{
					// Check if we can do an in-between insert
					TreeNodePtr* largestLeftNode = FindLargestLeftNode(rightBranch.get());
					if (!largestLeftNode || (value > (*largestLeftNode)->m_value))
					{
						// Take its place
						TreeNodePtr oldRightBranch = DetachTreeNodeBranch(rightBranch);
						TreeNodePtr newRightBranch = std::make_unique<TreeNode>(value);
						SetTreeNodeBranch(newRightBranch.get(), newRightBranch->m_left, DetachTreeNodeBranch(oldRightBranch->m_left));
						SetTreeNodeBranch(newRightBranch.get(), newRightBranch->m_right, std::move(oldRightBranch));

						SetTreeNodeBranch(root, root->m_right, std::move(newRightBranch));
						wasInserted = true;
					}
					else
					{
						root = rightBranch.get();
					}
				}
				else
				{
					root = rightBranch.get();
				}
			}
			else
			{
				SetTreeNodeBranch(root, root->m_right, std::make_unique<TreeNode>(value));
				wasInserted = true;
			}
		}
		else
		{
			// Duplicate value.
			// Insert it in the side that has the fewer nodes.
			if (!root->m_left)
			{
				SetTreeNodeBranch(root, root->m_left, std::make_unique<TreeNode>(value));
				wasInserted = true;
			}
			else if (!root->m_right)
			{
				SetTreeNodeBranch(root, root->m_right, std::make_unique<TreeNode>(value));
				wasInserted = true;
			}
			else
			{
				int leftNodesCount = GetNodesCount(root->m_left.get());
				int rightNodesCount = GetNodesCount(root->m_right.get());
				if (leftNodesCount <= rightNodesCount)
				{
					root = root->m_left.get();
				}
				else
				{
					root = root->m_right.get();
				}
			}
		}
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
		RemoveTreeNode(nodeToRemove);
		if (rebalanceTree)
		{
			RebalanceTree(m_root);
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
	Add(valueToAdd, /*rebalanceTree:*/true);
	return retValue;
}

int BinaryTreeBalanced::RebalanceTree(TreeNodePtr& root)
{
	int rebalanceCount = 0;
	if (!root)
	{
		return rebalanceCount;
	}

	int leftNodesCount = GetNodesCount(root->m_left.get());
	int rightNodesCount = GetNodesCount(root->m_right.get());
	int nodesDiff = std::abs(leftNodesCount - rightNodesCount);
	while(nodesDiff > 1)
	{
		++rebalanceCount;

		// Keep the original root parent
		TreeNode* rootParent = root->m_parent;

		// Detach the old root from the tree
		TreeNodePtr oldRoot = DetachTreeNodeBranch(root);

		TreeNodePtr newRoot;
		if (leftNodesCount < rightNodesCount)
		{
			// Find the smallest node in right root branch.
			// This will be the new root.
			newRoot = FetchSmallestRightNode(oldRoot.get());
		}
		else
		{
			// Find the largest node in left root branch.
			// This will be the new root.
			newRoot = FetchLargestLeftNode(oldRoot.get());
		}

		// Keep the remainder of the left root branch.
		TreeNodePtr newLeftNode = DetachTreeNodeBranch(oldRoot->m_left);
		SetTreeNodeBranch(newRoot.get(), newRoot->m_left, std::move(newLeftNode));

		// Keep the remainder of the right root branch.
		TreeNodePtr newRightNode = DetachTreeNodeBranch(oldRoot->m_right);
		SetTreeNodeBranch(newRoot.get(), newRoot->m_right, std::move(newRightNode));
		
		// Insert the value of the oldRoot in the new sub-tree
		InsertValue(newRoot.get(), oldRoot->m_value);

		// Update the tree hierarchy
		SetTreeNodeBranch(rootParent, root, std::move(newRoot));

		// Check the nodes count again
		leftNodesCount = GetNodesCount(root->m_left.get());
		rightNodesCount = GetNodesCount(root->m_right.get());
		nodesDiff = std::abs(leftNodesCount - rightNodesCount);
	}

	if (root->m_left && root->m_left->m_needsRebalance)
	{
		rebalanceCount += RebalanceTree(root->m_left);
	}
	if (root->m_right && root->m_right->m_needsRebalance)
	{
		rebalanceCount += RebalanceTree(root->m_right);
	}
	root->m_needsRebalance = false;

	return rebalanceCount;
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