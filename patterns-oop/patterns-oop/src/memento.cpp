#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <stdexcept>

namespace {
    class document;

    class memento
    {
        friend document;

        explicit memento(const std::string &content)
            : m_content(content)
        {}

        std::string m_content;
    };

    class document
    {
    public:
        void set_content(const std::string &content)
        {
            m_content = content;
        }

        const std::string &get_content() const
        {
            return m_content;
        }

        memento create_memento() const
        {
            return memento{ m_content };
        }

        void set_memento(memento &&memento)
        {
            m_content = std::move(memento.m_content);
        }

    private:
        std::string m_content;
    };

    class caretaker
    {
    public:
        void add(memento &&memento)
        {
            m_mementos.push_back(std::move(memento));
        }

        memento extract_last()
        {
            if(m_mementos.empty()) {
                throw std::logic_error("no memento");
            }
            auto ans = std::move(m_mementos.back());
            m_mementos.pop_back();
            return ans;
        }

    private:
        std::vector<memento> m_mementos;
    };
}

TEST_SUITE("Memento") {
    TEST_CASE("Document") {
        caretaker caretaker;
        document sut;
        CHECK(sut.get_content() == "");

        sut.set_content("content");
        caretaker.add(sut.create_memento());
        CHECK(sut.get_content() == "content");

        sut.set_content("some error");
        sut.set_memento(caretaker.extract_last());
        CHECK(sut.get_content() == "content");
    }
}
