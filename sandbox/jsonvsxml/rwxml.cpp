#include "rwxml.h"

#include <cstring>

#include "global/serialization/xmlstreamwriter.h"
#include "global/serialization/xmlstreamreader.h"
#include "global/serialization/xmldom.h"
#include "global/io/buffer.h"

using namespace mu;

// Sax ---------------------
static void writeFields(XmlStreamWriter& xml, const std::vector<int>& fields)
{
    for (size_t i = 0; i < fields.size(); ++i) {
        xml.element("field_" + std::to_string(i), fields.at(i));
    }
}

static void readFields(const AsciiStringView& tag, XmlStreamReader& xml,  std::vector<int>& fields)
{
    if (std::strncmp("field_", tag.ascii(), 6) == 0) {
        fields.push_back(xml.readInt());
    }
}

static void writeNote(XmlStreamWriter& xml, const Note* n)
{
    xml.startElement("Note");
    writeFields(xml, n->fields);
    xml.endElement();
}

static void readNote(XmlStreamReader& xml, Note* n)
{
    while (xml.readNextStartElement()) {
        const AsciiStringView tag(xml.name());
        readFields(tag, xml, n->fields);
    }
}

static void writeText(XmlStreamWriter& xml, const Text* t)
{
    xml.startElement("Text");
    writeFields(xml, t->fields);
    xml.element("text", t->text);
    xml.endElement();
}

static void readText(XmlStreamReader& xml, Text* t)
{
    while (xml.readNextStartElement()) {
        const AsciiStringView tag(xml.name());
        if (tag == "text") {
            t->text = xml.readText();
        } else {
            readFields(tag, xml, t->fields);
        }
    }
}

static void writeMeasure(XmlStreamWriter& xml, const Measure* m)
{
    xml.startElement("Measure");
    writeFields(xml, m->fields);
    for (const Note* n : m->notes) {
        writeNote(xml, n);
    }
    for (const Text* t : m->texts) {
        writeText(xml, t);
    }
    xml.endElement();
}

static void readMeasure(XmlStreamReader& xml, Measure* m)
{
    while (xml.readNextStartElement()) {
        const AsciiStringView tag(xml.name());
        if (tag == "Note") {
            Note* n = new Note();
            readNote(xml, n);
            m->notes.push_back(n);
        } else if (tag == "Text") {
            Text* t = new Text();
            readText(xml, t);
            m->texts.push_back(t);
        } else {
            readFields(tag, xml, m->fields);
        }
    }
}

static void writeScore(XmlStreamWriter& xml, const Score* s)
{
    xml.startElement("Score");
    writeFields(xml, s->fields);
    for (const Measure* m : s->measures) {
        writeMeasure(xml, m);
    }
    xml.endElement();
}

static void readScore(XmlStreamReader& xml,  Score* s)
{
    while (xml.readNextStartElement()) {
        const AsciiStringView tag(xml.name());
        if (tag == "Measure") {
            Measure* m = new Measure();
            readMeasure(xml, m);
            s->measures.push_back(m);
        } else {
            readFields(tag, xml, s->fields);
        }
    }
}

void RWXml::writeSax(const Score* s, mu::ByteArray& data)
{
    io::Buffer buf(&data);
    buf.open(io::IODevice::OpenMode::WriteOnly);

    XmlStreamWriter xml(&buf);
    xml.startDocument();

    writeScore(xml, s);

    xml.flush();
}

void RWXml::readSax(Score* s, const mu::ByteArray& data)
{
    XmlStreamReader xml(data);

    while (xml.readNextStartElement()) {
        const AsciiStringView tag(xml.name());
        if (tag == "Score") {
            readScore(xml, s);
        }
    }
}

// Dom -----------------------------------------------------------
static void fromEl(const XmlDomNode& el, Note* n)
{
    XmlDomNode child = el.firstChild();

    while (!child.isNull()) {
        String val = child.toElement().text();
        n->fields.push_back(val.toInt());

        child = child.nextSibling();
    }
}

static void fromEl(const XmlDomNode& el, Text* t)
{
    XmlDomNode child = el.firstChild();

    while (!child.isNull()) {
        String nodeName = child.nodeName();
        if (nodeName == u"text") {
            t->text = child.toElement().text();
        } else if (nodeName.startsWith(u"field_")) {
            String val = child.toElement().text();
            t->fields.push_back(val.toInt());
        }

        child = child.nextSibling();
    }
}

static void fromEl(const XmlDomNode& el, Measure* m)
{
    XmlDomNode child = el.firstChild();

    while (!child.isNull()) {
        String nodeName = child.nodeName();
        if (nodeName == u"Note") {
            Note* n = new Note();
            fromEl(child, n);
            m->notes.push_back(n);
        } else if (nodeName == u"Text") {
            Text* t = new Text();
            fromEl(child, t);
            m->texts.push_back(t);
        } else if (nodeName.startsWith(u"field_")) {
            String val = child.toElement().text();
            m->fields.push_back(val.toInt());
        }

        child = child.nextSibling();
    }
}

static void fromEl(const XmlDomNode& el, Score* s)
{
    XmlDomNode child = el.firstChild();

    while (!child.isNull()) {
        String nodeName = child.nodeName();
        if (nodeName == u"Measure") {
            Measure* m = new Measure();
            fromEl(child, m);
            s->measures.push_back(m);
        } else if (nodeName.startsWith(u"field_")) {
            String val = child.toElement().text();
            s->fields.push_back(val.toInt());
        }

        child = child.nextSibling();
    }
}

void RWXml::readDom(Score* s, const mu::ByteArray& data)
{
    XmlDomDocument doc;
    doc.setContent(data);

    XmlDomElement scoreEl = doc.rootElement();
    fromEl(scoreEl, s);
}
