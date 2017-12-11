#ifndef FLUTTER_FLUTTERTESTSURFACE_H_
#define FLUTTER_FLUTTERTESTSURFACE_H_

#import <QuartzCore/CAEAGLLayer.h>
#include "FlutterMacros.h"


FLUTTER_EXPORT
@interface Surface : NSObject
@property(readonly, nonatomic) CAEAGLLayer* layer;
-(id)init;
-(void)updateStorage;
-(void)draw:(NSString*)text;
@end

#endif
