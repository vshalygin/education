#include <doctest/doctest.h>
#include <string>
#include <memory>
#include <map>

namespace {
    class vacation_checker
    {
    public:
        vacation_checker() = default;
        vacation_checker(std::unique_ptr<vacation_checker> next_checker)
            : m_next_checker(std::move(next_checker))
        {}

        virtual ~vacation_checker() = default;

        virtual bool check(const std::string &employee, unsigned start, unsigned end) const = 0;

    protected:
        bool check_next(const std::string &employee, unsigned start, unsigned end) const
        {
            return m_next_checker ? m_next_checker->check(employee, start, end) : true;
        }

    private:
        std::unique_ptr<vacation_checker> m_next_checker;
    };

    class valid_date_checker
        : public vacation_checker
    {
    public:
        valid_date_checker() = default;

        valid_date_checker(std::unique_ptr<vacation_checker> next_checker)
            : vacation_checker(std::move(next_checker))
        {}

        bool check(const std::string &employee, unsigned start, unsigned end) const override
        {
            if(start >= end) {
                return false;
            }

            return check_next(employee, start, end);
        }
    };

    class employee_vacation_days_checker
        : public vacation_checker
    {
    public:
        employee_vacation_days_checker() = default;

        employee_vacation_days_checker(std::unique_ptr<vacation_checker> next_checker)
            : vacation_checker(std::move(next_checker))
        {}

        bool check(const std::string &employee, unsigned start, unsigned end) const override
        {
            auto it = m_employees_vacation_days.find(employee);
            if(it == m_employees_vacation_days.end()) {
                return false;
            }
            if(it->second < (end - start + 1)) {
                return false;
            }
            return check_next(employee, start, end);
        }

    private:
        std::map<std::string, unsigned> m_employees_vacation_days{ {"Jone Smith", 10},
                                                                   {"Jane Cooper", 50} };
    };

    class vacation_length_checker
        : public vacation_checker
    {
    public:
        vacation_length_checker() = default;

        vacation_length_checker(std::unique_ptr<vacation_checker> next_checker)
            : vacation_checker(std::move(next_checker))
        {}

        bool check(const std::string &employee, unsigned start, unsigned end) const override
        {
            const unsigned max_length = 21;
            if((end - start + 1) > max_length) {
                return false;
            }
            return check_next(employee, start, end);
        }
    };
}

TEST_SUITE("Chain of responsibility") {
    TEST_CASE("Vacation checker") {
        auto checker1 = std::make_unique<vacation_length_checker>();
        auto checker2 = std::make_unique<employee_vacation_days_checker>(std::move(checker1));
        auto checker3 = std::make_unique<valid_date_checker>(std::move(checker2));

        SUBCASE("Vacation is allowed") {
            CHECK(checker3->check("Jone Smith", 3, 7));
        }

        SUBCASE("Invalid period of time") {
            CHECK(!checker3->check("Jone Smith", 7, 3));
            CHECK(!checker3->check("Jone Smith", 3, 3));
        }

        SUBCASE("Exceed requested vacation days for worker") {
            CHECK(!checker3->check("Jone Smith", 3, 15));
        }

        SUBCASE("Unknown worker for vacation days check") {
            CHECK(!checker3->check("Jone Doe", 3, 15));
        }

        SUBCASE("Requested days above allowed limits") {
            CHECK(!checker3->check("Jane Cooper", 3, 25));
        }
    }
}
