
luac -v

luac -o helloworld.luac helloworld.lua


go build -o lua_binchunk_parser_go main.go

./lua_binchunk_parser_go ./helloworld.luac


# g++ -std=c++11 -o lua_binchunk_parser_cpp parser.cpp && ./lua_binchunk_parser_cpp