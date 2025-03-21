#pragma once

template <typename T>
class Array<T, 2>;

struct PhaseMapElement {
    PhaseMapElement() = default;
    PhaseMapElement(bool a_flag);
    PhaseMapElement(bool a_flag, double a_consistency);
    PhaseMapElement(const PhaseMapElement& src) = default;
    double operator()() const { return consistency; }

    bool flag { false };
    double consistency { 0. };
};

typedef Array<PhaseMapElement, 2> PhaseMap;
