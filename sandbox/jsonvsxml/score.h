#ifndef SCORE_H
#define SCORE_H

#include <vector>
#include "global/types/string.h"

static const int FIELDS_COUNT = 10;
static const int MEASURE_COUNT = 1000;
static const int NOTE_BY_MEASURE_COUNT = 8;
static const int TEXT_BY_MEASURE_COUNT = 2;

struct Text {
    std::vector<int> fields;
    mu::String text;
};

struct Note {
    std::vector<int> fields;
};

struct Measure {
    std::vector<int> fields;
    std::vector<Note*> notes;
    std::vector<Text*> texts;
};

struct Score {
    std::vector<int> fields;
    std::vector<Measure*> measures;

    static Score* create();
};

#endif // SCORE_H
