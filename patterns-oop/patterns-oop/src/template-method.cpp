#include <doctest/doctest.h>

#include <string>

namespace {
    struct drink
    {
        bool is_hot = false;
        bool is_mixed = false;
        std::string ingredient;
    };

    class hot_drink_system
    {
    public:
        virtual ~hot_drink_system() = default;

        drink prepare() const
        {
            drink ans;
            heat(ans);
            set_ingredient(ans);
            mix(ans);

            return ans;
        }

    private:
        void heat(drink &d) const
        {
            d.is_hot = true;
        }

        virtual void set_ingredient(drink &) const = 0;

        void mix(drink &d) const
        {
            d.is_mixed = true;
        }
    };

    class coffee_system
        : public hot_drink_system
    {
    private:
        void set_ingredient(drink &d) const
        {
            d.ingredient = "coffee";
        }
    };

    class tea_system
        : public hot_drink_system
    {
    private:
        void set_ingredient(drink &d) const
        {
            d.ingredient = "tea";
        }
    };
}

TEST_SUITE("Template method") {
    TEST_CASE("Coffee system") {
        coffee_system sut;
        auto drink = sut.prepare();
        CHECK(drink.is_hot);
        CHECK(drink.is_mixed);
        CHECK(drink.ingredient == "coffee");
    }

    TEST_CASE("Tea system") {
        tea_system sut;
        auto drink = sut.prepare();
        CHECK(drink.is_hot);
        CHECK(drink.is_mixed);
        CHECK(drink.ingredient == "tea");
    }
}
