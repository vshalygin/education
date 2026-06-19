#include <doctest/doctest.h>

#include <vector>
#include <string>
#include <stdexcept>

namespace {
    class book_collection
    {
    public:
        class iterator
        {
            friend book_collection;

            iterator(std::vector<std::string> &collection)
                : m_collection(collection)
            {}

        public:
            std::string &get_value()
            {
                return m_collection.at(m_curr_idx);
            }

            void next()
            {
                if(is_done()) {
                    throw std::out_of_range("out of range");
                }

                ++m_curr_idx;
            }

            bool is_done() const
            {
                return m_curr_idx == m_collection.size();
            }

        private:
            std::vector<std::string> &m_collection;
            size_t m_curr_idx = 0;
        };

        explicit book_collection(std::initializer_list<const char *> collection)
        {
            for(auto el : collection) {
                m_collection.push_back(el);
            }
        }

        iterator create_iterator()
        {
            return iterator(m_collection);
        }

    private:
        std::vector<std::string> m_collection;
    };
}

TEST_SUITE("Iterator") {
    TEST_CASE("Book collection") {
        book_collection sut({ "War and Peace", "Moby Dick" });

        std::vector<std::string> ans;
        for(auto it = sut.create_iterator(); !it.is_done(); it.next()) {
            ans.push_back(it.get_value());
        }

        REQUIRE(ans.size() == 2);
        CHECK(ans[0] == "War and Peace");
        CHECK(ans[1] == "Moby Dick");
    }
}
