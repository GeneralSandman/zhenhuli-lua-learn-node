#include <iostream>
#include <vector>
#include <string>

typedef unsigned char byte;
typedef long int int64;
typedef float float64;
typedef unsigned int uint32;

typedef struct BinaryChunk BinaryChunk;
typedef struct Header Header;
typedef struct Prototype Prototype;
typedef struct Upvalue Upvalue;
typedef struct LocVar LocVar;



struct Header {
    byte signature[4];
    byte version;
    byte format;
    byte luacData[6];
    byte cintSize;
    byte sizetSize;
    byte instructionSize;
    byte luaIntegerSize;
    byte luaNumberSize;
    int64 luacInt;
    float64 luacNum;
};

struct Prototype {
    std::string Source;
    uint32 LineDefined;
    uint32 LastLineDefined;

    byte NumParams;
    byte IsVararg;
    byte MaxStackSize;

    uint32* Code;
    Upvalue* Upvalues;
};

struct BinaryChunk {
    Header header;
    byte sizeUpvalues;
    Prototype* mainFunc;
};

struct Upvalue {
    byte Instack;
    byte Idx;
};

struct LocVar {
    std::string VarName;
    uint32 StartPC;
    uint32 EndPC;
};
