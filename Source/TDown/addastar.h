#ifndef AddAstarH
#define AddAstarH

const int c_HeapSizeForAstarList = 8192;

typedef struct AstarListIt
{
	int				idx;
	AstarListIt*	prev;
	AstarListIt*	next;
} AstarListIt;

class AstarList
{
public:
	AstarList()
	{
		iSize = 0;
		iHeap = 0;
		heap.push_back(new AstarListIt[c_HeapSizeForAstarList]);
		head = tail = heap[iHeap]; tail->prev = head;
	}
	~AstarList()
	{
		//for (size_t i = 0; i < heap.size(); ++i)
		delete[] heap[0];
	}
	AstarListIt* begin() {return head;}
	AstarListIt* end() {return tail;}
	AstarListIt* push_back(int idx)
	{
		++iSize;
		if (iSize == c_HeapSizeForAstarList)
			resize();
		AstarListIt* it = heap[iHeap]+iSize;
		tail->next = it;
		tail->idx = idx;
		it->prev = tail;
		tail = it;
		return tail->prev;
	}
	int	front()
	{
		AstarListIt* it = head;
		int idx =it->idx;
		if (head != tail)
			head = head->next;
		return idx;
	}
	void erase(AstarListIt* it)
	{
		if (it != head)
		{
			it->prev->next = it->next;
			it->next->prev = it->prev;
		}
		else
			head = it->next;
	}
	bool empty() { return tail == head; }
	AstarListIt* insert(AstarListIt* itNext, int idx)
	{
		++iSize;
		if(iSize == c_HeapSizeForAstarList)
			resize();
		AstarListIt* it = heap[iHeap]+iSize;
		it->next = itNext;
		it->idx = idx;
		if (itNext != head)
		{
			itNext->prev->next = it;
			it->prev = itNext->prev;
		}
		else
		{
			head = it;
		}
		itNext->prev = it;
		return it;
	}
	void clear()
	{
		for (int i = 1; i <= iHeap; ++i)
			delete[] heap[i];
		head = tail = heap[0];
		tail->prev = head;
		iHeap = 0;
		iSize = 0;
	}
	void resize()
	{
		iSize = 0;
		++iHeap;
		if (iHeap == (int)heap.size())
			heap.push_back(new AstarListIt[c_HeapSizeForAstarList]);
		else
			heap[iHeap] = new AstarListIt[c_HeapSizeForAstarList];
	}

protected:
	AstarListIt*				head;
	AstarListIt*				tail;
	std::vector<AstarListIt*>	heap;
	int							iSize;
	int							iHeap;
};

class AstarHash
{
public:
	typedef struct PathHashTable{
		AstarListIt*	it;
		int				value;
		bool			closed;
	} PathHashTable;

	PathHashTable*	Table;
	int				Size;
	int				Power;
	int				Mask;

	AstarHash(int power): Power(power), Size(1<<power)
	{ Table = new PathHashTable[Size]; Clear(); }
	~AstarHash(){delete [] Table;}
	void Clear() { ZeroMemory(Table, Size*sizeof(PathHashTable)); }
	void Add(int value, AstarListIt* it, bool closed)
	{
		int idx = value%Size;
		int iCount = 0;
		while(Table[idx].it)
		{
			++idx;
			if (idx >= Size)
				idx = 0;
			++iCount;
			if (iCount > Size/2)
				Resize();
		}
		Table[idx].value = value;
		Table[idx].it = it;
		Table[idx].closed = closed;
	}
	void Resize()
	{
		++Power;
		Size = (1<<Power);
		PathHashTable* tmp = new PathHashTable[Size];
		for (int i = 0; i < Size/2; ++i)
		{
			if (Table[i].it)
			{
				int idx = Table[i].value%Size;
				while(tmp[idx].it)
				{
					++idx;
					if (idx >= Size)
						idx = 0;
				}
				tmp[idx] = Table[i];
			}
		}
		delete [] Table;
		Table = tmp;
	}
	AstarListIt* Find(int value, bool closed)
	{
		int idx = value%Size;
		while(Table[idx].it && Table[idx].value != value)
		{
			++idx;
			if (idx >= Size)
				idx = 0;
		}
		if (Table[idx].it && Table[idx].closed != closed)
			return 0;
		return Table[idx].it;
	}
	void Erase(int value)
	{
		int idx = value%Size;
		while(Table[idx].it && Table[idx].value != value)
		{
			++idx;
			if (idx >= Size)
				idx = 0;
		}
		Table[idx].it = 0;
	}
	void Check()
	{
		for (int i = 0; i < Size; ++i)
		{
			if (Table[i].it)
			{
				if (Table[i].value != Table[i].it->idx)
				{
					bool isCatch = true;
				}
			}
		}
	}
};

#endif