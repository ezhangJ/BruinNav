#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED
// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#include <iostream>
#include <queue>

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node
	{
		KeyType m_key;
		ValueType m_value;

		//Node* m_parent;
		Node* m_left;
		Node* m_right;
	};

	Node* m_head;
	int m_numNodes;
	
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
	m_head = nullptr;
	m_numNodes = 0;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
	if (m_head == nullptr)
		return;

	Node* ptr = m_head;
	std::queue<Node*> nodes;

	nodes.push(ptr);

	while (!nodes.empty())
	{
		Node* node = nodes.front();
		nodes.pop();
		if (node != nullptr)
		{
			if (node->m_left != nullptr)
				nodes.push(node->m_left);
			if (node->m_right != nullptr)
				nodes.push(node->m_right);
		}
		delete node;
	}
	m_numNodes = 0;

}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
	return m_numNodes;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{

	if (m_head == nullptr)
	{
		m_head = new Node;
		m_head->m_value = value;
		m_head->m_key = key;
		m_head->m_left = nullptr;
		m_head->m_right = nullptr;
		m_numNodes++;
	}

	Node* ptr = m_head;

	while (ptr != nullptr)
	{
		if (ptr->m_key == key)
		{
			ptr->m_value = value;
			return;
		}

		if (ptr->m_key > key)
		{
			if (ptr->m_left == nullptr)
			{
				ptr->m_left = new Node;
				ptr->m_left->m_value = value;
				ptr->m_left->m_key = key;
				ptr->m_left->m_left = nullptr;
				ptr->m_left->m_right = nullptr;
				m_numNodes++;
				return;
			}
			ptr = ptr->m_left;
		}

		if (ptr->m_key < key)
		{
			if (ptr->m_right == nullptr)
			{
				ptr->m_right = new Node;
				ptr->m_right->m_value = value;
				ptr->m_right->m_key = key;
				ptr->m_right->m_left = nullptr;
				ptr->m_right->m_right = nullptr;
				m_numNodes++;
				return;
			}
			ptr = ptr->m_right;
		}
	}
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
	Node* ptr = m_head;

	while (ptr != nullptr)
	{
		if (ptr->m_key == key)
			return &ptr->m_value;
		else if (ptr->m_key > key)
			ptr = ptr->m_left;	
		else if (ptr->m_key < key)
			ptr = ptr->m_right;
	}
	
	return nullptr;
}
#endif 