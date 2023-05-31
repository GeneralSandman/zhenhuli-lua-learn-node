#include <iostream>
#include <vector>
#include <string>

typedef unsigned char byte;
typedef long int int64;
typedef unsigned int uint32;

typedef struct BinaryChunk BinaryChunk;
typedef struct Header Header;
typedef struct Prototype Prototype;
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

typedef enum {
    CONSTANT_TYPE_NIL,
    CONSTANT_TYPE_BOOLEAN,
    CONSTANT_TYPE_INTEGER,
    CONSTANT_TYPE_NUMBER,
    CONSTANT_TYPE_SHORT_STR,
    CONSTANT_TYPE_LONG_STR,
} Constant_Type;


class Constant {
public:
    Constant_Type type;
    union {
        byte byte_value;
        bool bool_value;
        int64_t int_value;
        double number_value;
        std::string* string_value;
    } u;
};

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

class Upvalue {
public:
    byte Instack;
    byte Idx;
};
struct Prototype {
    std::string Source;
    uint32 LineDefined;
    uint32 LastLineDefined;

    byte NumParams;
    byte IsVararg;
    byte MaxStackSize;

    std::vector<uint32> Code;
    std::vector<Constant> Constants;
    std::vector<Upvalue> Upvalues;
    std::vector<Prototype> Protots;
    std::vector<uint32_t> LineInfo;
    std::vector<LocVar> LocVars;
    std::vector<std::string> UpvalueNames; 
};

struct BinaryChunk {
    Header header;
    byte sizeUpvalues;
    Prototype* mainFunc;
};



struct LocVar {
    std::string VarName;
    uint32 StartPC;
    uint32 EndPC;
};




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

    void undump() {
        int res = checkHeader();
        if(res != 0) {
            printf("checkHeader error:%d\n", res);
        }
        readByte();
        Prototype proto = readProto("");
        list(proto);
    }

    void list(Prototype proto) {
        printHeader(proto);
        printCode(proto);
        printDetail(proto);
        for(auto proto : proto.Protots) {
            list(proto);
        }
    }

    void printHeader(Prototype proto) {
        std::string funcType = "main";
        if(proto.LineDefined > 0) {
            funcType = "function";
        }

        std::string varargFlag = "";
        if(proto.IsVararg > 0) {
            varargFlag = "+";
        }

        printf("\n%s <%s:%d,%d> (%ld instructions)\n",
		    funcType.c_str(), proto.Source.c_str(), proto.LineDefined, proto.LastLineDefined, proto.Code.size());

        printf("%d%s params, %d slots, %ld upvalues, ",
		    proto.NumParams, varargFlag.c_str(), proto.MaxStackSize, proto.Upvalues.size());

        printf("%ld locals, %ld constants, %ld functions\n",
		    proto.LocVars.size(), proto.Constants.size(), proto.Protots.size());
    }

    void printCode(Prototype proto) {
        for(size_t pc = 0; pc < proto.Code.size(); pc++) {
            std::string line = "-";
            if(proto.LineInfo.size()) {
                line = std::to_string(proto.LineInfo[pc]);
            }
            printf("\t%ld\t[%s]\t0x%08X\n", pc+1, line.c_str(), proto.Code[pc]);
        }
    }

    void printDetail(Prototype proto) {
	    printf("constants (%ld):\n", proto.Constants.size());
        for(size_t i = 0; i<proto.Constants.size(); i++) {
            printf("\t%ld\t%s\n", i+1, constantToString(proto.Constants[i]).c_str());
        }

        printf("locals (%ld):\n", proto.LocVars.size());
        for(size_t i=0; i<proto.LocVars.size(); i++) {
            printf("\t%ld\t%s\t%ud\t%d\n",
	    		i, proto.LocVars[i].VarName.c_str(), proto.LocVars[i].StartPC+1, proto.LocVars[i].EndPC+1);
        }

	    printf("upvalues (%ld):\n", proto.Upvalues.size());
        for(size_t i=0; i<proto.Upvalues.size(); i++) {
	    	printf("\t%ld\t%s\t%d\t%d\n",
	    		i, upvalName(proto, i).c_str(), proto.Upvalues[i].Instack, proto.Upvalues[i].Idx);
	    }

    }

    std::string constantToString(Constant constant) {
        switch(constant.type) {
            case CONSTANT_TYPE_NIL:
            return "nil";
            break;
            case CONSTANT_TYPE_BOOLEAN: 
            return std::to_string(constant.u.bool_value);
            break;
            case CONSTANT_TYPE_INTEGER: 
            return std::to_string(constant.u.int_value);
            break;
            case CONSTANT_TYPE_NUMBER: 
            return std::to_string(constant.u.number_value);
            break;
            case CONSTANT_TYPE_SHORT_STR: 
            return std::string(*constant.u.string_value);
            break;
            case CONSTANT_TYPE_LONG_STR: 
            return std::string(*constant.u.string_value);
            break;
        }
    }

    std::string upvalName(Prototype proto, int idx) {
        if(idx >= proto.UpvalueNames.size()){
            return "-";
        }
        return proto.UpvalueNames[idx];
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
            res += char(byte_value);
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

        return 0;
    }

    Prototype readProto(std::string parentSource) {
        std::string source = readString();
        if (source == "") {
            source = parentSource;
        }

        Prototype *proto = new Prototype;

        proto->Source = source;
        proto->LineDefined = readUint32();
        proto->LastLineDefined = readUint32();
        proto->NumParams = readByte();
        proto->IsVararg = readByte();
        proto->MaxStackSize = readByte();
        proto->Code = readCode();
        proto->Constants = readConstants();
        proto->Upvalues = readUpvalues();
        proto->Protots = readProtos(source);
        proto->LineInfo = readLineInfo();
        proto->LocVars = readLocVars();
        proto->UpvalueNames = readUpvalueNames();

        return *proto;
    }

    std::vector<uint32> readCode() {
        uint32_t size = readUint32();
        std::vector<uint32> res(size, 0);
        for(uint32_t i=0; i<size; i++) {
            res[i] = readUint32();
        }
        return res;
    }

    std::vector<Constant> readConstants() {
        uint32_t size = readUint32();
        std::vector<Constant> res(size);
        for(uint32_t i=0; i<size; i++) {
            res[i] = readConstant();
        }
        return res;
    }

    Constant readConstant() {
        Constant res;
        Constant_Type type = (Constant_Type)(readByte());

        std::string* tmp = nullptr;

        switch(type) {
            case CONSTANT_TYPE_NIL: 
            res.type = type;
            break;
            case CONSTANT_TYPE_BOOLEAN: 
            res.type = type;
            res.u.bool_value = (readByte() != 0);
            break;
            case CONSTANT_TYPE_INTEGER: 
            res.type = type;
            res.u.int_value = readLuaInteger();
            break;
            case CONSTANT_TYPE_NUMBER: 
            res.type = type;
            res.u.int_value = readLuaNumber();
            break;
            case CONSTANT_TYPE_SHORT_STR: 
            res.type = type;
            // std::cout << "readString:" << readString() << std::endl;
            tmp = new std::string(readString());
            res.u.string_value = tmp;
            break;
            case CONSTANT_TYPE_LONG_STR: 
            res.type = type;
            tmp = new std::string(readString());
            res.u.string_value = tmp;
            break;
        }

        return res;
    }

    std::vector<Upvalue> readUpvalues() {
        uint32_t size = readUint32();
        std::vector<Upvalue> res(size);
        for(uint32_t i=0; i<size; i++) {
            res[i].Instack = readByte();
            res[i].Idx = readByte();
        }
        return res;
    }

    std::vector<Prototype> readProtos(std::string parentSource) {
        uint32_t size = readUint32();
        std::vector<Prototype> res(size);
        for(uint32_t i=0; i<size; i++) {
            res[i] = readProto(parentSource);
        }
        return res;
    }

    std::vector<uint32> readLineInfo() {
        uint32_t size = readUint32();
        std::vector<uint32> res(size, 0);
        for(uint32_t i=0; i<size; i++) {
            res[i] = readUint32();
        }
        return res;
    }

    std::vector<LocVar> readLocVars() {
        uint32_t size = readUint32();
        std::vector<LocVar> res(size);
        for(uint32_t i=0; i<size; i++) {
            res[i].VarName = readString();
            res[i].StartPC = readUint32();
            res[i].EndPC = readUint32();
        }
        return res;
    }

    std::vector<std::string> readUpvalueNames() {
        uint32_t size = readUint32();
        std::vector<std::string> res(size);
        for(uint32_t i=0; i<size; i++) {
            res[i] = readString();
        }
        return res;

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