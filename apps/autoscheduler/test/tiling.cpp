#include "Halide.h"

#include <set>

#include "Tiling.h"

using namespace Halide;
using namespace Halide::Internal;
using namespace Halide::Internal::Autoscheduler;

#define user_assert(c) _halide_internal_assertion(c, Halide::Internal::ErrorReport::User)
#define EXPECT_EQ(expected, actual) expect_eq(__LINE__, expected, actual)

using tilings_t = vector<vector<int64_t>>;

std::string to_string(const tilings_t& tilings) {
    std::ostringstream s;
    s << "[\n";
    bool first_tiling = true;
    for (const auto& t : tilings) {
        if (!first_tiling) {
            s << ",\n";
        }
        s << "  [";
        bool first = true;
        for (const auto& x : t) {
            if (!first) {
                s << ", ";
            }
            s << x;
            first = false;
        }
        s << "]";
        first_tiling = false;
    }
    s << "\n]";

    return s.str();
}

void expect_eq(int line, const tilings_t& expected, const tilings_t& actual) {
    user_assert(expected == actual)
        << "Assert failed on line " << line << "."
        << "\nExpected value = " << to_string(expected)
        << "\nActual value = " << to_string(actual);
}

void test_serial_tilings() {
    {
        vector<int64_t> s;
        s.push_back(3);

        vector<vector<int64_t>> expected;
        expected.push_back({3});

        vector<vector<int64_t>> actual = generate_serial_tilings(s, 0, 0, 0, {}, false, true);

        // Don't split small, odd extents
        EXPECT_EQ(expected, actual);

        // If 'allow_inner_ones' is false, don't split
        actual = generate_serial_tilings(s, 0, 0, 0, {}, false, false);
        expected.clear();
        EXPECT_EQ(expected, actual);
    }

    {
        vector<int64_t> s;
        s.push_back(8);

        vector<vector<int64_t>> expected;
        expected.push_back({8});
        expected.push_back({4});
        expected.push_back({2});

        vector<vector<int64_t>> actual = generate_serial_tilings(s, 0, 0, 0, {}, false, true);

        EXPECT_EQ(expected, actual);
    }

    {
        vector<int64_t> s;
        s.push_back(8);

        vector<vector<int64_t>> expected;
        // If 'filter_small_outer_extents' is true, don't split small extents
        vector<vector<int64_t>> actual = generate_serial_tilings(s, 0, 0, 0, {}, true, true);

        EXPECT_EQ(expected, actual);
    }

    {
        vector<int64_t> s;
        s.push_back(8);

        vector<vector<int64_t>> expected;
        expected.push_back({8});
        expected.push_back({4});
        expected.push_back({2});

        // If 'filter_small_outer_extents' is true but we're not considering the
        // vectorized_loop_index, do split
        vector<vector<int64_t>> actual = generate_serial_tilings(s, 0, 0, 1, {}, true, true);

        EXPECT_EQ(expected, actual);
    }
}

int main(int argc, char **argv) {
    test_serial_tilings();
    printf("All tests passed.\n");
    return 0;
}