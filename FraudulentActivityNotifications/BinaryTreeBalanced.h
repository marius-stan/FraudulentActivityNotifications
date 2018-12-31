#pragma once

#include <memory>
#include <vector>

class TreeNode;
using TreeNodePtr = std::unique_ptr<TreeNode>;

class TreeNode
{
public:
	TreeNode(int value = 0)
		: m_parent(nullptr)
		, m_value(value)
	{}

public:
	TreeNode* m_parent;
	TreeNodePtr m_left;
	TreeNodePtr m_right;
	int m_value;
};

class BinaryTreeBalanced
{
public:
	BinaryTreeBalanced();

	void Add(int value);
	void Remove(int value);
	bool IsEmpty() const;
	size_t GetNodesCount() const;

	int GetMidValue() const;
	int GetMidValueLeft() const;
	int GetMidValueRight() const;

	std::vector<int> GetValues_DepthTraversal() const;
	std::vector<int> GetValues_InOrder() const;

private:
	static void InsertValue(TreeNode* root, int value);
	static void RebalanceTree(TreeNodePtr& root);

	static int GetNodesCount(TreeNode* root);

	static void SetTreeNodeBranchLeft(TreeNode* treeNode, TreeNodePtr newRightBranch);
	static void SetTreeNodeBranchRight(TreeNode* treeNode, TreeNodePtr newRightBranch);

	static TreeNode* FindTreeNode(TreeNode* root, int value);

	static std::vector<int> GetValues_InOrder(TreeNode* root);

	static TreeNodePtr FetchLargestLeftNode(TreeNode* root);
	static TreeNodePtr FetchSmallestRightNode(TreeNode* root);
	
	void RemoveTreeNode(TreeNode* nodeToRemove);
private:
	TreeNodePtr m_root;
};