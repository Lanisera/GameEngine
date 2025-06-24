#include "Game/Game.h"
#include <iostream>

#include <sol/sol.hpp>

int main(int argc, char* argv[]) {
    Game m_Game;

    m_Game.Initialize();

    m_Game.Run();

    m_Game.Destory();

    return 0;
}
