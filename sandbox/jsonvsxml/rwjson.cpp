#include "rwjson.h"

#include "global/serialization/json.h"

using namespace mu;

static void writeFields(JsonObject& obj, const std::vector<int>& fields)
{
    for (size_t i = 0; i < fields.size(); ++i) {
        obj["field_" + std::to_string(i)] = fields.at(i);
    }
}

static void readFields(const JsonObject& obj, std::vector<int>& fields)
{
    for (size_t i = 0; i < FIELDS_COUNT; ++i) {
        fields.push_back(obj.value("field_" + std::to_string(i)).toInt());
    }
}

template<class T>
static JsonArray toArr(const std::vector<T>& vals)
{
    JsonArray arr;
    for (size_t i = 0; i < vals.size(); ++i) {
        arr << toObj(vals.at(i));
    }
    return arr;
}

static JsonObject toObj(Note* n)
{
    JsonObject obj;
    writeFields(obj, n->fields);
    return obj;
}

static void fromObj(const JsonObject& obj, Note* n)
{
    readFields(obj, n->fields);
}

static JsonObject toObj(Text* t)
{
    JsonObject obj;
    writeFields(obj, t->fields);
    obj["text"] = t->text;
    return obj;
}

static void fromObj(const JsonObject& obj, Text* t)
{
    readFields(obj, t->fields);
    t->text = obj.value("text").toString();
}

static JsonObject toObj(Measure* m)
{
    JsonObject obj;
    writeFields(obj, m->fields);
    obj["notes"] = toArr(m->notes);
    obj["texts"] = toArr(m->texts);
    return obj;
}

static void fromObj(const JsonObject& obj, Measure* m)
{
    readFields(obj, m->fields);

    JsonArray notes = obj.value("notes").toArray();
    for (size_t i = 0; i < notes.size(); ++i) {
        Note* n = new Note();
        fromObj(notes.at(i).toObject(), n);
        m->notes.push_back(n);
    }

    JsonArray texts = obj.value("texts").toArray();
    for (size_t i = 0; i < texts.size(); ++i) {
        Text* t = new Text();
        fromObj(texts.at(i).toObject(), t);
        m->texts.push_back(t);
    }
}

static JsonObject toObj(const Score* s)
{
    JsonObject obj;
    writeFields(obj, s->fields);
    obj["measures"] = toArr(s->measures);
    return obj;
}

static void fromObj(const JsonObject& obj, Score* s)
{
    readFields(obj, s->fields);
    JsonArray measures = obj.value("measures").toArray();
    for (size_t i = 0; i < measures.size(); ++i) {
        Measure* m = new Measure();
        fromObj(measures.at(i).toObject(), m);
        s->measures.push_back(m);
    }
}

void RWJson::write(const Score* s, mu::ByteArray& data)
{
    data = JsonDocument(toObj(s)).toJson();
}

void RWJson::read(Score* s, const mu::ByteArray& data)
{
    JsonObject obj = JsonDocument::fromJson(data).rootObject();
    fromObj(obj, s);
}
