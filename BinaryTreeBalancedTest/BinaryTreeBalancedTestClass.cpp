#include "stdafx.h"
#include "CppUnitTest.h"

#include "../FraudulentActivityNotifications/BinaryTreeBalanced.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BinaryTreeBalancedTest
{		
	TEST_CLASS(BinaryTreeBalancedTestClass)
	{
	public:
		TEST_METHOD(InsertTest_KeepSortedValues)
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

		TEST_METHOD(InsertTest_UnsortedValues)
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

		TEST_METHOD(InsertTest_AscendingValues)
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

		TEST_METHOD(RemoveTest_01)
		{
			BinaryTreeBalanced tree;
			for (int i = 1; i <= 15; ++i)
			{
				tree.Add(i);
			}

			std::vector<int> expectedRootValues{8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
			for (int expectedRoot: expectedRootValues)
			{
				Assert::AreEqual(expectedRoot, tree.GetMidValue());
				tree.Remove(expectedRoot);
			}
		}
	};
}