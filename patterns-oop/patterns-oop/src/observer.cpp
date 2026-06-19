#include <doctest/doctest.h>

#include <list>
#include <memory>

namespace {
    class icurrency_observer
    {
    public:
        virtual ~icurrency_observer() = default;

        virtual void on_value_change(unsigned new_val) = 0;
    };

    class currency
    {
    public:
        void set_value(unsigned val)
        {
            m_value = val;
            for(auto observer : m_observers) {
                observer->on_value_change(val);
            }
        }

        unsigned get_value() const
        {
            return m_value;
        }

        void add_observer(icurrency_observer *observer)
        {
            m_observers.push_back(observer);
        }

        void remove_observer(icurrency_observer *observer)
        {
            m_observers.remove_if([observer](auto el) { return el == observer; });
        }

        size_t get_observers_count() const
        {
            return m_observers.size();
        }

    private:
        unsigned m_value = 100;

        std::list<icurrency_observer *> m_observers;
    };

    class currency_exchange_table
        : private icurrency_observer
    {
    public:
        explicit currency_exchange_table(std::shared_ptr<currency> currency)
            : m_currency(std::move(currency))
        {
            m_show_value = m_currency->get_value();
            m_currency->add_observer(this);
        }

        ~currency_exchange_table()
        {
            m_currency->remove_observer(this);
        }

        unsigned get_show_value() const
        {
            return m_show_value;
        }

    private:
        void on_value_change(unsigned new_val) override
        {
            m_show_value = new_val;
        }

    private:
        std::shared_ptr<currency> m_currency;

        unsigned m_show_value;
    };
}

TEST_SUITE("Observer") {
    TEST_CASE("Currency exchange table") {
        auto currency0 = std::make_shared<currency>();
        CHECK(currency0->get_observers_count() == 0);

        auto table = std::make_unique<currency_exchange_table>(currency0);
        CHECK(currency0->get_observers_count() == 1);
        CHECK(table->get_show_value() == currency0->get_value());

        SUBCASE("Update table on currency change") {
            currency0->set_value(90);
            CHECK(table->get_show_value() == 90);
        }

        SUBCASE("Table unsubscribes itself on destruction") {
            table.reset();
            CHECK(currency0->get_observers_count() == 0);
        }
    }
}
