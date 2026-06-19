#include <doctest/doctest.h>

#include <string>

namespace {
    class iview_event_handler
    {
    public:
        virtual ~iview_event_handler() = default;

        virtual void on_field_change() = 0;
    };

    class field
    {
    public:
        explicit field(iview_event_handler *event_handler)
            : m_event_handler(event_handler)
        {}

        void set(const std::string &val)
        {
            m_value = val;
            m_event_handler->on_field_change();
        }

        bool is_empty() const
        {
            return m_value.empty();
        }

    private:
        std::string m_value;

        iview_event_handler *m_event_handler;
    };

    class login_field
        : public field
    {
    public:
        using field::field;
    };

    class password_field
        : public field
    {
    public:
        using field::field;
    };

    class check_button
    {
    public:
        bool is_active() const
        {
            return m_is_active;
        }

        void set_active()
        {
            m_is_active = true;
        }

        void set_inactive()
        {
            m_is_active = false;
        }

        void press()
        {
            if(m_is_active) {
                /*do something*/
            }
        }

    private:
        bool m_is_active = false;
    };

    class view
        : private iview_event_handler
    {
    public:
        view()
            : m_login_field(this)
            , m_password_field(this)
        {}

        void on_field_change() override
        {
            if(!m_login_field.is_empty() && !m_password_field.is_empty()) {
                m_check_button.set_active();
            } else {
                m_check_button.set_inactive();
            }
        }

        void set_login(const std::string &login)
        {
            m_login_field.set(login);
        }

        void set_password(const std::string &password)
        {
            m_password_field.set(password);
        }

        void press_check_button()
        {
            m_check_button.press();
        }

        bool is_check_button_active() const
        {
            return m_check_button.is_active();
        }

    private:
        login_field m_login_field;
        password_field m_password_field;
        check_button m_check_button;
    };
}

TEST_SUITE("Mediator") {
    TEST_CASE("Page view") {
        view sut;
        CHECK(!sut.is_check_button_active());

        sut.set_login("login");
        CHECK(!sut.is_check_button_active());

        sut.set_password("password");
        CHECK(sut.is_check_button_active());

        sut.set_login("");
        CHECK(!sut.is_check_button_active());
    }
}
