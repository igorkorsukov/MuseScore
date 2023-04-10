#ifndef RWXML_H
#define RWXML_H

#include "score.h"
#include "global/types/bytearray.h"

class RWXml
{
public:
    RWXml() = default;

    static void writeSax(const Score* s, mu::ByteArray& data);
    static void readSax(Score* s, const mu::ByteArray& data);

    static void readDom(Score* s, const mu::ByteArray& data);
};

#endif // RWXML_H
