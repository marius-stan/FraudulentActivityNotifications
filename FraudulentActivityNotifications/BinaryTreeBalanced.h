#pragma once

#include <memory>
#include <vector>
#include <utility>

class TreeNode;
using TreeNodePtr = std::unique_ptr<TreeNode>;

class TreeNode
{
public:
	TreeNode(int value = 0)
		: m_parent(nullptr)
		, m_nodesCount(1)
		, m_refCount(1)
		, m_needsRebalance(false)
		, m_value(value)
	{}

public:
	TreeNode* m_parent;
	TreeNodePtr m_left;
	TreeNodePtr m_right;
	size_t m_nodesCount;
	size_t m_refCount;
	bool m_needsRebalance;
	int m_value;
};

class BinaryTreeBalanced
{
public:
	BinaryTreeBalanced();

	void Add(int value);
	bool Remove(int value);
	bool ReplaceValue(int valueToRemove, int valueToAdd);
	bool IsEmpty() const;
	size_t GetNodesCount() const;

	int GetMidValue() const;
	std::pair<int, int> GetMidValues() const;

	std::vector<int> GetValues_DepthTraversal() const;
	std::vector<int> GetValues_InOrder() const;

	template<typename InputIterator>
	void AddValues(InputIterator beginIt, InputIterator endIt)
	{
		for (InputIterator it = beginIt; it != endIt; ++it)
		{
			Add(*it, /*rebalanceTree:*/false);
		}
		RebalanceTree(m_root);
	}

private:
	void Add(int value, bool rebalanceTree);
	bool Remove(int value, bool rebalanceTree);

	static void InsertValue(TreeNode* root, int value);
	static void RebalanceTree(TreeNodePtr& root);

	static size_t GetNodesCount(TreeNode* root);

	static void SetTreeNodeBranch(TreeNode* treeNode, TreeNodePtr& branchToReplace, TreeNodePtr newBranch);
	static TreeNodePtr DetachTreeNodeBranch(TreeNodePtr& treeNodeBranch);

	static TreeNode* FindTreeNode(TreeNode* root, int value);

	static std::vector<int> GetValues_InOrder(TreeNode* root);

	static TreeNodePtr FetchLargestLeftNode(TreeNode* root);
	static TreeNodePtr FetchSmallestRightNode(TreeNode* root);
	
	static TreeNode* FindRightMostNodeParent(TreeNode* root);
	static TreeNode* FindLeftMostNodeParent(TreeNode* root);

	static void UpdateNodesCountInHierarchy(TreeNode* parent);

	void RemoveTreeNode(TreeNode* nodeToRemove);

	static TreeNodePtr* FindLargestLeftNode(TreeNode* root);
	static TreeNodePtr* FindSmallestRightNode(TreeNode* root);
private:
	TreeNodePtr m_root;

	static int s_rebalanceCount;
};