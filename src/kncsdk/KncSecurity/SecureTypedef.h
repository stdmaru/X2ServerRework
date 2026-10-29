
#define IN
#define OUT

typedef unsigned char   byte;
typedef unsigned short  SpiType;
static_assert( sizeof( SpiType ) == 2, "SpiType is part of the secure packet header (wire format)" );

#define NAMESPACE_BEGIN( x ) namespace x {
#define NAMESPACE_END }
