#include "Game/Game.h"
#include <iostream>

#include <sol/sol.hpp>
#include <sol/forward.hpp>

void TestLua();

int main(int argc, char* argv[]) {
    // TestLua();
    
    Game m_Game;

    m_Game.Initialize();

    m_Game.Run();

    m_Game.Destory();

    return 0;
}

void TestLua() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.script_file("../assets/scripts/test.lua");

    float luaTestV = lua["testv"];

    std::cout << luaTestV << std::endl;
}