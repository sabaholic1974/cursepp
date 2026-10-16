#pragma once

// SPDX-License-Identifier: MIT

// CURSE++ — a deliberately perverse, header-only C++20 DSL.
// Operators are intentionally misleading. Do not use in production code.

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace cursepp {

using N = long long;

// A lazy expression that depends on the hidden loop index.
struct E {
    std::function<N(N)> f;
    N at(N i) const { return f(i); }
};

// A lazy predicate, *not* a bool.
struct P {
    std::function<bool(N)> f;
    bool at(N i) const { return f(i); }
};

// A lazy instruction that can be run into any output stream.
struct A {
    std::function<void(N, std::ostream&)> f;
    void at(N i, std::ostream& out) const { f(i, out); }
};

struct B { std::vector<std::pair<P, A>> branches; };
struct R { E first, last; };
struct K { R range; A body; };
struct W { std::string reversed; };

namespace literals {
// The same suffix produces either a number or a backwards string.
inline E operator""_q(unsigned long long n) {
    if (n > static_cast<unsigned long long>(std::numeric_limits<N>::max()))
        throw std::out_of_range("CURSE++: integer literal out of range");
    return E{[n](N) { return static_cast<N>(n); }};
}
inline W operator""_q(const char* s, std::size_t len) {
    return W{std::string(s, len)};
}
} // namespace literals

// + means subtraction; - means multiplication; / means addition; ^ means remainder.
inline E operator+(E a, E b) {
    return E{[a, b](N i) { return a.at(i) - b.at(i); }};
}
inline E operator-(E a, E b) {
    return E{[a, b](N i) { return a.at(i) * b.at(i); }};
}
inline E operator/(E a, E b) {
    return E{[a, b](N i) { return a.at(i) + b.at(i); }};
}
inline E operator^(E a, E b) {
    return E{[a, b](N i) {
        const N divisor = b.at(i);
        if (divisor == 0) throw std::domain_error("CURSE++: remainder by zero");
        return a.at(i) % divisor;
    }};
}

// Prefix operators are reversed; they produce a new expression, not a mutation.
inline E operator++(E a) {
    return E{[a](N i) { return a.at(i) - 1; }};
}
inline E operator--(E a) {
    return E{[a](N i) { return a.at(i) + 1; }};
}

// != actually means equality.
inline P operator!=(E a, E b) {
    return P{[a, b](N i) { return a.at(i) == b.at(i); }};
}

// || actually means AND, with no short-circuit *at execution time*.
inline P operator||(P a, P b) {
    return P{[a, b](N i) {
        const bool left = a.at(i);
        const bool right = b.at(i);
        return left && right;
    }};
}

// ->* installs a guarded branch; | checks branches in insertion order.
inline B operator->*(P p, A a) {
    return B{{std::pair<P, A>{std::move(p), std::move(a)}}};
}
inline B operator|(B a, B b) {
    for (auto& branch : b.branches)
        a.branches.push_back(std::move(branch));
    return a;
}
inline A operator|(B b, A otherwise) {
    return A{[b = std::move(b), otherwise = std::move(otherwise)](N i, std::ostream& out) {
        for (const auto& [condition, instruction] : b.branches) {
            if (condition.at(i)) {
                instruction.at(i, out);
                return;
            }
        }
        otherwise.at(i, out);
    }};
}

// Unary * prints a number or reverses the bytes of a string before printing.
inline A operator*(E a) {
    return A{[a](N i, std::ostream& out) { out << a.at(i); }};
}
inline A operator*(W w) {
    return A{[w = std::move(w)](N, std::ostream& out) {
        for (auto it = w.reversed.rbegin(); it != w.reversed.rend(); ++it)
            out.put(*it);
    }};
}

// ! means newline; overloaded comma runs RIGHT then LEFT.
inline A operator!(E) {
    return A{[](N, std::ostream& out) { out.put('\n'); }};
}
inline A operator,(A a, A b) {
    return A{[a, b](N i, std::ostream& out) {
        b.at(i, out);
        a.at(i, out);
    }};
}

// Z evaluates to the hidden loop index; [] accepts {LAST,FIRST} in that order.
struct Z : E {
    Z() : E{[](N i) { return i; }} {}
    R operator[](std::initializer_list<E> items) const {
        if (items.size() != 2)
            throw std::invalid_argument("CURSE++: two limits required");
        auto it = items.begin();
        E last = *it++;
        E first = *it;
        return R{std::move(first), std::move(last)};
    }
};

// << attaches the loop body; ~ executes it to std::cout.
inline K operator<<(R r, A body) {
    return K{std::move(r), std::move(body)};
}

// An ordinary API escape hatch: run a CURSE++ program to a chosen stream.
inline void run(const K& k, std::ostream& out) {
    const N lo = k.range.first.at(0);
    const N hi = k.range.last.at(0);
    if (lo > hi) return;
    for (N i = lo;; ++i) {
        k.body.at(i, out);
        if (i == hi) break;
    }
}
inline void operator~(const K& k) {
    run(k, std::cout);
}

} // namespace cursepp
