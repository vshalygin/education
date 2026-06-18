#include <doctest/doctest.h>
#include <memory>

namespace {
    class lamp
    {
    public:
        void turn_on()
        {
            m_is_working = true;
        }

        void turn_off()
        {
            m_is_working = false;
        }

        bool is_working() const
        {
            return m_is_working;
        }

    private:
        bool m_is_working = false;
    };

    class icommand
    {
    public:
        virtual ~icommand() = default;

        virtual void execute() = 0;
    };

    class turn_on_command
        : public icommand
    {
    public:
        turn_on_command(lamp &l)
            : m_lamp(l)
        {}

        void execute() override
        {
            m_lamp.turn_on();
        }

    private:
        lamp &m_lamp;
    };

    class turn_off_command
        : public icommand
    {
    public:
        turn_off_command(lamp &l)
            : m_lamp(l)
        {
        }

        void execute() override
        {
            m_lamp.turn_off();
        }

    private:
        lamp &m_lamp;
    };

    class controller
    {
    public:
        controller(std::unique_ptr<icommand> on, std::unique_ptr<icommand> off)
            : m_on(std::move(on))
            , m_off(std::move(off))
        {}

        void press_on()
        {
            m_on->execute();
        }

        void press_off()
        {
            m_off->execute();
        }

    private:
        std::unique_ptr<icommand> m_on;
        std::unique_ptr<icommand> m_off;
    };
}

TEST_SUITE("Command") {
    TEST_CASE("Lamp controller") {
        lamp lamp;
        controller controller(std::make_unique<turn_on_command>(lamp),
                              std::make_unique<turn_off_command>(lamp));
        CHECK(!lamp.is_working());

        SUBCASE("Turn on") {
            controller.press_on();
            CHECK(lamp.is_working());

            SUBCASE("Turn off") {
                controller.press_off();
                CHECK(!lamp.is_working());
            }
        }
    }
}
