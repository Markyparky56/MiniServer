#pragma once
#include <cassert>
#include <stack>

// Class to manage a set of unique IDs, allowing for allocation and return of IDs
// back into the pool. 
// Uses a stack to track which IDs have been allocated. When an ID is returned it
// is pushed back onto the stack.
class IdPool
{
public:
	IdPool(const unsigned int Size)
		: size(Size)
		, numUsed(0)
	{
		for (unsigned int i = 0; i < Size; i++)
		{
			unused.push(Size - i);
		}
	}

	inline unsigned int GetNextID()
	{
		assert(!unused.empty());
		unsigned int id = unused.top();
		unused.pop();
		numUsed++;
		return id;
	}

	inline void ReturnID(const unsigned int Id)
	{
		unused.push(Id);
		numUsed--;
	}

	inline unsigned int GetSize() { return size; }
	inline unsigned int GetNumUsed() { return numUsed; }

private:
	std::stack<unsigned int> unused;
	unsigned int size;
	unsigned int numUsed;
};
