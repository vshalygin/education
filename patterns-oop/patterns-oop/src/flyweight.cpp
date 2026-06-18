#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace {
    struct character_properties
    {
        character_properties(const std::string &f, unsigned s, const std::string &st)
            : font(f)
            , size(s)
            , style(st)
        {}

        std::string font;
        unsigned size;
        std::string style;
    };
    
    class character_properties_pool
    {
    public:
        std::shared_ptr<const character_properties> get_properties(const std::string &font,
                                                                   unsigned size,
                                                                   const std::string &style)
        {
            auto it = std::find_if(m_pool.begin(), m_pool.end(),
                                   [&](const auto &el) {
                                       return el->font == font &&
                                              el->size == size &&
                                              el->style == style;
                                   });

            if(it == m_pool.end()) {
                auto new_prop = std::make_shared<const character_properties>(font, size, style);
                m_pool.push_back(new_prop);
                return new_prop;
            }

            return *it;
        }

    private:
        std::vector<std::shared_ptr<const character_properties>> m_pool;
    };

    class character
    {
    public:
        character(unsigned position,
                  char val,
                  const std::string &font,
                  unsigned size,
                  const std::string &style,
                  character_properties_pool &prop_pool)
            : m_position(position)
            , m_val(val)
            , m_shared_properties(prop_pool.get_properties(font, size, style))
        {}

        unsigned get_position() const
        {
            return m_position;
        }

        char get_val() const
        {
            return m_val;
        }

        const std::string &get_font() const
        {
            return m_shared_properties->font;
        }

        unsigned get_size() const
        {
            return m_shared_properties->size;
        }

        const std::string &get_style() const
        {
            return m_shared_properties->style;
        }

    private:
        unsigned m_position;
        char m_val;
        std::shared_ptr<const character_properties> m_shared_properties;
    };
}

TEST_SUITE("Flyweight") {
    TEST_CASE("Character") {
        character_properties_pool pool;

        SUBCASE("Single character") {
            character ch(0, 'a', "Arial", 12, "normal", pool);

            CHECK(ch.get_position() == 0);
            CHECK(ch.get_val() == 'a');
            CHECK(ch.get_font() == "Arial");
            CHECK(ch.get_size() == 12);
            CHECK(ch.get_style() == "normal");
        }

        SUBCASE("Two characters with shared data") {
            character ch1(0, 'a', "Arial", 12, "normal", pool);
            character ch2(1, 'b', "Arial", 12, "normal", pool);
            CHECK(&ch1.get_style() == &ch2.get_style());
            CHECK(&ch1.get_font() == &ch2.get_font());
        }

        SUBCASE("Two characters with no shared data") {
            character ch1(0, 'a', "Arial", 12, "bold", pool);
            character ch2(1, 'b', "Arial", 12, "normal", pool);
            CHECK(&ch1.get_style() != &ch2.get_style());
            CHECK(&ch1.get_font() != &ch2.get_font());
        }
    }
}
