#include <doctest/doctest.h>
#include <memory>
#include <string>

namespace {
    class ichair;
    class isofa;
    class icoffee_table;

    class ifactory
    {
    public:
        virtual ~ifactory() = default;

        virtual std::unique_ptr<ichair> create_chair() = 0;
        virtual std::unique_ptr<isofa> create_sofa() = 0;
        virtual std::unique_ptr<icoffee_table> create_coffee_table() = 0;
    };

    class ichair
    {
    public:
        virtual ~ichair() = default;

        virtual std::string get_style() const = 0;
    };

    class isofa
    {
    public:
        virtual ~isofa() = default;

        virtual std::string get_style() const = 0;
    };

    class icoffee_table
    {
    public:
        virtual ~icoffee_table() = default;

        virtual std::string get_style() const = 0;
    };
    
    class classic_chair
        : public ichair
    {
    public:
        std::string get_style() const override
        {
            return "classic";
        }
    };

    class modern_chair
        : public ichair
    {
    public:
        std::string get_style() const override
        {
            return "modern";
        }
    };

    class classic_sofa
        : public isofa
    {
    public:
        std::string get_style() const override
        {
            return "classic";
        }
    };

    class modern_sofa
        : public isofa
    {
    public:
        std::string get_style() const override
        {
            return "modern";
        }
    };

    class classic_coffee_table
        : public icoffee_table
    {
    public:
        std::string get_style() const override
        {
            return "classic";
        }
    };

    class modern_coffee_table
        : public icoffee_table
    {
    public:
        std::string get_style() const override
        {
            return "modern";
        }
    };

    class classic_style_factory
        : public ifactory
    {
    public:
        std::unique_ptr<ichair> create_chair() override
        {
            return std::make_unique<classic_chair>();
        }

        std::unique_ptr<isofa> create_sofa() override
        {
            return std::make_unique<classic_sofa>();
        }

        std::unique_ptr<icoffee_table> create_coffee_table() override
        {
            return std::make_unique<classic_coffee_table>();
        }
    };

    class modern_style_factory
        : public ifactory
    {
    public:
        std::unique_ptr<ichair> create_chair() override
        {
            return std::make_unique<modern_chair>();
        }

        std::unique_ptr<isofa> create_sofa() override
        {
            return std::make_unique<modern_sofa>();
        }

        std::unique_ptr<icoffee_table> create_coffee_table() override
        {
            return std::make_unique<modern_coffee_table>();
        }
    };

    class room
    {
    public:
        explicit room(ifactory *factory)
            : m_chair(factory->create_chair())
            , m_sofa(factory->create_sofa())
            , m_coffee_table(factory->create_coffee_table())
        {}

        bool is_style_coherent() const
        {
            return m_chair->get_style() == m_sofa->get_style() &&
                   m_sofa->get_style() == m_coffee_table->get_style();
        }

        std::string get_style() const
        {
            if(!is_style_coherent()) {
                return "no style";
            }

            return m_chair->get_style();
        }

    private:
        std::unique_ptr<ichair> m_chair;
        std::unique_ptr<isofa> m_sofa;
        std::unique_ptr<icoffee_table> m_coffee_table;
    };
}

TEST_SUITE("Abstract factory") {
    TEST_CASE("Classic style") {
        classic_style_factory factory;

        room room(&factory);

        CHECK(room.is_style_coherent());
        CHECK(room.get_style() == "classic");
    }

    TEST_CASE("Modern style") {
        modern_style_factory factory;

        room room(&factory);

        CHECK(room.is_style_coherent());
        CHECK(room.get_style() == "modern");
    }
}
