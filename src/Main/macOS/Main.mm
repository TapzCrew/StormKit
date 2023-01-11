extern int macosMain(const int argc, const char **argv);

#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

auto main(const int argc, const char **argv) -> int {
    [[NSFileManager defaultManager]
        changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];

    return macosMain(argc, argv);
}
