#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace {
    enum class origin
    {
        no,
        hr,
        pr,
        accounting_department
    };

    std::string to_string(origin origin)
    {
        switch(origin) {
            case origin::hr:
                return "hr";
            case origin::pr:
                return "pr";
            case origin::accounting_department:
                return "accounting_department";
            default:
                return "no";
        }
    }

    class document
    {
    public:
        virtual ~document() = default;

        virtual void add_signature(const std::string &signature) = 0;
        virtual void set_origin(origin origin) = 0;
        virtual void set_text(const std::string &text) = 0;

        virtual std::string get_content() const = 0;

        virtual std::unique_ptr<document> clone() const = 0;
    };

    class business_letter
        : public document
    {
    public:
        void add_signature(const std::string &signature) override
        {
            m_signature = signature;
        }

        void set_origin(origin origin) override
        {
            m_origin = origin;
        }

        void set_text(const std::string &text) override
        {
            m_text = format_text(text);
        }

        std::string get_content() const override
        {
            return "From: " + to_string(m_origin) + "; " + m_text + "; Signature: " + m_signature;
        }

        std::unique_ptr<document> clone() const override
        {
            return std::make_unique<business_letter>(*this);
        }

    private:
        std::string format_text(const std::string &text)
        {
            return "Business letter: " + text;
        }

        std::string m_text;
        std::string m_signature;
        origin m_origin = origin::no;
    };

    class financial_report
        : public document
    {
    public:
        void add_signature(const std::string &signature) override
        {
            m_signature = encrypt_signature(signature);
        }

        void set_origin(origin origin) override
        {
            if(origin != origin::accounting_department) {
                throw std::logic_error("financial report is not created by accounting department");
            }
            m_origin = origin;
        }

        void set_text(const std::string &text) override
        {
            m_text = format_text(text);
        }

        std::string get_content() const override
        {
            return m_text + "; Encrypted signature: " + m_signature;
        }

        std::unique_ptr<document> clone() const override
        {
            return std::make_unique<financial_report>(*this);
        }

    private:
        std::string encrypt_signature(const std::string &signarure)
        {
            return "Encrypted(" + signarure + ")";
        }

        std::string format_text(const std::string &text)
        {
            return "Finantial report: " + text;
        }

        std::string m_text;
        std::string m_signature;
        origin m_origin = origin::no;
    };

    class sender
    {
    public:
        sender(std::unique_ptr<document> doc)
            : m_document(std::move(doc))
        {}

        virtual ~sender() = default;

        void send(const std::string &text, origin origin, const std::string &signature)
        {
            auto doc = m_document->clone();

            doc->set_origin(origin);
            doc->set_text(text);
            doc->add_signature(signature);

            do_send(doc->get_content());
        }

    private:
        virtual void do_send(const std::string & /*content*/)
        {
            /*implementation*/
        }

    private:
        std::unique_ptr<document> m_document;
    };

    class test_sender
        : public sender
    {
    public:
        using sender::sender;

        std::string get_last_sent() const
        {
            return m_last_sent;
        }

    private:
        void do_send(const std::string &content) override
        {
            m_last_sent = content;
        }

    private:
        std::string m_last_sent;
    };
}

TEST_SUITE("Prototype") {
    TEST_CASE("Business Letter") {
        test_sender sender(std::make_unique<business_letter>());
        sender.send("some text", origin::hr, "Mary Jones");

        CHECK(sender.get_last_sent() == "From: hr; Business letter: some text; Signature: Mary Jones");
    }

    TEST_CASE("Financial report") {
        test_sender sender(std::make_unique<financial_report>());

        SUBCASE("From Accounting Department") {
            sender.send("some text", origin::accounting_department, "Mary Jones");

            CHECK(sender.get_last_sent() ==
                  "Finantial report: some text; Encrypted signature: Encrypted(Mary Jones)");
        }

        SUBCASE("Not from Accounting Department") {
            CHECK_THROWS(sender.send("some text", origin::hr, "Mary Jones"));
        }
    }
}
