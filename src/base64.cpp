#include <encmetric/base64.hpp>

using namespace adv;

static uint chLen(const byte *);

static bool validChar(const byte *, uint &) noexcept;

static uint decode(byte_data *uni, const byte *by, size_t l);

static uint encode(const byte_data &uni, byte *by, size_t l);
