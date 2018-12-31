#include "stdafx.h"
#include "CppUnitTest.h"

#include "../FraudulentActivityNotifications/BinaryTreeBalanced.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BinaryTreeBalancedTest
{		
	TEST_CLASS(BinaryTreeBalancedTestClass)
	{
	private:
		BinaryTreeBalanced BuildTestTree(int startValue, int endValue)
		{
			BinaryTreeBalanced tree;
			for (int i = startValue; i <= endValue; ++i)
			{
				tree.Add(i);
			}
			return tree;
		}

	public:
		TEST_METHOD(TreeAdd_KeepsSortedValues)
		{
			BinaryTreeBalanced tree;
			tree.Add(11);
			tree.Add(9);
			tree.Add(1);
			tree.Add(7);
			tree.Add(8);
			tree.Add(15);
			tree.Add(2);

			{
				// Assert that the tree has the initial expected structure
				std::vector<int> treeValues = tree.GetValues_DepthTraversal();
				std::vector<int> expected
				{
								8,
						2,				11,
					1,		7,		9,		  15
				};
				Assert::IsTrue(treeValues == expected);
			}

			// Now insert a 'middle' value for the right branch
			tree.Add(10);

			// And a 'middle' value for the left branch
			tree.Add(3);

			{
				// Assert that the tree has the sorted expected structure
				std::vector<int> treeValues = tree.GetValues_DepthTraversal();
				std::vector<int> expected
				{
									8,
							3,				10,
						2,	   7,		9,		11,
					1,								15
				};

				Assert::IsTrue(treeValues == expected);
			}
		}

		TEST_METHOD(TreeAdd_UnsortedValues)
		{
			BinaryTreeBalanced tree;
			tree.Add(11);
			tree.Add(9);
			tree.Add(1);
			tree.Add(7);
			tree.Add(8);
			tree.Add(15);
			tree.Add(2);
			tree.Add(12);
			tree.Add(3);
			tree.Add(6);
			tree.Add(10);
			tree.Add(4);
			tree.Add(5);
			tree.Add(13);
			tree.Add(14);

			Assert::AreEqual(8, tree.GetMidValue());

			std::vector<int> treeValues = tree.GetValues_DepthTraversal();
			std::vector<int> expected
			{
								8,
						4,				12,
					2,		6,		10,		  14,
				  1,  3,  5,  7,  9,   11,  13,  15
			};
			Assert::IsTrue(treeValues == expected);
		}

		TEST_METHOD(TreeAdd_AscendingValues)
		{
			BinaryTreeBalanced tree;
			for (int i = 1; i <= 15; ++i)
			{
				tree.Add(i);
			}
			
			Assert::AreEqual(8, tree.GetMidValue());

			std::vector<int> treeValues = tree.GetValues_DepthTraversal();
			std::vector<int> expected
			{ 
								8,
						4,				12, 
					2,		6,		10,		 14, 
				  1,  3,  5,  7,  9,   11, 13,  15 
			};
			Assert::IsTrue(treeValues == expected);
		}

		TEST_METHOD(TreeRemove_TreeRoots)
		{
			BinaryTreeBalanced tree = BuildTestTree(1, 15);

			std::vector<int> expectedRootValues{8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
			for (int expectedRoot: expectedRootValues)
			{
				Assert::AreEqual(expectedRoot, tree.GetMidValue());
				tree.Remove(expectedRoot);
			}
		}

		TEST_METHOD(TreeRemove_ValuesInAscendingOrder)
		{
			int startValue = 1;
			int endValue = 15;
			BinaryTreeBalanced tree = BuildTestTree(startValue, endValue);

			std::vector<int> expectedRootValues{ 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15 };
			size_t expectedRootIndex = 0;

			for (int i = startValue; i <= endValue; ++i)
			{
				int expectedRoot = expectedRootValues[expectedRootIndex];
				++expectedRootIndex;

				Assert::AreEqual(expectedRoot, tree.GetMidValue());
				
				tree.Remove(i);
			}

			Assert::IsTrue(tree.IsEmpty());
		}

		TEST_METHOD(TreeRemove_ValuesInDescendingOrder)
		{
			int startValue = 1;
			int endValue = 15;
			BinaryTreeBalanced tree = BuildTestTree(1, 15);

			std::vector<int> expectedRootValues{ 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1 };
			size_t expectedRootIndex = 0;

			for (int i = endValue; i >= startValue; --i)
			{
				int expectedRoot = expectedRootValues[expectedRootIndex];
				++expectedRootIndex;

				Assert::AreEqual(expectedRoot, tree.GetMidValue());

				tree.Remove(i);
			}

			Assert::IsTrue(tree.IsEmpty());
		}
	};
}