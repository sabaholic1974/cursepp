#include <cursepp/cursepp.hpp>
using namespace cursepp;
using namespace cursepp::literals;

int cursepp_odr_probe() {
    return static_cast<int>((9_q + 4_q).at(0));
}
