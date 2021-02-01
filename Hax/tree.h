/*
 * Generic N-ary TreeMap data structure supporting move semantics.
 * 
 * It is designed to be stateful, and so does not easily support recursive operations.
 * Rather it maintains a pointer to the current node which can be advanced to it's children,
 * or "ascended" to it's parent.
 */


#pragma once
#include <memory>
#include <unordered_map>
#include "debug.h"


namespace Hax
{
	template<class K, class V, class Hash = std::hash<K>>
	class Tree
	{
	private:
		struct Node;
		using Pointer = std::unique_ptr<Node>;
		using Container = std::unordered_map<K, Pointer, Hash>;

		struct Node
		{
			template<class... Args>
			Node(Args&& ...args) : data(std::forward<Args>(args)...), parent(nullptr) {}
			
			V data;
			Container children;
			Node* parent;
		};

		template<class... Args>
		Pointer MakePtr(Args&&... args)
		{
			return std::make_unique<Node>(std::forward<Args>(args)...);
		}
		
		void RegisterChild(const K& key)
		{
			ptr->children[key]->parent = ptr;
		}

		Pointer root;
		Node* ptr;

	public:
		
		Tree(V&& data) noexcept : root(MakePtr(std::move(data)))
		{
			ptr = root.get();
		}

		Tree(const V& data) : root(MakePtr(data))
		{
			ptr = root.get();
		}

		Tree(const Tree&) = delete;
		Tree& operator=(const Tree&) = delete;

		Tree(Tree&& other) noexcept
		{
			root.swap(other.root);
			ptr = root.get();
		}

		//Returns data stored in the current tree node.
		V& Data()
		{
			return ptr->data;
		}
		
		//Returns the number of children of the current tree node.
		size_t Size() const
		{
			return ptr->children.size();
		}

		//Insert a new child in to the current tree node with the specified key and value(s).
		template<class... Args>
		void Insert(K&& key, Args&&... args)
		{
			ptr->children[std::move(key)] = (MakePtr(std::forward<Args>(args)...));
			RegisterChild(key);
		}

		//Insert a new child in to the current node with the specified key and value(s).
		template<class... Args>
		void Insert(const K& key, Args&&... args)
		{
			ptr->children[key] = (MakePtr(std::forward<Args>(args)...));
			RegisterChild(key);
		}

		//Returns true if the current node has no parent (i.e. is the root of the tree).
		bool IsRoot() const
		{
			return ptr->parent == nullptr;
		}

		//Returns true if the current node has no children.
		bool IsLeaf() const
		{
			return Size() == 0;
		}

		//Returns true if the current node has a child with the specified key.
		bool HasChild(const K& key) const
		{
			return ptr->children.find(key) != ptr->children.end();
		}
		
		//Sets the current node equal to it's parent.
		void Ascend()
		{
			D(if (IsRoot()) throw std::logic_error("Cannot call ascend on root"));
			ptr = ptr->parent;
		}

		//Sets the current node equal to the specified child of the current node.
		void Descend(const K& key)
		{
			D(if (IsLeaf()) throw std::logic_error("Cannot call descend on leaf"));
			D(if (!HasChild(key)) throw std::out_of_range("Key not found"));
			ptr = ptr->children[key].get();
		}

		//Resets the current node to the root of the tree.
		void Reset()
		{
			ptr = root.get();
		}

		//Returns a reference to the data stored in the child of the
		//current node with the specified key.
		V& Child(const K& key)
		{
			D(if (!HasChild(key)) throw std::out_of_range("key not found"));
			return ptr->children[key]->data;
		}

		//Returns a reference to the data stored in the child of the
		//current node with the specified key.
		const V& Child(const K& key) const
		{
			D(if (!HasChild(key)) throw std::out_of_range("key not found"));
			return ptr->children[key]->data;
		}

		//Destroys the tree leaving an empty root with no children.
		void ClearAll()
		{
			Reset();
			root->children.clear();
		}
	};
}