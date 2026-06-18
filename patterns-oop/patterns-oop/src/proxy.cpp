#include <doctest/doctest.h>

#include <map>
#include <string>
#include <memory>

namespace {
    std::map<std::string, std::string> s_document_source { {"report", "content of report"},
                                                           {"letter", "content of letter"} };

    class idocument
    {
    public:
        virtual ~idocument() = default;

        virtual size_t get_size() const = 0;
        virtual const std::string &get_content() const = 0;
    };

    class document
        : public idocument
    {
    public:
        explicit document(const std::string &name)
            : m_content(s_document_source.at(name))
        {}

        size_t get_size() const override
        {
            return m_content.size();
        }

        const std::string &get_content() const override
        {
            return m_content;
        }

    private:
        std::string m_content;
    };

    class document_proxy
        : public idocument
    {
    public:
        explicit document_proxy(const std::string &name)
            : m_name(name)
            , m_size(s_document_source.at(name).size())
        {}

        size_t get_size() const override
        {
            return m_size;
        }

        const std::string &get_content() const override
        {
            return get_document()->get_content();
        }

    private:
        document *get_document() const
        {
            if(!m_document) {
                m_document = std::make_unique<document>(m_name);
            }
            return m_document.get();
        }

    private:
        std::string m_name;
        size_t m_size;
        mutable std::unique_ptr<document> m_document;
    };
}

TEST_SUITE("Proxy") {
    TEST_CASE("Document") {
        std::unique_ptr<idocument> sut = std::make_unique<document_proxy>("report");

        CHECK(sut->get_size() == strlen("content of report"));
        CHECK(sut->get_content() == "content of report");
    }
}
