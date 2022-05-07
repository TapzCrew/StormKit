extern int macosMain(const int argc, const char **argv);

#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

int main(const int argc, const char **argv) {
    [[NSFileManager defaultManager]
        changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];

    return macosMain(argc, argv);
}
