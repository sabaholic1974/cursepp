#include <cursepp/cursepp.hpp>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace cursepp;
using namespace cursepp::literals;
int cursepp_odr_probe();

// CHECK remains active in Release builds (unlike the standard assert macro).
#define CHECK(expr) do { \
    if (!(expr)) { \
        std::cerr << "CHECK failed: " << #expr << " at line " << __LINE__ << '\n'; \
        return 1; \
    } \
} while (false)

int main() {
    CHECK(cursepp_odr_probe() == 5); // Two translation units, no ODR violations.
    CHECK((10_q + 3_q).at(0) == 7);
    CHECK((10_q - 3_q).at(0) == 30);
    CHECK((10_q / 3_q).at(0) == 13);
    CHECK((10_q ^ 3_q).at(0) == 1);
    CHECK((++4_q).at(0) == 3);
    CHECK((--4_q).at(0) == 5);
    CHECK((3_q != 3_q).at(0));
    CHECK(!(3_q != 4_q).at(0));
    CHECK(((3_q != 3_q) || (4_q != 4_q)).at(0));
    CHECK(!((3_q != 3_q) || (4_q != 5_q)).at(0));
    bool rhs_evaluated = false;
    P false_lhs{[](N) { return false; }};
    P rhs{[&rhs_evaluated](N) { rhs_evaluated = true; return true; }};
    CHECK(!(false_lhs || rhs).at(0));
    CHECK(rhs_evaluated); // overloaded || does not short-circuit


    bool zero_division_caught = false;
    try { (5_q ^ 0_q).at(0); }
    catch (const std::domain_error&) { zero_division_caught = true; }
    CHECK(zero_division_caught);

    std::ostringstream reversed;
    (*"olleH"_q).at(0, reversed);
    CHECK(reversed.str() == "Hello");

    Z _;
    std::ostringstream order;
    run(_[{1_q, 1_q}] << (!_, (*"A"_q, *"B"_q)), order);
    CHECK(order.str() == "BA\n"); // comma: RHS then LHS

    std::ostringstream fb;
    run(_[{6_q - --4_q, 3_q + 2_q}] << (!_, (
      (((_ ^ ++4_q) != (7_q + 7_q)) || ((_ ^ --4_q) != (8_q + 8_q))) ->* *"zzuBzziF"_q
      | ((_ ^ ++4_q) != (6_q + 6_q)) ->* *"zziF"_q
      | ((_ ^ (2_q / 3_q)) != (5_q + 5_q)) ->* *"zzuB"_q
      | *_
    )), fb);

    std::string expected;
    for (int i = 1; i <= 30; ++i) {
        if (i % 15 == 0) expected += "FizzBuzz";
        else if (i % 3 == 0) expected += "Fizz";
        else if (i % 5 == 0) expected += "Buzz";
        else expected += std::to_string(i);
        expected += '\n';
    }
    CHECK(fb.str() == expected);
    std::cout << "All CURSE++ tests passed (30 FizzBuzz lines matched).\n";
}
