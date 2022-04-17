extern int main(int argc, char **argv);

#include <Windows.h>

#include <cstdlib>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    auto argc  = __argc;
    auto &argv = __argv;

    return main(argc, argv);
}
