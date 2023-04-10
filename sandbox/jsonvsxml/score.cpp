#include "score.h"

static void addFields(std::vector<int>& fields)
{
    for (int f = 0; f < FIELDS_COUNT; ++f) {
        fields.push_back(f);
    }
}

Score* Score::create()
{
    Score* s = new Score();
    addFields(s->fields);
    for (int mi = 0; mi < MEASURE_COUNT; ++mi) {
        Measure* m = new Measure();
        addFields(m->fields);

        for (int ni = 0; ni < NOTE_BY_MEASURE_COUNT; ++ni) {
            Note* n = new Note();
            addFields(n->fields);
            m->notes.push_back(n);
        }

        for (int ti = 0; ti < TEXT_BY_MEASURE_COUNT; ++ti) {
            Text* t = new Text();
            addFields(t->fields);
            t->text = u"Some Text";
            m->texts.push_back(t);
        }

        s->measures.push_back(m);
    }

    return s;
}
