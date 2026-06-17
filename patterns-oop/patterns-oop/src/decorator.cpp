#include <doctest/doctest.h>
#include <string>
#include <climits>
#include <memory>

namespace {
    std::string s_log_buffer;

    void log(const std::string &message)
    {
        if(s_log_buffer.empty()) {
            s_log_buffer = message;
        } else {
            s_log_buffer += "; " + message;
        }
    }

    std::string encrypt(const std::string &data)
    {
        std::string ans;
        for(int ch : data) {
            if(ch == CHAR_MAX) {
                ch = CHAR_MIN;
            } else {
                ++ch;
            }
            ans.push_back(ch);
        }

        return ans;
    }

    std::string decrypt(const std::string &data)
    {
        std::string ans;
        for(int ch : data) {
            if(ch == CHAR_MIN) {
                ch = CHAR_MAX;
            } else {
                --ch;
            }
            ans.push_back(ch);
        }

        return ans;
    }

    class idata_source
    {
    public:
        virtual ~idata_source() = default;

        virtual void write(const std::string &data) = 0;
        virtual std::string read() const = 0;
    };

    class data_source
        : public idata_source
    {
    public:
        void write(const std::string &data) override
        {
            m_data = data;
        }

        std::string read() const override
        {
            return m_data;
        }

    private:
        std::string m_data;
    };

    class data_source_decorator
        : public idata_source
    {
    public:
        explicit data_source_decorator(std::unique_ptr<idata_source> source)
            : m_data_source(std::move(source))
        {}

        void write(const std::string &data) override
        {
            m_data_source->write(data);
        }

        std::string read() const override
        {
            return m_data_source->read();
        }

    private:
        std::unique_ptr<idata_source> m_data_source;
    };

    class data_source_encrypted_decorator
        : public data_source_decorator
    {
    public:
        using data_source_decorator::data_source_decorator;

        void write(const std::string &data) override
        {
            data_source_decorator::write(encrypt(data));
        }

        std::string read() const override
        {
            return decrypt(data_source_decorator::read());
        }
    };

    class data_source_logged_decorator
        : public data_source_decorator
    {
    public:
        using data_source_decorator::data_source_decorator;

        void write(const std::string &data) override
        {
            data_source_decorator::write(data);
            log("write: " + data);
        }

        std::string read() const override
        {
            auto data = data_source_decorator::read();
            log("read: " + data);
            return data;
        }
    };
}

TEST_SUITE("Decorator") {
    TEST_CASE("Data source") {
        s_log_buffer.clear();

        auto source = std::make_unique<data_source>();
        auto source_ptr = source.get();
        auto decorator = std::make_unique<data_source_decorator>(std::move(source));
        auto encrypted_decorator =
            std::make_unique<data_source_encrypted_decorator>(std::move(decorator));
        auto logged_encrypted_decorator =
            std::make_unique<data_source_logged_decorator>(std::move(encrypted_decorator));

        logged_encrypted_decorator->write("abc");
        CHECK(source_ptr->read() == "bcd");
        CHECK(s_log_buffer == "write: abc");

        auto data = logged_encrypted_decorator->read();
        CHECK(data == "abc");
        CHECK(s_log_buffer == "write: abc; read: abc");
    }
}
