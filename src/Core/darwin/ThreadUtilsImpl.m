#import "ThreadUtils.h"

#import <Foundation/NSThread.h>
#import <Foundation/NSString.h>

////////////////////////////////////////
////////////////////////////////////////
void setThreadName(NSThread *id, const char *name) {
    [id setName: [NSString stringWithCString: name]];
}

////////////////////////////////////////
////////////////////////////////////////
void setCurrentThreadName(const char *name) {
    NSThread *id = [NSThread currentThread];
    setThreadName(id, name);
}