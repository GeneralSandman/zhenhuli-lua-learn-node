
luac -v

luac -o helloworld.luac helloworld.lua


go build -o lua_binchunk_parser main.go

./lua_binchunk_parser ./helloworld.luac