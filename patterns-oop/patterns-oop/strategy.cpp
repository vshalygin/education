#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>

namespace {
    class idelivery_strategy
    {
    public:
        virtual ~idelivery_strategy() = default;

        virtual unsigned calc_price(unsigned mass) const = 0;
    };

    class normal_delivery
        : public idelivery_strategy
    {
    public:
        unsigned calc_price(unsigned mass) const override
        {
            return mass * 100;
        }
    };

    class express_delivery
        : public idelivery_strategy
    {
    public:
        unsigned calc_price(unsigned mass) const override
        {
            return mass * 200;
        }
    };

    class delivery_service
    {
    public:
        bool deliver(unsigned mass, unsigned payment)
        {
            if(!m_strategy) {
                throw std::logic_error("no strategy");
            }

            auto price = m_strategy->calc_price(mass);
            if(price > payment) {
                return false;
            }
            /*do delivery*/
            return true;
        }

        void set_strategy(std::unique_ptr<idelivery_strategy> strategy)
        {
            m_strategy = std::move(strategy);
        }

    private:
        std::unique_ptr<idelivery_strategy> m_strategy;
    };
}

TEST_SUITE("Strategy") {
    TEST_CASE("Delivery service") {
        delivery_service sut;

        SUBCASE("Do normal delivery") {
            sut.set_strategy(std::make_unique<normal_delivery>());
            CHECK(sut.deliver(1, 100));
            CHECK(sut.deliver(2, 200));
            CHECK(!sut.deliver(2, 199));
        }

        SUBCASE("Do express delivery") {
            sut.set_strategy(std::make_unique<express_delivery>());
            CHECK(sut.deliver(1, 200));
            CHECK(sut.deliver(2, 400));
            CHECK(!sut.deliver(2, 399));
        }
    }
}
