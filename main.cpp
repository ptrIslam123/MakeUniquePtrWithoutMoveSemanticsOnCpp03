#include <gtest/gtest.h>

#include <vector>
#include <list>
#include <array>
#include <set>

#include "unique_ptr.h"

struct Foo {
    Foo(size_t id = 0): id_(id) {}
    size_t id() const {
        return  id_;
    }
    ~Foo() {}

private:
    size_t id_;
};


TEST(UniquePtr, CreateEmptyUniquePtr) {
    mem::UniquePtr<Foo> p;
}

TEST(UniquePtr, CreateUniquePtr) {
    mem::UniquePtr<Foo> p(new Foo());
    EXPECT_EQ(p->id(), 0);
}

TEST(UniquePtr, CreateMoreOneItems) {
    mem::UniquePtr<Foo> p1(new Foo(10));
    mem::UniquePtr<Foo> p2(new Foo(23));

    mem::UniquePtr<Foo> p3(p1.move());

    EXPECT_NE(p2->id(), p3->id());
}

TEST(UniquePtr, WorkWithVector) {
    std::vector<mem::UniquePtr<Foo>::RvalueUniquePtr> data;
    mem::UniquePtr<Foo> p1(new Foo(10));
    mem::UniquePtr<Foo> p2(new Foo(20));
    mem::UniquePtr<Foo> p3(p1.move());

    data.push_back(p2.move());
    data.push_back(p3.move());

    mem::UniquePtr<Foo> cp2(data[0]);
    mem::UniquePtr<Foo> cp3(data[1]);

    EXPECT_EQ(cp2->id(), 20);
    EXPECT_EQ(cp3->id(), 10);
}

TEST(UniquePtr, WorkWithList) {
    std::list<mem::UniquePtr<Foo>::RvalueUniquePtr> data;
    mem::UniquePtr<Foo> p1(new Foo());
    mem::UniquePtr<Foo> p2(new Foo(12));
    mem::UniquePtr<Foo> p3(p2.move());


    data.push_back(p1.move());
    data.push_back(p3.move());

    typedef std::list<mem::UniquePtr<Foo>::RvalueUniquePtr>::iterator It;
    It it = data.begin();
    mem::UniquePtr<Foo> cp1(*it);
    std::advance(it, 1);
    mem::UniquePtr<Foo> cp2(*it);

    EXPECT_EQ(cp1->id(), 0);
    EXPECT_EQ(cp2->id(), 12);
}

TEST(UniquePtr, WorkWithArray) {
    std::array<mem::UniquePtr<Foo>, 10> data;
    mem::UniquePtr<Foo> p1(new Foo());
    mem::UniquePtr<Foo> p2(new Foo(23));

    data[0] = p1.move();
    data[1] = p2.move();

    mem::UniquePtr<Foo>::RvalueUniquePtr rcp1(data[0].move());
    mem::UniquePtr<Foo>::RvalueUniquePtr rcp2(data[1].move());
    mem::UniquePtr<Foo> cp1(rcp1);
    mem::UniquePtr<Foo> cp2(rcp2);

    EXPECT_EQ(cp1->id(), 0);
    EXPECT_EQ(cp2->id(), 23);
}


struct CmpUniquePtr {
    bool operator() (const mem::UniquePtr<Foo>::RvalueUniquePtr& a, const mem::UniquePtr<Foo>::RvalueUniquePtr& b) const {
        const mem::UniquePtr<Foo> ca(a);
        const mem::UniquePtr<Foo> cb(b);
        return ca->id() < cb->id();
    }
};

TEST(UniquePtr, WorkWithSet) {
    std::set<mem::UniquePtr<Foo>::RvalueUniquePtr, CmpUniquePtr> data;
    mem::UniquePtr<Foo> p1(new Foo(10));
    mem::UniquePtr<Foo> p2(new Foo(23));

    data.insert(p1.move());
    data.insert(p2.move());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}