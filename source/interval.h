#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
    public:
        double min, max;

        interval() : min(+infinity), max(-infinity) {}  // empty, nothing inside interval

        interval(double min, double max) : min(min), max(max) {}

        double size() const {
            return max - min;
        }

        bool contains(double x) const {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const {
            return min < x && x < max;
        }

        static const interval empty, universe;  // define static members
                                                //
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif
