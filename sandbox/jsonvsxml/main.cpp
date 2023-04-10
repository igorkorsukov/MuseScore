#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "score.h"
#include "rwjson.h"
#include "rwxml.h"

using namespace mu;

using mclock = std::chrono::high_resolution_clock;

static double elapsed(mclock::time_point start)
{
    auto end = mclock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count();
}

static bool writeFile(const std::string& filePath, const ByteArray& data)
{
    FILE* file = fopen(filePath.c_str(), "wb");
    if (!file) {
        return false;
    }

    fwrite(data.constData(), 1, data.size(), file);

    fclose(file);

    return true;
}

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;

    Score* origin = Score::create();
    Score* fromJson = new Score();
    Score* fromXmlSax = new Score();
    Score* fromXmlDom = new Score();

    ByteArray json;
    {
        std::cout << "begin json     write" << std::endl;
        auto startPoint = mclock::now();
        RWJson::write(origin, json);
        writeFile("score.json", json);
        std::cout << "end   json     write, elapsed: " << elapsed(startPoint) << std::endl;
    }

    {
        std::cout << "begin json     read" << std::endl;
        auto startPoint = mclock::now();
        RWJson::read(fromJson, json);
        std::cout << "end   json     read,  elapsed: " << elapsed(startPoint) << std::endl;
    }

    ByteArray xml;
    {
        std::cout << "begin xml(sax) write" << std::endl;
        auto startPoint = mclock::now();
        RWXml::writeSax(origin, xml);
        writeFile("score.xml", xml);
        std::cout << "end   xml(sax) write, elapsed: " << elapsed(startPoint) << std::endl;
    }

    {
        std::cout << "begin xml(sax) read" << std::endl;
        auto startPoint = mclock::now();
        RWXml::readSax(fromXmlSax, xml);
        std::cout << "end   xml(sax) read,  elapsed: " << elapsed(startPoint) << std::endl;
    }

    {
        std::cout << "begin xml(dom) read" << std::endl;
        auto startPoint = mclock::now();
        RWXml::readDom(fromXmlDom, xml);
        std::cout << "end   xml(dom) read,  elapsed: " << elapsed(startPoint) << std::endl;
    }
}
