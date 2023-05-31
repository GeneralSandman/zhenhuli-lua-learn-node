
luac -v

luac -o helloworld.luac helloworld.lua


go build -o lua_binchunk_parser_go main.go


echo "=============lua_binchunk_parser_go=============\n"
./lua_binchunk_parser_go ./helloworld.luac


echo "=============lua_binchunk_parser_cpp=============\n"
g++ -std=c++11 -o lua_binchunk_parser_cpp parser.cpp && ./lua_binchunk_parser_cpp