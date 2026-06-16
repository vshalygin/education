#include <doctest/doctest.h>
#include <vector>
#include <string>

namespace {
    class journal
    {
        journal() = default;

    public:
        journal(journal &) = delete;
        journal &operator=(const journal &) = delete;

        static journal &instance()
        {
            static journal journal;
            return journal;
        }

        void write(const std::string &note)
        {
            m_notes.push_back(note);
        }

        size_t get_note_number() const
        {
            return m_notes.size();
        }

        std::string get_content() const
        {
            std::string ans;
            if(!m_notes.empty()) {
                ans = m_notes[0];
            }

            for(size_t i = 1; i < m_notes.size(); ++i) {
                ans += "; " + m_notes[i];
            }
            return ans;
        }

        void clear()
        {
            m_notes.clear();
        }

        bool is_empty() const
        {
            return m_notes.empty();
        }

    private:
        std::vector<std::string> m_notes;
    };
}

TEST_SUITE("Singleton") {
    TEST_CASE("Journal") {
        journal::instance().clear();
        CHECK(journal::instance().is_empty());
        CHECK(&journal::instance() == &journal::instance());

        SUBCASE("Add one note") {
            journal::instance().write("note");

            CHECK(journal::instance().get_content() == "note");
            CHECK(journal::instance().get_note_number() == 1);
            CHECK(!journal::instance().is_empty());
        }

        SUBCASE("Add two note") {
            journal::instance().write("note1");
            journal::instance().write("note2");

            CHECK(journal::instance().get_content() == "note1; note2");
            CHECK(journal::instance().get_note_number() == 2);
            CHECK(!journal::instance().is_empty());
        }

        SUBCASE("Clear content") {
            journal::instance().write("note1");
            journal::instance().write("note2");
            journal::instance().clear();

            CHECK(journal::instance().get_content() == "");
            CHECK(journal::instance().get_note_number() == 0);
            CHECK(journal::instance().is_empty());
        }
    }
}
