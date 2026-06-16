#include <doctest/doctest.h>
#include <algorithm>
#include <string>
#include <stdexcept>

namespace {
    class device
    {
    protected:
        device(unsigned init_volume, unsigned max_volume)
            : m_volume(init_volume)
            , m_max_volume(max_volume)
        {}

    public:
        virtual ~device() = default;

        void turn_on()
        {
            m_is_working = true;
        }

        void turn_off()
        {
            m_is_working = false;
        }

        void set_volume(unsigned volume)
        {
            if(m_is_working) {
                m_volume = std::min(volume, m_max_volume);
            }
        }

        bool is_working() const
        {
            return m_is_working;
        }

        unsigned get_current_volume() const
        {
            if(!m_is_working) {
                throw std::runtime_error("device is turned off");
            }
            return m_volume;
        }

        virtual void set_channel(unsigned number) = 0;

    protected:
        const unsigned m_max_volume;
        bool m_is_working = false;
        unsigned m_volume;
    };

    class tv
        : public device
    {
    public:
        tv()
            : device(50, 100)
        {}

        void set_channel(unsigned number) override
        {
            if(!is_working()) {
                return;
            }

            if(number == 0) {
                m_channel_name = "cnn";
            } else if (number == 1) {
                m_channel_name = "fox";
            } else {
                m_channel_name = "unknown";
            }
        }

        std::string get_current_channel() const
        {
            return m_channel_name;
        }

    private:
        std::string m_channel_name;
    };

    class radio
        : public device
    {
    public:
        radio()
            : device(25, 50)
        {}

        void set_channel(unsigned number) override
        {
            if(!is_working()) {
                return;
            }

            if(number == 0) {
                m_station_name = "classic music";
            } else if(number == 1) {
                m_station_name = "rock music";
            } else {
                m_station_name = "unknown";
            }
        }

        std::string get_current_station() const
        {
            return m_station_name;
        }

    private:
        std::string m_station_name;
    };

    class controller
    {
    public:
        virtual ~controller() = default;

        controller(device *device)
            : m_device(device)
        {}

        void turn_on()
        {
            m_device->turn_on();
        }

        void turn_off()
        {
            m_device->turn_off();
        }

        virtual void set_volume(unsigned volume)
        {
            m_device->set_volume(volume);
        }

        void set_channel(unsigned number)
        {
            m_device->set_channel(number);
        }

    protected:
        device *m_device;
    };

    class extended_controller
        : public controller
    {
    public:
        extended_controller(device *device)
            : controller(device)
            , m_prev_volume(device->get_current_volume())
        {}

        void set_volume(unsigned volume) override
        {
            m_prev_volume = m_device->get_current_volume();
            controller::set_volume(volume);
        }

        void restore_volume()
        {
            m_device->set_volume(m_prev_volume);
        }

        void mute()
        {
            set_volume(0);
        }

    private:
        unsigned m_prev_volume;
    };
}

TEST_SUITE("Bridge") {
    TEST_CASE("TV") {
        tv tv;
        tv.turn_on();
        CHECK(tv.is_working());
        CHECK(tv.get_current_volume() == 50);

        SUBCASE("Controller") {
            controller controller(&tv);

            SUBCASE("Change channel") {
                controller.set_channel(1);
                CHECK(tv.get_current_channel() == "fox");
            }

            SUBCASE("Change volume") {
                controller.set_volume(70);
                CHECK(tv.get_current_volume() == 70);
            }

            SUBCASE("Change volume above 100") {
                controller.set_volume(101);
                CHECK(tv.get_current_volume() == 100);
            }

            SUBCASE("Turn off") {
                controller.turn_off();
                CHECK(!tv.is_working());
                CHECK_THROWS(tv.get_current_volume());
            }

            SUBCASE("Turn on") {
                controller.turn_off();
                controller.turn_on();
                CHECK(tv.is_working());
                CHECK(tv.get_current_volume() == 50);
            }
        }

        SUBCASE("Extended controller") {
            extended_controller controller(&tv);

            SUBCASE("Change channel") {
                controller.set_channel(1);
                CHECK(tv.get_current_channel() == "fox");
            }

            SUBCASE("Change volume") {
                controller.set_volume(70);
                CHECK(tv.get_current_volume() == 70);
            }

            SUBCASE("Change volume above 100") {
                controller.set_volume(101);
                CHECK(tv.get_current_volume() == 100);
            }

            SUBCASE("Turn off") {
                controller.turn_off();
                CHECK(!tv.is_working());
                CHECK_THROWS(tv.get_current_volume());
            }

            SUBCASE("Turn on") {
                controller.turn_off();
                controller.turn_on();
                CHECK(tv.is_working());
                CHECK(tv.get_current_volume() == 50);
            }

            SUBCASE("Mute") {
                controller.mute();
                CHECK(tv.get_current_volume() == 0);
            }

            SUBCASE("Restore previous volume") {
                controller.set_volume(34);
                controller.set_volume(30);
                controller.restore_volume();
                CHECK(tv.get_current_volume() == 34);
            }
        }
    }

    TEST_CASE("Radion") {
        radio radio;
        radio.turn_on();
        CHECK(radio.is_working());
        CHECK(radio.get_current_volume() == 25);

        SUBCASE("Controller") {
            controller controller(&radio);

            SUBCASE("Change channel") {
                controller.set_channel(1);
                CHECK(radio.get_current_station() == "rock music");
            }

            SUBCASE("Change volume") {
                controller.set_volume(45);
                CHECK(radio.get_current_volume() == 45);
            }

            SUBCASE("Change volume above 50") {
                controller.set_volume(51);
                CHECK(radio.get_current_volume() == 50);
            }

            SUBCASE("Turn off") {
                controller.turn_off();
                CHECK(!radio.is_working());
                CHECK_THROWS(radio.get_current_volume());
            }

            SUBCASE("Turn on") {
                controller.turn_off();
                controller.turn_on();
                CHECK(radio.is_working());
                CHECK(radio.get_current_volume() == 25);
            }
        }

        SUBCASE("Extended controller") {
            extended_controller controller(&radio);

            SUBCASE("Change channel") {
                controller.set_channel(1);
                CHECK(radio.get_current_station() == "rock music");
            }

            SUBCASE("Change volume") {
                controller.set_volume(49);
                CHECK(radio.get_current_volume() == 49);
            }

            SUBCASE("Change volume above 50") {
                controller.set_volume(51);
                CHECK(radio.get_current_volume() == 50);
            }

            SUBCASE("Turn off") {
                controller.turn_off();
                CHECK(!radio.is_working());
                CHECK_THROWS(radio.get_current_volume());
            }

            SUBCASE("Turn on") {
                controller.turn_off();
                controller.turn_on();
                CHECK(radio.is_working());
                CHECK(radio.get_current_volume() == 25);
            }

            SUBCASE("Mute") {
                controller.mute();
                CHECK(radio.get_current_volume() == 0);
            }

            SUBCASE("Restore previous volume") {
                controller.set_volume(34);
                controller.set_volume(30);
                controller.restore_volume();
                CHECK(radio.get_current_volume() == 34);
            }
        }
    }
}
