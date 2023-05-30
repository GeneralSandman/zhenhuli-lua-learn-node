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


const std::string LUA_SIGNATURE    = "\x1bLua";
const int LUAC_VERSION             = 0x53;
const int LUAC_FORMAT              = 0;
const std::string LUAC_DATA        = "\x19\x93\r\n\x1a\n";
const int CINT_SIZE                = 4;
const int CSIZET_SIZE              = 8;
const int INSTRUCTION_SIZE         = 4;
const int LUA_INTEGER_SIZE         = 8;
const int LUA_NUMBER_SIZE          = 8;
const int LUAC_INT                 = 0x5678;
const double LUAC_NUM              = 370.5;

const int TAG_NIL       = 0x00;
const int TAG_BOOLEAN   = 0x01;
const int TAG_NUMBER    = 0x03;
const int TAG_INTEGER   = 0x13;
const int TAG_SHORT_STR = 0x04;
const int TAG_LONG_STR  = 0x14;

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

typedef enum {
    CONSTANT_TYPE_NIL,
    CONSTANT_TYPE_BOOLEAN,
    CONSTANT_TYPE_INTEGER,
    CONSTANT_TYPE_NUMBER,
    CONSTANT_TYPE_SHORT_STR,
    CONSTANT_TYPE_LONG_STR,
} Constant_Type;

struct Constant {
    Constant_Type type;
    union {
        byte byte_value;
        bool bool_value;
        int64_t int_value;
        float64 number_value;
    } u;
};


void Undump(byte data, size_t size, Prototype* proto) {

}


byte readByte() {

}

std::vector<byte> readBytes() {

}

uint32_t readUint32() {

}

uint64_t readUint64() {

}

int64_t readLuaInteger() {

}

float64 readLuaNumber() {

}

std::string readString() {

}

void checkHeader() {

}

void readProto(Prototype* proto) {

}

std::vector<uint32> readCode() {

}

std::vector<Constant> readConstants() {

}

void readConstants(Constant* constant) {

}



std::vector<Prototype> readProtos() {
    
}

std::vector<uint32> readLineInfo() {

}

std::vector<LocVar> readLocVars() {
    
}

std::vector<std::string> readUpvalueNames() {

}

int main() {
    // read content from binary file
    std::string inputFileName = "./helloworld.luac";

    FILE* input = NULL;
    input = fopen(inputFileName.c_str(), "rb");
    if(input == NULL) {
        exit(1);
    }

    size_t buffer_size = 1024*1024;
    void* buffer = malloc(sizeof(byte)*buffer_size);


    size_t read_size = fread(buffer, buffer_size, 100, input);

    std::cout << "read_size:" << read_size << std::endl;


    free(buffer);

}