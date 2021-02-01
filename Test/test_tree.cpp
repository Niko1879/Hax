#include "pch.h"
#include "tree.h"
#include <string>
#include <vector>


//Fixture to test move semantics
class MoveSemanticsFixture : public ::testing::Test {
protected:
	struct CopyMoveCounter
	{
		CopyMoveCounter() : mcopy(0), mmove(0), nmcopy(0) {}
		void reset() { mcopy = 0; mmove = 0; nmcopy = 0; }
		int mcopy;
		int mmove;
		int nmcopy;
	};

	struct Moveable
	{
		bool operator==(const Moveable& other) const { return id == other.id; }
		Moveable(CopyMoveCounter& c, int id) : c(c), id(id) {}
		Moveable(const Moveable& other) : c(other.c), id(other.id) { ++c.mcopy; }
		Moveable(Moveable&& other) noexcept : c(other.c), id(other.id) { ++c.mmove; }
		int id;
		CopyMoveCounter& c;
	};

	struct NonMoveable
	{
		bool operator==(const NonMoveable& other) const { return id == other.id; }
		NonMoveable(CopyMoveCounter& c, int id) : c(c), id(id) {}
		NonMoveable(const NonMoveable& other) : c(other.c), id(other.id) { ++c.nmcopy; }
		NonMoveable(NonMoveable&& other) = delete;
		int id;
		CopyMoveCounter& c;
	};

	struct MoveableDummyHash
	{
		size_t operator()(const Moveable& m) const { return 1; }
	};

	struct NonMoveableDummyHash
	{
		size_t operator()(const NonMoveable& m) const { return 1; }
	};

	CopyMoveCounter c;
};


template<class V>
using DefaultTree = Hax::Tree<int, V>;


TEST(TestTree, TestConstructors)
{
	//basic tests
	EXPECT_NO_THROW(DefaultTree<int>(10));
	std::vector<std::string> vect{ "test" };
	EXPECT_NO_THROW(DefaultTree<std::vector<std::string>> tmp(vect));
}


TEST(TestTree, TestAccess)
{
	DefaultTree<int> t(2);
	EXPECT_EQ(t.Data(), 2);
}


TEST(TestTree, TestEmplace)
{
	struct Test
	{
		Test(int a, std::string b, float c) : a(a), b(b), c(c) {}
		int a; std::string b; float c;
	};


	DefaultTree<Test> t(Test(1, "2", 3.0));
	EXPECT_NO_THROW(t.Insert(2, 3, "2", 1.0));
	EXPECT_EQ(t.Child(2).a, 3);
	EXPECT_EQ(t.Child(2).b, "2");
	EXPECT_EQ(t.Child(2).c, 1.0);
}


TEST(TestTree, TestInsert)
{
	Hax::Tree<int, int> t(1);
	EXPECT_NO_THROW(t.Insert(2, 1));
	EXPECT_EQ(t.Child(2), 1);
}


TEST(TestTree, TestSize)
{
	DefaultTree<int> t(2);
	EXPECT_NO_THROW(t.Size());
	EXPECT_EQ(t.Size(), 0);

	t.Insert(10, 5);
	EXPECT_EQ(t.Size(), 1);
}


TEST(TestTree, TestRoaming)
{
	DefaultTree<int> t(10);
	EXPECT_TRUE(t.IsRoot());
	EXPECT_TRUE(t.IsLeaf());

	t.Insert(0, 1);
	EXPECT_NO_THROW(t.Descend(0));
	EXPECT_FALSE(t.IsRoot());
	EXPECT_TRUE(t.IsLeaf());

	EXPECT_NO_THROW(t.Ascend());
	EXPECT_FALSE(t.IsLeaf());
}


TEST_F(MoveSemanticsFixture, TestMoveSemantics)
{
	Moveable m(c, 1);
	NonMoveable n(c, 1);
	EXPECT_NO_THROW(DefaultTree<Moveable> t(std::move(m)));
	EXPECT_NO_THROW(DefaultTree<NonMoveable> u(n));

	//check that moveable is moved once and only once, and never copied
	EXPECT_EQ(c.mcopy, 0);
	EXPECT_EQ(c.mmove, 1);

	//check that non-moveable is copied only once
	EXPECT_EQ(c.nmcopy, 1);
}


TEST_F(MoveSemanticsFixture, TestMoveSemantics2)
{
	Moveable m(c, 1);
	NonMoveable n(c, 1);
	Hax::Tree<Moveable, int, MoveableDummyHash> t(1);
	Hax::Tree<NonMoveable, int, NonMoveableDummyHash> u(2);
	t.Insert(std::move(m), 1);
	u.Insert(n, 1);

	//check that moveable is moved once and only once, and never copied
	EXPECT_EQ(c.mcopy, 0);
	EXPECT_EQ(c.mmove, 1);

	//check that non-moveable is copied only once
	EXPECT_EQ(c.nmcopy, 1);
}


TEST_F(MoveSemanticsFixture, TestInsertMoveSemantics)
{
	Moveable tmp(c, 1);
	NonMoveable tmp2(c, 2);
	Moveable m(c, 3);
	NonMoveable n(c, 4);
	DefaultTree<Moveable> t(tmp);
	DefaultTree<NonMoveable> t2(tmp2);
	c.reset();
	t.Insert(1, std::move(m));
	t2.Insert(1, n);

	EXPECT_EQ(c.mcopy, 0);
	EXPECT_EQ(c.mmove, 1);
	EXPECT_EQ(c.nmcopy, 1);
}