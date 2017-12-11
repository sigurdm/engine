#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/CAEAGLLayer.h>
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"
#include "third_party/skia/include/gpu/GrContextOptions.h"
#include "third_party/skia/include/gpu/gl/GrGLInterface.h"


#include "flutter/shell/platform/darwin/ios/framework/Headers/FlutterTestSurface.h"

#define VERIFY(x) if (!(x)) NSLog(@"Fail! %d", __LINE__);

@implementation Surface
GLuint framebuffer = GL_NONE;
GLuint colorbuffer = GL_NONE;
EAGLContext *context;

 -(void)updateStorage{
     VERIFY([EAGLContext setCurrentContext:context]);

     VERIFY(glGetError() == GL_NO_ERROR);

     glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

     glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
     VERIFY(glGetError() == GL_NO_ERROR);

     VERIFY([context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_layer]);


     VERIFY(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
 }


 -(id)init{
     self = [super init];
     context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
     VERIFY([EAGLContext setCurrentContext:context]);

     glGenFramebuffers(1, &framebuffer);
     VERIFY(glGetError() == GL_NO_ERROR);
     VERIFY(framebuffer != GL_NONE);
     glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
     VERIFY(glGetError() == GL_NO_ERROR);
     glGenRenderbuffers(1, &colorbuffer);
     VERIFY(colorbuffer != GL_NONE);

     glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
     VERIFY(glGetError() == GL_NO_ERROR);

     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorbuffer);
     VERIFY(glGetError() == GL_NO_ERROR);

     _layer = [[CAEAGLLayer alloc] init];
     NSString* drawableColorFormat = kEAGLColorFormatRGBA8;
     _layer.drawableProperties =
     @{
       kEAGLDrawablePropertyColorFormat : drawableColorFormat,
       kEAGLDrawablePropertyRetainedBacking : @(NO),
       };
     return self;
 }

-(void)draw:(NSString*)text {
  VERIFY([EAGLContext setCurrentContext:context]);
  const CGSize layer_size = [_layer frame].size;

  const GrGLFramebufferInfo framebuffer_info = {
      .fFBOID = framebuffer,
  };

  const GrPixelConfig pixel_config = kRGBA_8888_GrPixelConfig;

  GrBackendRenderTarget render_target(layer_size.width,      // width
                                      layer_size.height,     // height
                                      0,                // sample count
                                      0,                // stencil bits (TODO)
                                      pixel_config,     // pixel config
                                      framebuffer_info  // framebuffer info
  );

  GrContextOptions options;
   options.fAvoidStencilBuffers = true;

  sk_sp<SkColorSpace> colorspace = nullptr;

  SkSurfaceProps surface_props(
      SkSurfaceProps::InitType::kLegacyFontHost_InitType);


  auto backend_context =
       reinterpret_cast<GrBackendContext>(GrGLCreateNativeInterface());

  auto skContext = sk_sp<GrContext>(
        GrContext::Create(kOpenGL_GrBackend, backend_context, options));

  auto surface = SkSurface::MakeFromBackendRenderTarget(
        skContext.get(),                                       // gr context
        render_target,                                 // render target
        GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin,  // origin
        colorspace,                                    // colorspace
        &surface_props                                 // surface properties
    );

  auto canvas = surface->getCanvas();

  SkPaint paint;
  paint.setColor(0xFFFF0000);

  canvas->clear(0xFFFFFFFF);

  canvas->drawString([text cStringUsingEncoding:[NSString defaultCStringEncoding]], 10, 10, paint);

  canvas->flush();

  [[EAGLContext currentContext] presentRenderbuffer:GL_RENDERBUFFER];
}

@end

