#include <doctest/doctest.h>
#include <vector>
#include <memory>

namespace {
    class text_document;
    class table;

    class ivisitor
    {
    public:
        virtual ~ivisitor() = default;

        virtual void visit(const text_document *doc) = 0;
        virtual void visit(const table *doc) = 0;
    };

    class idocument
    {
    public:
        virtual ~idocument() = default;
        virtual void accept(ivisitor *visitor) const = 0;
    };

    class text_document
        : public idocument
    {
    public:
        explicit text_document(size_t chars_count)
            : m_chars_count(chars_count)
        {}

        size_t get_chars_count() const
        {
            return m_chars_count;
        }

        void accept(ivisitor *visitor) const override
        {
            visitor->visit(this);
        }

    private:
        size_t m_chars_count;
    };

    class table
        : public idocument
    {
    public:
        explicit table(size_t cells_count)
            : m_cells_count(cells_count)
        {}

        size_t get_cells_count() const
        {
            return m_cells_count;
        }

        void accept(ivisitor *visitor) const override
        {
            visitor->visit(this);
        }

    private:
        size_t m_cells_count;
    };

    class size_visitor
        : public ivisitor
    {
    public:
        void visit(const text_document *doc) override
        {
            m_size = doc->get_chars_count();
        }

        void visit(const table *doc) override
        {
            m_size = doc->get_cells_count();
        }

        size_t get_size() const
        {
            return m_size;
        }

    private:
        size_t m_size = 0;
    };

    class size_accumulate_visitor
        : public ivisitor
    {
    public:
        void visit(const text_document *doc) override
        {
            m_size += doc->get_chars_count();
        }

        void visit(const table *doc) override
        {
            m_size += doc->get_cells_count();
        }

        size_t get_total_size() const
        {
            return m_size;
        }

    private:
        size_t m_size = 0;
    };
}

TEST_SUITE("Visitor") {
    TEST_CASE("Text document") {
        text_document doc(40);
        size_visitor visitor;

        doc.accept(&visitor);

        CHECK(visitor.get_size() == 40);
    }

    TEST_CASE("Table") {
        table doc(50);
        size_visitor visitor;

        doc.accept(&visitor);

        CHECK(visitor.get_size() == 50);
    }

    TEST_CASE("Collection") {
        std::vector<std::unique_ptr<idocument>> collection;
        collection.emplace_back(std::make_unique<text_document>(34));
        collection.emplace_back(std::make_unique<table>(66));

        size_accumulate_visitor visitor;
        for(auto &el : collection) {
            el->accept(&visitor);
        }

        CHECK(visitor.get_total_size() == 100);
    }
}
