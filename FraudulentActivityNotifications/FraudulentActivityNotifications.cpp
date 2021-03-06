// FraudulentActivityNotifications.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <fstream>
#include <cassert>
#include "BinaryTreeBalanced.h"

void ReadInput(std::istream& input, std::vector<int>& out_values, size_t& out_trailingDaysCount)
{
	size_t daysCount = 0;
	input >> daysCount >> out_trailingDaysCount;

	out_values.resize(daysCount);
	for (size_t i = 0; i < daysCount; ++i)
	{
		input >> out_values[i];
	}
}

float GetMedianValue(const BinaryTreeBalanced& trailingValueSorted)
{
	float medianValue = 0.0f;

	size_t nodesCount = trailingValueSorted.GetNodesCount();
	if (nodesCount % 2 == 1)
	{
		medianValue = static_cast<float>(trailingValueSorted.GetMidValue());
	}
	else
	{
		std::pair<int, int> midValues = trailingValueSorted.GetMidValues();
		medianValue = (midValues.first + midValues.second) / 2.0f;
	}
	return medianValue;
}

int GetFraudNotificationsCount(const std::vector<int>& values, size_t trailingDaysCount)
{
	int notificationsCount = 0;
	if ((trailingDaysCount == 0) || 
		(trailingDaysCount >= values.size()))
	{
		return notificationsCount;
	}

	BinaryTreeBalanced trailingDaysSorted;

	auto startIt = values.begin();
	auto endIt = startIt + trailingDaysCount;
	trailingDaysSorted.AddValues(startIt, endIt);

	for (size_t i = trailingDaysCount; i < values.size(); ++i)
	{
		float medianValue = GetMedianValue(trailingDaysSorted);
		int triggerValue = static_cast<int>(2 * medianValue);
		int dayValue = values[i];
		if (dayValue >= triggerValue)
		{
			++notificationsCount;
		}

		trailingDaysSorted.ReplaceValue(values[i - trailingDaysCount], values[i]);
	}
	return notificationsCount;
}

void TestProgram()
{
	{
		std::vector<int> values{ 2, 3, 4, 2, 3, 6, 8, 4, 5 };
		size_t trailingDaysCount = 5;
		int result = GetFraudNotificationsCount(values, trailingDaysCount);
		int expected = 2;
		assert(result == expected);
	}
	{
		std::vector<int> values{3, 5, 7, 1, 1, 1, 3};
		size_t trailingDaysCount = 3;
		int result = GetFraudNotificationsCount(values, trailingDaysCount);
		int expected = 1;
		assert(result == expected);
	}
	{
		std::ifstream inputFile{"TestCase01.txt"};

		std::vector<int> values;
		size_t trailingDaysCount = 0;
		ReadInput(inputFile, values, trailingDaysCount);

		int result = GetFraudNotificationsCount(values, trailingDaysCount);
		int expected = 633;
		assert(result == expected);
	}
}

void RunProgram()
{
	std::vector<int> values;
	size_t trailingDaysCount = 0;
	ReadInput(std::cin, values, trailingDaysCount);

	int result = GetFraudNotificationsCount(values, trailingDaysCount);
	std::cout << result << std::endl;
}

int main()
{
	//RunProgram();
	TestProgram();
    return 0;
}

