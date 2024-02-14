#import <UIKit/UIKit.h>

auto main(int argc, char *argv[]) -> int {
    stormkit::core::backtraceInit(argv[0]);
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, @"AppDelegate");
    }
}
