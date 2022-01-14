#pragma once
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

namespace mf
{
	template <typename DataType>
	class ListNode
	{

		template <typename DataType2>
		friend class LinkedList;

	private:

		DataType data;

		ListNode<DataType>* next;

		ListNode();

		ListNode(DataType data, ListNode<DataType>* next);

	public:

		DataType& getData();

		ListNode<DataType>* getNext() const;

	};


	template <typename DataType>
	class LinkedList
	{
	public:

		class Iterator
		{
		private:
			ListNode<DataType>* currentListNode;

		public:
			Iterator(ListNode<DataType>* node) : currentListNode(node) {}
			void operator++() { currentListNode = currentListNode->next; }
			bool operator!=(const Iterator& other) const { return currentListNode != other.currentListNode; }
			DataType& operator*() const { return currentListNode->data; }
		};

	protected:

		ListNode<DataType>* head = nullptr;

		ListNode<DataType>* tail = nullptr;

		int size = 0 ;

		int lastListNodeIndex = 0;

		ListNode<DataType>* lastListNode = head;

	public:

		LinkedList();

		LinkedList(const LinkedList<DataType>& linkedlist);

		int getSize() const;

		ListNode<DataType>* getHead() const;

		ListNode<DataType>* getTail() const;

		void pushFront(const DataType& data);

		void pushBack(const DataType& data);

		DataType* find(const DataType& data) const;

		bool popFront();

		void remove(const DataType& data);

		Iterator begin() const
		{
			return Iterator(head);
		}

		Iterator end() const
		{
			return Iterator(nullptr);
		}

		DataType& operator [] (int index);

		const DataType& operator[] (int index) const;

		void operator + (const LinkedList<DataType>& list);

		void operator - (LinkedList<DataType>& list);

		void operator = (const LinkedList<DataType>& list);

		void destroy();

		~LinkedList();
	};

	template <typename DataType>
	ListNode<DataType>::ListNode() {}

	template <typename DataType>
	ListNode<DataType>::ListNode(DataType data, ListNode<DataType>* next) : data(data), next(next){}

	template <typename DataType>
	DataType& ListNode<DataType>::getData()
	{
		return data;
	}

	template <typename DataType>
	ListNode<DataType>* ListNode<DataType>::getNext() const
	{
		return next;
	}

	template <typename DataType>
	LinkedList<DataType>::LinkedList() {}

	template <typename DataType>
	LinkedList<DataType>::LinkedList(const LinkedList<DataType>& linkedlist)
	{

		ListNode<DataType>* node = linkedlist.head;

		while (node != nullptr)
		{
			pushBack(node->data);

			node = node->next;
		}
	}

	template <typename DataType>
	int LinkedList<DataType>::getSize() const
	{
		return size;
	}

	template <typename DataType>
	ListNode<DataType>* LinkedList<DataType>::getHead() const
	{
		return head;
	}

	template <typename DataType>
	ListNode<DataType>* LinkedList<DataType>::getTail() const
	{
		return tail;
	}

	template <typename DataType>
	void LinkedList<DataType>::pushFront(const DataType& data)
	{
		ListNode<DataType>* node = new ListNode<DataType>(data, head);

		if (size == 0)
		{
			tail = node;
		}

		head = node;

		size++;
	}

	template <typename DataType>
	void LinkedList<DataType>::pushBack(const DataType& data)
	{
		if (size == 0)
		{
			pushFront(data);

			return;
		}

		ListNode<DataType>* node = new ListNode<DataType>(data, nullptr);

		tail->next = node;

		tail = node;

		size++;

		return;
	}

	template <typename DataType>
	DataType* LinkedList<DataType>::find(const DataType& data) const
	{
		ListNode<DataType>* temp = head;

		while (temp != nullptr)
		{
			if (temp->data == data)
			{
				return &(temp->data);
			}

			temp = temp->next;
		}

		return nullptr;
	}

	template <typename DataType>
	bool LinkedList<DataType>::popFront()
	{
		if (size == 0)
		{
			return false;
		}

		if (size == 1)
		{
			tail = nullptr;
		}

		ListNode<DataType>* temp = head;

		head = head->next;

		delete temp;

		size--;

		return true;
	}

	template <typename DataType>
	void LinkedList<DataType>::remove(const DataType& data)
	{
		ListNode<DataType>* cur = head ;

		if (size == 0)
		{
			throw "List is empty";
		}

		if (head->data == data)
		{
			head = head->next;
			delete cur;
			size--;
			lastListNode = head;
			lastListNodeIndex = 0;

			if (size == 0)
			{
				tail = nullptr;
			}

			return;
		}

		while (true)
		{
			if (cur->next == nullptr)
			{
				return;
			}

			if (cur->next->data == data)
			{
				break;
			}

			cur = cur->next;
		}

		if (tail->data == data)
		{
			tail = cur;
		}

		ListNode<DataType>* toRemove = cur->next;

		cur->next = toRemove->next;
		delete toRemove;
		size--;
		lastListNode = head;
		lastListNodeIndex = 0;
	}

	template <typename DataType>
	const DataType& LinkedList<DataType>::operator [] (int index) const
	{
		ListNode<DataType>* node = head;

		while (index > 0)
		{
			node = node->next;
			--index;
		}

		return node->getData();
	}

	template <typename DataType>
	DataType& LinkedList<DataType>::operator [] (int index)
	{
		if (index >= lastListNodeIndex)
		{
			if (lastListNodeIndex == 0)
			{
				lastListNode = head;
			}

			for (int i = 0; i < index - lastListNodeIndex; ++i)
			{
				lastListNode = lastListNode->getNext();
			}

			lastListNodeIndex = index;

			return lastListNode->data;
		}

		ListNode<DataType>* temp = head;

		int step = 0;

		while (true)
		{
			if (step == index)
			{
				lastListNodeIndex = index;
				lastListNode = temp;

				return temp->data;
			}

			temp = temp->next;

			step++;
		}
	}


	template <typename DataType>
	void LinkedList<DataType>::operator + (const LinkedList<DataType>& list)
	{
		for (ListNode<DataType>* node = list.head; node != nullptr; node = node->next)
		{
			pushBack(node->data);
		}
	}

	template <typename DataType>
	void LinkedList<DataType>::operator - (LinkedList<DataType>& list)
	{
		for (int i = 0; i <= list.size - 1; i++)
		{
			remove(list[i].data);
		}
	}

	template <typename DataType>
	void LinkedList<DataType>::operator = (const LinkedList<DataType>& list)
	{
		destroy();
		*this + (list);
	}


	template <typename DataType>
	void LinkedList<DataType>::destroy()
	{
		while (size != 0)
		{
			remove(head->data);
		}
	}

	template <typename DataType>
	LinkedList<DataType>::~LinkedList()
	{
		destroy();
	}
	

	template <typename DataType>
	LinkedList<DataType> getIntersection(const LinkedList<DataType>& list1, const LinkedList<DataType>& list2)
	{
		LinkedList<DataType> intersection;

		for (DataType& cur : list1)
		{
			if (list2.find(cur) != nullptr)
			{
				intersection.pushBack(cur);
			}
		}

		return intersection;
	}

}


#endif // LINKEDLIST_H
