#include "pch.h"
#include "CppUnitTest.h"
#include "Shared/ValueQueue.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void rtassert(bool condition) {

}

namespace UnitTestShared
{
	

	TEST_CLASS(UnitTestShared)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{			
			ValueQueue<5> queue;
			queue.push(1);
			queue.push(2);
			queue.push(3);
			queue.push(4);

			for (int n = 0; n < 4; n++) {
				int value = queue.pop();
				Assert::AreEqual(n + 1, value);
			}
		}
	};
}
