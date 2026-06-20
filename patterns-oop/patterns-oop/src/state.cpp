#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>

namespace {
    class coin
    {
    public:
        explicit coin(unsigned val)
            : m_value(val)
        {}

        unsigned get_value() const
        {
            return m_value;
        }

    private:
        const unsigned m_value;
    };

    class product
    {
    public:
        explicit product(unsigned val)
            : m_value(val)
        {}

        unsigned get_value() const
        {
            return m_value;
        }

    private:
        const unsigned m_value;
    };

    class ivending_machine_state
    {
    public:
        virtual ~ivending_machine_state() = default;

        virtual void insert_coin(coin c) = 0;
        virtual coin return_coin() = 0;
        virtual product press_buy() = 0;
    };

    class vending_machine_state_context
    {
    public:
        virtual ~vending_machine_state_context() = default;

        virtual void set_state(std::unique_ptr<ivending_machine_state> state) = 0;
    };

    class inserted_coin_state
        : public ivending_machine_state
    {
    public:
        inserted_coin_state(coin c,
                            vending_machine_state_context *context);

        void insert_coin(coin c) override;
        coin return_coin() override;
        product press_buy() override;

    private:
        coin m_coin;
        vending_machine_state_context *m_context;
    };

    class no_coin_state
        : public ivending_machine_state
    {
    public:
        explicit no_coin_state(vending_machine_state_context *context);

        void insert_coin(coin c) override;
        coin return_coin() override;
        product press_buy() override;

    private:
        vending_machine_state_context *m_context;
    };

    inserted_coin_state::inserted_coin_state(coin c,
                                             vending_machine_state_context *context)
        : m_coin(c)
        , m_context(context)
    {}

    void inserted_coin_state::insert_coin(coin)
    {
        throw std::runtime_error("coin already inserted");
    }

    coin inserted_coin_state::return_coin()
    {
        auto ans = m_coin;
        m_context->set_state(std::make_unique<no_coin_state>(m_context));
        return ans;
    }

    product inserted_coin_state::press_buy()
    {
        product ans(m_coin.get_value());
        m_context->set_state(std::make_unique<no_coin_state>(m_context));

        return ans;
    }

    no_coin_state::no_coin_state(vending_machine_state_context *context)
        : m_context(context)
    {}

    void no_coin_state::insert_coin(coin c)
    {
        m_context->set_state(std::make_unique<inserted_coin_state>(c, m_context));
    }

    coin no_coin_state::return_coin()
    {
        throw std::runtime_error("no coin");
    }

    product no_coin_state::press_buy()
    {
        throw std::runtime_error("no coin");
    }

    class vending_machine
        : private vending_machine_state_context
    {
    public:
        vending_machine()
            : m_state(std::make_unique<no_coin_state>(static_cast<vending_machine_state_context *>(this)))
        {}

        void insert_coin(coin c)
        {
            m_state->insert_coin(c);
        }

        coin return_coin()
        {
            return m_state->return_coin();
        }

        product press_buy()
        {
            return m_state->press_buy();
        }

    private:
        void set_state(std::unique_ptr<ivending_machine_state> state) override
        {
            m_state = std::move(state);
        }

    private:
        std::unique_ptr<ivending_machine_state> m_state;
    };
}

TEST_SUITE("State") {
    TEST_CASE("Vending Machine") {
        vending_machine sut;

        SUBCASE("Buy product") {
            sut.insert_coin(coin(5));
            auto product = sut.press_buy();

            CHECK(product.get_value() == 5);
        }

        SUBCASE("Cannot buy twice") {
            sut.insert_coin(coin(5));
            sut.press_buy();
            CHECK_THROWS(sut.press_buy());
        }

        SUBCASE("Cannot buy without coin") {
            CHECK_THROWS(sut.press_buy());
        }

        SUBCASE("Buy again after insert coin") {
            sut.insert_coin(coin(5));
            sut.press_buy();
            sut.insert_coin(coin(10));
            auto product = sut.press_buy();
            CHECK(product.get_value() == 10);
        }

        SUBCASE("Cannot buy after return coin") {
            sut.insert_coin(coin(5));
            sut.return_coin();
            CHECK_THROWS(sut.press_buy());
        }

        SUBCASE("Cannot return coin if no coin") {
            CHECK_THROWS(sut.return_coin());
        }
    }
}
