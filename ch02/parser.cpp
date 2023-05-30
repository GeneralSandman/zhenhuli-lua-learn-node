#include <iostream>
#include <vector>
#include <string>

typedef unsigned char byte;
typedef long int int64;
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
    double luacNum;
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
        double number_value;
    } u;
};


void Undump(byte data, size_t size, Prototype* proto) {

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

std::string byte_to_string(std::vector<byte>& bytes) {
    std::string result;
    for(auto byte_value: bytes) {
        // std::cout << "byte_value:" << byte_value << std::endl;
        result += char(byte_value);
    }
    return result;
}

class FileBuffer {
public:
    size_t buffer_size;
    byte* buffer;
    size_t read_offset;

    FileBuffer(size_t buffer_size_) {
        buffer_size = buffer_size_;
        buffer = (byte*)malloc(sizeof(byte)*buffer_size);
        read_offset = 0;
    }

    size_t fill(FILE* input) {
        return fread(buffer, 1, buffer_size, input);
    }

    ~FileBuffer() {
        free(buffer);
    }

    byte readByte() {
        byte byte_value = (byte)(this->buffer[read_offset]);
        read_offset++;
        return byte_value;
    }

    std::vector<byte> readBytes(uint n) {
        std::vector<byte> res(n,0);
        for(int i=0; i<n; i++) {
            res[i] = (byte)(this->buffer[read_offset]);
            read_offset++;
        }
        return res;
    }

    uint32_t readUint32() {
        // 小端
        uint32_t res = 0;
        res = (res<<8)|(uint32_t)(this->buffer[read_offset+3]);
        res = (res<<8)|(uint32_t)(this->buffer[read_offset+2]);
        res = (res<<8)|(uint32_t)(this->buffer[read_offset+1]);
        res = (res<<8)|(uint32_t)(this->buffer[read_offset+0]);
        read_offset += 4;
        return res;
    }

    uint64_t readUint64() {
        // 小端
        uint64_t res = 0;
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+7]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+6]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+5]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+4]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+3]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+2]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+1]);
        res = (res<<8)|(uint64_t)(this->buffer[read_offset+0]);
        read_offset += 8;
        return res;

    }

    int64_t readLuaInteger() {
        return int64_t(readUint64());
    }

    double readLuaNumber() {
        uint64_t value = readUint64();
        // std::cout << "int value:" << value << std::endl;
        // std::cout << "float value:" << *(double*)(&value) << std::endl;
        return *(double*)(&value);
    }

    std::string readString() {
        byte size = uint(readByte());
        if(size == 0) {
            return "";
        } else if (size == 0XFF) {
            size = uint(readUint64());
        }

        std::string res;
        std::vector<byte> bytes = readBytes(size-1);
        for(auto byte_value: bytes) {
            res += std::to_string(byte_value);
        }
        return res;
    }


    int checkHeader() {
        std::vector<byte> bytes = readBytes(4);
        std::string signature = byte_to_string(bytes);
        if (signature != LUA_SIGNATURE) {
            std::cout << "signature:" << signature << std::endl;
            std::cout << "signature invalid" << std::endl;
            return -1;
        }

        if((int)readByte() != LUAC_VERSION) {
            std::cout << "LUAC_VERSION invalid" << std::endl;
            return -1;
        }

        if((int)readByte() != LUAC_FORMAT) {
            std::cout << "LUAC_FORMAT invalid" << std::endl;
            return -1;
        }

        bytes = readBytes(6);
        std::string luac_data = byte_to_string(bytes);
        if (luac_data != LUAC_DATA) {
            std::cout << "luac_data:" << luac_data << std::endl;
            std::cout << "LUAC_DATA invalid" << std::endl;
            return -1;
        }

        if((int)readByte() != CINT_SIZE) {
            std::cout << "CINT_SIZE invalid" << std::endl;
            return -1;
        }

        if((int)readByte() != CSIZET_SIZE) {
            std::cout << "CSIZET_SIZE invalid" << std::endl;
            return -1;
        }
        
        if((int)readByte() != INSTRUCTION_SIZE) {
            std::cout << "INSTRUCTION_SIZE invalid" << std::endl;
            return -1;
        }

        if((int)readByte() != LUA_INTEGER_SIZE) {
            std::cout << "LUA_INTEGER_SIZE invalid" << std::endl;
            return -1;
        }

        if((int)readByte() != LUA_NUMBER_SIZE) {
            std::cout << "LUA_NUMBER_SIZE invalid" << std::endl;
            return -1;
        }

        if(readLuaInteger() != LUAC_INT) {
            std::cout << "LUAC_INT invalid" << std::endl;
            return -1;
        }

        if(readLuaNumber() != LUAC_NUM) {
            std::cout << "LUAC_NUM invalid" << std::endl;
            return -1;
        }


        std::cout << "checkHeader success" << std::endl;
        return 0;
        
    }

    void undump() {
        checkHeader();
    }
};

int main() {
    // read content from binary file
    std::string inputFileName = "./helloworld.luac";
    FILE* input = NULL;
    input = fopen(inputFileName.c_str(), "rb");
    if(input == NULL) {
        exit(1);
    }

    FileBuffer buffer(1024*1024);
    size_t read_size = buffer.fill(input);
    buffer.undump();

    // std::cout << "read_size:" << read_size << std::endl;

}