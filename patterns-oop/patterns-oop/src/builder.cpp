#include <doctest/doctest.h>
#include <string>
#include <algorithm>

namespace {
    enum class character_class
    {
        peasant,
        warrior,
        mage,
        archer
    };

    enum class weapon
    {
        no,
        sword,
        bow,
        spear
    };

    enum class armor
    {
        no,
        light,
        heavy
    };

    class character_builder;

    class character
    {
        friend class character_builder;

    public:
        unsigned give_damage()
        {
            switch(m_weapon) {
                case weapon::sword:
                    return 30;
                case weapon::bow:
                    return 20;
                case weapon::spear:
                    return 15;
                case weapon::no:
                    return 5;
            }
        }

        unsigned give_magic_damage(unsigned mana)
        {
            mana = std::min(m_mana, mana);
            m_mana -= mana;
            return mana;
        }

        void take_damage(unsigned damage)
        {
            switch(m_armor) {
                case armor::light:
                    damage = damage * 3 / 4;
                    break;
                case armor::heavy:
                    damage /= 2;
                    break;
            }

            m_health = (damage > m_health) ? 0 : (m_health - damage);
        }

        void take_magic_damage(unsigned damage)
        {
            m_health = (damage > m_health) ? 0 : (m_health - damage);
        }

        bool is_alive() const
        {
            return m_health != 0;
        }

    private:
        std::string m_name;
        character_class m_class = character_class::peasant;
        unsigned m_health = 0;
        unsigned m_mana = 0;
        weapon m_weapon = weapon::no;
        armor m_armor = armor::no;
    };

    class character_builder
    {
    public:
        void set_name(const std::string &name)
        {
            m_current_character.m_name = name;
        }

        void set_class(const character_class c)
        {
            m_current_character.m_class = c;
        }

        void set_health(unsigned health)
        {
            m_current_character.m_health = health;
        }

        void set_mana(unsigned mana)
        {
            if(m_current_character.m_class == character_class::mage) {
                m_current_character.m_mana = mana;
            }
        }

        void set_weapon(weapon weapon)
        {
            if(weapon == weapon::bow && m_current_character.m_class != character_class::archer) {
                return;
            }
            m_current_character.m_weapon = weapon;
        }

        void set_armor(armor armor)
        {
            m_current_character.m_armor = armor;
        }

        character get_character()
        {
            auto ans = std::move(m_current_character);
            m_current_character = character();
            return ans;
        }

    private:
        character m_current_character;
    };

    character create_warrior(const std::string &name)
    {
        character_builder builder;
        builder.set_class(character_class::warrior);
        builder.set_weapon(weapon::sword);
        builder.set_armor(armor::heavy);
        builder.set_name(name);
        builder.set_health(200);

        return builder.get_character();
    }

    character create_archer(const std::string &name)
    {
        character_builder builder;
        builder.set_class(character_class::archer);
        builder.set_weapon(weapon::bow);
        builder.set_armor(armor::light);
        builder.set_name(name);
        builder.set_health(150);

        return builder.get_character();
    }

    character create_mage(const std::string &name)
    {
        character_builder builder;
        builder.set_class(character_class::mage);
        builder.set_weapon(weapon::no);
        builder.set_armor(armor::no);
        builder.set_name(name);
        builder.set_health(100);
        builder.set_mana(100);

        return builder.get_character();
    }
}

TEST_SUITE("Builder") {
    TEST_CASE("Warrior") {
        auto character = create_warrior("Warrior");

        CHECK(character.is_alive());
        CHECK(character.give_damage() == 30);
        CHECK(character.give_magic_damage(100) == 0);

        SUBCASE("Alive after 399 damage") {
            character.take_damage(399);
            CHECK(character.is_alive());
        }

        SUBCASE("Dead after 400 damage") {
            character.take_damage(400);
            CHECK(!character.is_alive());
        }

        SUBCASE("Alive after 199 magic damage") {
            character.take_magic_damage(199);
            CHECK(character.is_alive());
        }

        SUBCASE("Dead after 200 magic damage") {
            character.take_magic_damage(200);
            CHECK(!character.is_alive());
        }
    }

    TEST_CASE("Mage") {
        auto character = create_mage("Mage");

        CHECK(character.is_alive());
        CHECK(character.give_damage() == 5);
        CHECK(character.give_magic_damage(100) == 100);

        SUBCASE("Alive after 99 damage") {
            character.take_damage(99);
            CHECK(character.is_alive());
        }

        SUBCASE("Dead after 100 damage") {
            character.take_damage(100);
            CHECK(!character.is_alive());
        }

        SUBCASE("Alive after 99 magic damage") {
            character.take_magic_damage(99);
            CHECK(character.is_alive());
        }

        SUBCASE("Dead after 100 magic damage") {
            character.take_magic_damage(100);
            CHECK(!character.is_alive());
        }
    }

    TEST_CASE("Archer") {
        auto character = create_archer("Archer");

        CHECK(character.is_alive());
        CHECK(character.give_damage() == 20);
        CHECK(character.give_magic_damage(100) == 0);

        SUBCASE("Alive after 199 damage") {
            character.take_damage(199);
            CHECK(character.is_alive());
        }

        SUBCASE("Dead after 200 damage") {
            character.take_damage(200);
            CHECK(!character.is_alive());
        }

        SUBCASE("Alive after 149 magic damage") {
            character.take_magic_damage(149);
            CHECK(character.is_alive());
        }

        SUBCASE("Dead after 150 magic damage") {
            character.take_magic_damage(150);
            CHECK(!character.is_alive());
        }
    }
}
