#include <gtest/gtest.h>

#include <vector>
#include <list>
#include <array>
#include <set>
#include <map>
#include <memory>

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

TEST(UniquePtr, CheckMethodRelease) {
    struct Baz {
        Baz() {
            counter()++;
        }

        ~Baz() {
            counter()--;
        }

        static unsigned int &counter() {
            static unsigned int counter = 0;
            return counter;
        }
    };

    struct XXX {
        void operator() () {
            typedef mem::UniquePtr<Baz> BazPtr;
            BazPtr b1(new Baz());
            BazPtr b2(new Baz());
            BazPtr b3(new Baz());

            EXPECT_EQ(Baz::counter(), 3);

            b1.release();
        }
    };

    XXX xxx;
    xxx();
    EXPECT_EQ(Baz::counter(), 1);
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

template<class T>
struct CmpUniquePtr2 {
    bool operator() (const std::unique_ptr<T>& a, const std::unique_ptr<T>& b) const {
        return *a < *b;
    }
};

TEST(UniquePtr, WorkWithComparator) {
    typedef mem::UniquePtr<Foo> FooPtr;
    FooPtr i(new Foo(10));
    FooPtr j(new Foo(12));
    CmpUniquePtr cmp;

    EXPECT_TRUE(cmp(i.move(), j.move()));
}

TEST(UniquePtr, WorkWithSet) {

    struct Baz {
        Baz() {
            std::cout << "Baz\n";
        }
        ~Baz() {
            std::cout << "~Baz\n";
        }
    };

    std::unique_ptr<Baz> b1(new Baz());
    Baz* b2 = b1.release();

    delete b2;

//    std::set<mem::UniquePtr<Foo>::RvalueUniquePtr, CmpUniquePtr> data;
//    mem::UniquePtr<Foo> p1(new Foo(10));
//    mem::UniquePtr<Foo> p2(new Foo(23));
//
//    data.insert(p1.move());
//    data.insert(p2.move());
}

TEST(UniquePtr, WorkWithMap) {
    typedef mem::UniquePtr<Foo> FooPtr;
    std::map<int, FooPtr::RvalueUniquePtr> data;

    mem::UniquePtr<Foo> f1(new Foo(10));
    mem::UniquePtr<Foo> f2(new Foo(32));

    data.insert(std::pair<int, FooPtr::RvalueUniquePtr>(10, f1.move()));
    data.insert(std::pair<int, FooPtr::RvalueUniquePtr>(12, f2.move()));

    FooPtr cf1(data.find(10)->second);
    FooPtr cf2(data.find(12)->second);

    EXPECT_EQ(cf1->id(), 10); // Ok
    EXPECT_EQ(cf2->id(), 32); // Ok

    // FooPtr cf1(data[10]); // error
    // FooPtr cf2(data[12]); // error

}

TEST(UniquePtr, WorkwithRef) {
    typedef mem::UniquePtr<int> IntPtr;

    struct Bar {
        Bar(int id):
            data_(new int(id))
        {}

        IntPtr::RvalueUniquePtr operator() () {
            return data_.move();
        }

        IntPtr data_;
    };

    Bar bar(12);
    IntPtr val(bar());
    EXPECT_EQ(*val, 12);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}