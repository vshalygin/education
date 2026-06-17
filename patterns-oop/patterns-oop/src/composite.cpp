#include <doctest/doctest.h>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

namespace {
    class fs_object
    {
    public:
        explicit fs_object(const std::string &name)
            : m_name(name)
        {}

        virtual ~fs_object() = default;

        std::string get_name() const
        {
            return m_name;
        }

        virtual unsigned get_size() const = 0;

    private:
        std::string m_name;
    };

    class file
        : public fs_object
    {
    public:
        file(const std::string &name, unsigned size)
            : fs_object(name)
            , m_size(size)
        {}

        unsigned get_size() const override
        {
            return m_size;
        }

    private:
        unsigned m_size;
    };

    class directory
        : public fs_object
    {
    public:
        explicit directory(const std::string &name)
            : fs_object(name)
        {}

        unsigned get_size() const override
        {
            unsigned ans = 0;
            for(const auto &el : m_elements) {
                ans += el->get_size();
            }
            return ans;
        }

        void add(std::unique_ptr<fs_object> object)
        {
            auto it = std::find_if(m_elements.begin(), m_elements.end(),
                                   [&](const auto &el) { return el->get_name() == object->get_name(); });
            if(it != m_elements.end()) {
                throw std::runtime_error("object with such name exists in target directory");
            }

            m_elements.push_back(std::move(object));
        }

        void remove(const std::string &name)
        {
            auto it = std::find_if(m_elements.begin(), m_elements.end(),
                                   [&](const auto &el) { return el->get_name() == name; });
            if(it != m_elements.end()) {
                m_elements.erase(it);
            }
        }

    private:
        std::vector<std::unique_ptr<fs_object>> m_elements;
    };
}

TEST_SUITE("Composite") {
    TEST_CASE("Filesystem") {
        directory root("root");
        CHECK(root.get_name() == "root");
        CHECK(root.get_size() == 0);

        SUBCASE("One file") {
            auto file0 = std::make_unique<file>("file", 40);
            CHECK(file0->get_name() == "file");
            CHECK(file0->get_size() == 40);

            root.add(std::move(file0));
            CHECK(root.get_size() == 40);
        }

        SUBCASE("Three files") {
            auto file1 = std::make_unique<file>("file1", 40);
            auto file2 = std::make_unique<file>("file2", 50);
            auto file3 = std::make_unique<file>("file3", 60);

            root.add(std::move(file1));
            root.add(std::move(file2));
            root.add(std::move(file3));
            CHECK(root.get_size() == 150);

            root.remove("file2");
            CHECK(root.get_size() == 100);
        }

        SUBCASE("Nested directory") {
            auto dir = std::make_unique<directory>("directory");
            dir->add(std::make_unique<file>("file", 40));

            root.add(std::move(dir));
            CHECK(root.get_size() == 40);
        }

        SUBCASE("Add a file with an existing name") {
            root.add(std::make_unique<file>("name", 1));
            CHECK_THROWS(root.add(std::make_unique<file>("name", 100)));
        }

        SUBCASE("Remove non-existent file") {
            root.add(std::make_unique<file>("name", 1));
            root.remove("not-name");
            CHECK(root.get_size() == 1);
        }
    }
}
