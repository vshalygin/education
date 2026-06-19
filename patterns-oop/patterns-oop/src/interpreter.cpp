#include <doctest/doctest.h>

#include <memory>

namespace {
    class ibool_expression
    {
    public:
        virtual ~ibool_expression() = default;

        virtual bool evaluate() const = 0;
    };

    class literal
        : public ibool_expression
    {
    public:
        explicit literal(bool val)
            : m_val(val)
        {}

        bool evaluate() const override
        {
            return m_val;
        }

    private:
        bool m_val;
    };

    class and_expression
        : public ibool_expression
    {
    public:
        and_expression(std::unique_ptr<ibool_expression> left,
                       std::unique_ptr<ibool_expression> right)
            : m_left(std::move(left))
            , m_right(std::move(right))
        {}

        bool evaluate() const override
        {
            return m_left->evaluate() && m_right->evaluate();
        }

    private:
        std::unique_ptr<ibool_expression> m_left;
        std::unique_ptr<ibool_expression> m_right;
    };

    class or_expression
        : public ibool_expression
    {
    public:
        or_expression(std::unique_ptr<ibool_expression> left,
                      std::unique_ptr<ibool_expression> right)
            : m_left(std::move(left))
            , m_right(std::move(right))
        {}

        bool evaluate() const override
        {
            return m_left->evaluate() || m_right->evaluate();
        }

    private:
        std::unique_ptr<ibool_expression> m_left;
        std::unique_ptr<ibool_expression> m_right;
    };
}

TEST_SUITE("Interpreter") {
    TEST_CASE("Bool expression") {
        SUBCASE("Single literal") {
            literal expr(true);
            CHECK(expr.evaluate());
        }

        SUBCASE("And expression returns false") {
            auto expr1 = std::make_unique<literal>(true);
            auto expr2 = std::make_unique<literal>(false);
            and_expression expr(std::move(expr1), std::move(expr2));
            CHECK(!expr.evaluate());
        }

        SUBCASE("And expression returns true") {
            auto expr1 = std::make_unique<literal>(true);
            auto expr2 = std::make_unique<literal>(true);
            and_expression expr(std::move(expr1), std::move(expr2));
            CHECK(expr.evaluate());
        }

        SUBCASE("Or expression returns false") {
            auto expr1 = std::make_unique<literal>(false);
            auto expr2 = std::make_unique<literal>(false);
            or_expression expr(std::move(expr1), std::move(expr2));
            CHECK(!expr.evaluate());
        }

        SUBCASE("Or expression returns true") {
            auto expr1 = std::make_unique<literal>(true);
            auto expr2 = std::make_unique<literal>(false);
            or_expression expr(std::move(expr1), std::move(expr2));
            CHECK(expr.evaluate());
        }

        SUBCASE("Complex expression") {
            //true && (true || false) => true 
            auto lit1 = std::make_unique<literal>(true);
            auto lit2 = std::make_unique<literal>(true);
            auto lit3 = std::make_unique<literal>(false);
            auto or_expr = std::make_unique<or_expression>(std::move(lit2), std::move(lit3));
            and_expression and_expr(std::move(lit1), std::move(or_expr));
            CHECK(and_expr.evaluate());
        }
    }
}
