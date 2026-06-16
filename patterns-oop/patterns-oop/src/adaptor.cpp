#include <doctest/doctest.h>
#include <memory>

namespace {
    unsigned s_environment_temp_F = 100;

    class vendor_temp_sensor
    {
    public:
        unsigned get_temp_F() const
        {
            return s_environment_temp_F;
        }
    };

    class itemp_sensor_adaptor
    {
    public:
        virtual ~itemp_sensor_adaptor() = default;

        virtual unsigned get_temp_celsius() const = 0;
    };

    class temp_sensor_adaptor
        : public itemp_sensor_adaptor
    {
    public:
        unsigned get_temp_celsius() const override
        {
            auto f = m_vendor_temp_sensor.get_temp_F();
            return (f - 32) * 5 / 9;
        }

    private:
        vendor_temp_sensor m_vendor_temp_sensor;
    };

    class monitoring_system
    {
    public:
        monitoring_system(std::unique_ptr<itemp_sensor_adaptor> temp_sensor)
            : m_temp_sensor(std::move(temp_sensor))
        {}

        bool is_temp_ok() const
        {
            return m_temp_sensor->get_temp_celsius() <= 100;
        }

    private:
        std::unique_ptr<itemp_sensor_adaptor> m_temp_sensor;
    };
}

TEST_SUITE("Adaptor") {
    TEST_CASE("Monitoring system") {
        monitoring_system sut(std::make_unique<temp_sensor_adaptor>());

        SUBCASE("Temperature is ok") {
            s_environment_temp_F = 212; // 100C

            CHECK(sut.is_temp_ok());
        }

        SUBCASE("Temperature is not ok") {
            s_environment_temp_F = 214; // > 100C

            CHECK(!sut.is_temp_ok());
        }
    }
}
