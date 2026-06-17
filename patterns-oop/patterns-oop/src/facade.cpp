#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <regex>
#include <map>

namespace {
    std::map<std::string, std::vector<std::string>> s_remote_mail_storage;

    class mail_server
    {
    public:
        void connect()
        {
            m_remote_mail_storage = &s_remote_mail_storage;
        }

        void disconnect()
        {
            m_remote_mail_storage = nullptr;
        }

        void send(const std::string &addres, const std::string &letter)
        {
            if(!m_remote_mail_storage) {
                throw std::runtime_error("mail server is not connected");
            }
            (*m_remote_mail_storage)[addres].push_back(letter);
        }

    private:
        std::map<std::string, std::vector<std::string>> *m_remote_mail_storage = nullptr;
    };

    class email_validator
    {
    public:
        bool is_email_valid(const std::string &email) const
        {
            const std::regex email_pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");

            return std::regex_match(email, email_pattern);
        }
    };

    class email_formatter
    {
    public:
        std::string format(const std::string &author, const std::string &content) const
        {
            return "From: " + author + "; " + content;
        }
    };

    class email_sender
    {
    public:
        void send(const std::string &email, const std::string &author, const std::string &content)
        {
            if(!m_email_validator.is_email_valid(email)) {
                throw std::runtime_error("invalid email");
            }

            m_mail_server.connect();
            m_mail_server.send(email, m_email_formatter.format(author, content));
            m_mail_server.disconnect();
        }

    private:
        mail_server m_mail_server;
        email_validator m_email_validator;
        email_formatter m_email_formatter;
    };
}

TEST_SUITE("Facade") {
    TEST_CASE("Mail sender") {
        email_sender sut;

        SUBCASE("Send") {
            sut.send("example@mail.com", "author", "message");
            REQUIRE(s_remote_mail_storage.count("example@mail.com"));
            REQUIRE(s_remote_mail_storage["example@mail.com"].size() == 1);
            CHECK(s_remote_mail_storage["example@mail.com"][0] == "From: author; message");
        }

        SUBCASE("Invalid email") {
            CHECK_THROWS(sut.send("example", "author", "message"));
        }
    }
}
