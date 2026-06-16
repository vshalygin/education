#include <doctest/doctest.h>
#include <memory>

namespace {
    class enemy
    {
    protected:
        enemy(unsigned health,
              unsigned damage)
            : m_health(health)
            , m_damage(damage)
        {}

    public:
        virtual ~enemy() = default;

        virtual void take_damage(unsigned damage) = 0;
        virtual unsigned do_damage() = 0;

        virtual void make_angry() = 0;

        bool is_alive() const
        {
            return m_health > 0;
        }

    protected:
        unsigned m_health;
        unsigned m_damage;
    };

    class wolf
        : public enemy
    {
    public:
        wolf()
            : enemy(100, 20)
        {}

        void take_damage(unsigned damage) override
        {
            damage /= 2;
            m_health = m_health > damage ? (m_health - damage) : 0;
        }

        unsigned do_damage() override
        {
            return m_damage;
        }

        void make_angry() override
        {
            m_damage *= 2;
        }
    };

    class skeleton
        : public enemy
    {
    public:
        skeleton()
            : enemy(80, 30)
        {}

        void take_damage(unsigned damage) override
        {
            m_health = m_health > damage ? (m_health - damage) : 0;
        }

        unsigned do_damage() override
        {
            return m_damage;
        }

        void make_angry() override
        {
            m_damage *= 3;
        }
    };

    class slug
        : public enemy
    {
    public:
        slug()
            : enemy(20, 5)
        {}

        void take_damage(unsigned damage) override
        {
            m_health = m_health > damage ? (m_health - damage) : 0;
        }

        unsigned do_damage() override
        {
            return m_damage;
        }

        void make_angry() override
        {}
    };

    class location
    {
    public:
        virtual ~location() = default;

        std::unique_ptr<enemy> spawn_enemy()
        {
            auto enemy = create_enemy();
            enemy->make_angry();

            return enemy;
        }

    private:
        virtual std::unique_ptr<enemy> create_enemy() = 0;
    };

    class forest
        : public location
    {
    private:
        std::unique_ptr<enemy> create_enemy() override
        {
            return std::make_unique<wolf>();
        }
    };

    class dungeon
        : public location
    {
    private:
        std::unique_ptr<enemy> create_enemy() override
        {
            return std::make_unique<skeleton>();
        }
    };

    class swamp
        : public location
    {
    private:
        std::unique_ptr<enemy> create_enemy() override
        {
            return std::make_unique<slug>();
        }
    };
}

TEST_SUITE("Factory Method") {
    TEST_CASE("Forrest") {
        forest forest;
        auto enemy = forest.spawn_enemy();

        CHECK(enemy->is_alive());
        CHECK(enemy->do_damage() == 40);

        SUBCASE("Alive after 199 damage") {
            enemy->take_damage(199);
            CHECK(enemy->is_alive());
        }

        SUBCASE("Dead after 200 damage") {
            enemy->take_damage(200);
            CHECK(!enemy->is_alive());
        }
    }

    TEST_CASE("Dungeon") {
        dungeon dungeon;
        auto enemy = dungeon.spawn_enemy();

        CHECK(enemy->is_alive());
        CHECK(enemy->do_damage() == 90);

        SUBCASE("Alive after 79 damage") {
            enemy->take_damage(79);
            CHECK(enemy->is_alive());
        }

        SUBCASE("Dead after 80 damage") {
            enemy->take_damage(80);
            CHECK(!enemy->is_alive());
        }
    }

    TEST_CASE("Swamp") {
        swamp swamp;
        auto enemy = swamp.spawn_enemy();

        CHECK(enemy->is_alive());
        CHECK(enemy->do_damage() == 5);

        SUBCASE("Alive after 19 damage") {
            enemy->take_damage(19);
            CHECK(enemy->is_alive());
        }

        SUBCASE("Dead after 20 damage") {
            enemy->take_damage(20);
            CHECK(!enemy->is_alive());
        }
    }
}
