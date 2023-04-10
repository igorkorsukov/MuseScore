#ifndef RWJSON_H
#define RWJSON_H

#include "score.h"
#include "global/types/bytearray.h"

class RWJson
{
public:
    RWJson() = default;

    static void write(const Score* s, mu::ByteArray& data);
    static void read(Score* s, const mu::ByteArray& data);
};

#endif // RWJSON_H
