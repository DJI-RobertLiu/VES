/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#import "EAGLView.h"
#import "ES2Renderer.h"

#include "vesKiwiViewerApp.h"

#define USE_DEPTH_BUFFER 1

@interface kwGestureDelegate : NSObject <UIGestureRecognizerDelegate>{
  
}
@end

@implementation kwGestureDelegate

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
  return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
  BOOL rotating2D = 
  [gestureRecognizer isMemberOfClass:[UIRotationGestureRecognizer class]] ||
  [otherGestureRecognizer isMemberOfClass:[UIRotationGestureRecognizer class]];
  
  BOOL pinching = 
  [gestureRecognizer isMemberOfClass:[UIPinchGestureRecognizer class]] ||
  [otherGestureRecognizer isMemberOfClass:[UIPinchGestureRecognizer class]];
  
  BOOL panning = 
  [gestureRecognizer numberOfTouches] == 2 &&
  ([gestureRecognizer isMemberOfClass:[UIPanGestureRecognizer class]] ||
   [otherGestureRecognizer isMemberOfClass:[UIPanGestureRecognizer class]]);
  
  if ((pinching && panning) ||
      (pinching && rotating2D) ||
      (panning && rotating2D))
    {
    return YES;
    }
  return NO;
}
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
  return YES;
}
@end

@interface EAGLView ()
@property (nonatomic, retain) EAGLContext *context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLView

@synthesize context;

// You must implement this method
+ (Class)layerClass
{
  return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
  self = [super initWithCoder:coder];
  if (self)
    {
    // Get the layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    eaglLayer.opaque = TRUE;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (!context || ![EAGLContext setCurrentContext:context])
      {
      [self release];
      return nil;
      }
    
    renderer = [[ES2Renderer alloc] init];
    
    if (!renderer)
      {
			[self release];
			return nil;
      }

    [self createGestureRecognizers];
    self.multipleTouchEnabled = YES;
    
    self->rotationDataLock = [NSLock new];
    self->accumulatedRotationDelta.x = 0.0;
    self->accumulatedRotationDelta.y = 0.0;
    }
  
  self->shouldRender = NO;
  self->recentRenderFPS = [NSMutableArray new];
  
  return self;
}

-(struct vesKiwiViewerApp*) getApp
{
  return self->renderer.app;
}

- (void)layoutSubviews 
{
  [EAGLContext setCurrentContext:context];
  
  if (self->displayLink)
  {
    [self->displayLink invalidate];
  }

  [self destroyFramebuffer];
  [self createFramebuffer];
  [renderer resizeFromLayer:backingWidth height:backingHeight];

  // Reposition buttons on the iPhone
  if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
    {
    UIButton* resetButton = [[self subviews] objectAtIndex:0];
    resetButton.frame = CGRectMake((backingWidth / 2) - (resetButton.frame.size.width / 2),
                                   backingHeight - 50,
                                   resetButton.frame.size.width, resetButton.frame.size.height);

    UIButton* openDataButton = [[self subviews] objectAtIndex:1];
    openDataButton.frame = CGRectMake(backingWidth - 90, backingHeight - 45,
                                      openDataButton.frame.size.width, openDataButton.frame.size.height);

    UIButton* infoButton = [[self subviews] objectAtIndex:2];
    infoButton.frame = CGRectMake(backingWidth - 36, backingHeight - 38,
                                  infoButton.frame.size.width, infoButton.frame.size.height);
    }
  //
  // set up animation loop
  self->displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(drawView:)];
  [self->displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(rotate) name:@"willRender" object:nil];
  
  [self forceRender];
}

- (BOOL)createFramebuffer 
{  
  glGenFramebuffers(1, &viewFramebuffer);
  glGenRenderbuffers(1, &viewRenderbuffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
  [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
  
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
  
  if (USE_DEPTH_BUFFER)
    {
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    }
  
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
    NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    return NO;
    }

  return YES;
}

- (void)destroyFramebuffer 
{  
  glDeleteFramebuffers(1, &viewFramebuffer);
  viewFramebuffer = 0;
  glDeleteRenderbuffers(1, &viewRenderbuffer);
  viewRenderbuffer = 0;
  
  if(depthRenderbuffer)
    {
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
    }
}

- (void) updateRefreshRate:(float)lastRenderFPS
{
  //
  // ignore the call if there is no animation loop
  if (!self->displayLink)
    {
    return;
    }
  
  //
  // keep track of the last few rendering speeds
  const unsigned int maxWindowSize = 20;
  [self->recentRenderFPS addObject:[NSNumber numberWithFloat:lastRenderFPS]];
  if ([self->recentRenderFPS count] > maxWindowSize)
    {
    [self->recentRenderFPS removeObjectAtIndex:0];
    }
  float sumFPS = 0.0;
  for (NSNumber* n in self->recentRenderFPS)
    {
    sumFPS += n.floatValue;
    }
  float meanFPS = sumFPS / maxWindowSize;

  //
  // set forward refresh rate to match current rendering speed
  // (round up to be conservative)
  int desiredFrameInterval = static_cast<int>(60.0 / meanFPS) + 1;

  //
  // clamp to 10Hz or higher
  desiredFrameInterval = desiredFrameInterval > 6 ? 6 : desiredFrameInterval;
  
  if (desiredFrameInterval != self->displayLink.frameInterval)
    {
    //NSLog(@"Changing frame interval to %d", desiredFrameInterval);
    [self->displayLink setFrameInterval:desiredFrameInterval];
    }
}

- (int)currentRefreshRate
{
  if (!self->displayLink)
    {
    return 0;
    }
  return 60 / self->displayLink.frameInterval;
}

- (void) scheduleRender
{
  self->shouldRender = YES;
}

- (void) forceRender
{
  [self scheduleRender];
  [self drawView:nil];
}

- (void)drawView:(id) sender
{    
  if (TRUE || self->shouldRender)
    {
    NSDate* startRenderTotalDate = [NSDate date];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"willRender" object:nil];
    NSDate* startRenderOnlyDate = [NSDate date];
    [EAGLContext setCurrentContext:context];
    glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
    [renderer render];
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER];
    self->shouldRender = NO;
    float currentFPS = 1.0 / [[NSDate date] timeIntervalSinceDate:startRenderOnlyDate];
    //NSLog(@"Render Only @ %4.1f fps", currentFPS);
    currentFPS = 1.0 / [[NSDate date] timeIntervalSinceDate:startRenderTotalDate];
    //NSLog(@"Total Render @ %4.1f fps", currentFPS);
    [self updateRefreshRate:currentFPS];
    }
}

- (void)dealloc
{
  if ([EAGLContext currentContext] == context)
    {
    [EAGLContext setCurrentContext:nil];
    }
  [context release];
  context = nil;
  [renderer release];
  [self->rotationDataLock release];
  self->rotationDataLock = nil;
  [super dealloc];
}

- (void)resetView
{
  [self stopInertialMotion]; 
  [renderer resetView];
  [self scheduleRender];
}

- (void) setFilePath :(NSString *) fpath
{
	if(renderer)
    {
		[renderer setFilePath:fpath];
    [self resetView];
	  }
}


#pragma mark -
#pragma mark Touch handling

- (void) createGestureRecognizers
{
  UIPanGestureRecognizer *singleFingerPanGesture = [[UIPanGestureRecognizer alloc]
                                                    initWithTarget:self action:@selector(handleSingleFingerPanGesture:)];
  [singleFingerPanGesture setMinimumNumberOfTouches:1];
  [singleFingerPanGesture setMaximumNumberOfTouches:1];
  [self addGestureRecognizer:singleFingerPanGesture];
  [singleFingerPanGesture release];
  
  UIPanGestureRecognizer *doubleFingerPanGesture = [[UIPanGestureRecognizer alloc]
                                                    initWithTarget:self action:@selector(handleDoubleFingerPanGesture:)];
  [doubleFingerPanGesture setMinimumNumberOfTouches:2];
  [doubleFingerPanGesture setMaximumNumberOfTouches:2];
  [self addGestureRecognizer:doubleFingerPanGesture];
  [doubleFingerPanGesture release];
  
  UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc]
                                            initWithTarget:self action:@selector(handlePinchGesture:)];
  [self addGestureRecognizer:pinchGesture];
  [pinchGesture release];
  
  UIRotationGestureRecognizer *rotate2DGesture = [[UIRotationGestureRecognizer alloc]
                                                  initWithTarget:self action:@selector(handle2DRotationGesture:)];
  [self addGestureRecognizer:rotate2DGesture];
  [rotate2DGesture release];
  
  UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc]
                                             initWithTarget:self action:@selector(handleTapGesture:)];
  // this is needed so that the buttons on top of the render view will 
  // work since this is the first responder---is this the best way to 
  // fix this problem?
  tapGesture.cancelsTouchesInView = NO;
  [self addGestureRecognizer:tapGesture];
  [tapGesture release];

  UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc]
                                             initWithTarget:self action:@selector(handleDoubleTapGesture:)];
  [doubleTapGesture setNumberOfTapsRequired:2];
  [self addGestureRecognizer:doubleTapGesture];
  [doubleTapGesture release];

  //
  // allow two-finger gestures to work simultaneously
  kwGestureDelegate* gestureDelegate = [[kwGestureDelegate alloc] init];
  [rotate2DGesture setDelegate:gestureDelegate];
  [pinchGesture setDelegate:gestureDelegate];
  [doubleFingerPanGesture setDelegate:gestureDelegate];
}

- (IBAction)handleDoubleFingerPanGesture:(UIPanGestureRecognizer *)sender
{
  if (sender.state == UIGestureRecognizerStateEnded ||
      sender.state == UIGestureRecognizerStateCancelled)
    {
    // start inertial pan?
    return;
    }
  
  [self stopInertialMotion];
    
  //
  // get current translation and (then zero it out so it won't accumulate)
  CGPoint currentLocation = [sender locationInView:self];
  CGPoint currentTranslation = [sender translationInView:self];
  [sender setTranslation:CGPointZero inView:self];
  
  //
  // compute the previous location (have to flip y)
  CGPoint previousLocation;
  previousLocation.x = currentLocation.x - currentTranslation.x;
  previousLocation.y = currentLocation.y + currentTranslation.y;
  
  self->renderer.app->handleTwoTouchPanGesture(previousLocation.x, previousLocation.y, currentLocation.x, currentLocation.y);

  [self scheduleRender];
}

- (IBAction)handleSingleFingerPanGesture:(UIPanGestureRecognizer *)sender
{ 
  if (sender.state == UIGestureRecognizerStateEnded ||
      sender.state == UIGestureRecognizerStateCancelled)
    {
    bool widgetInteractionActive = self->renderer.app->widgetInteractionIsActive();

    self->renderer.app->handleSingleTouchUp();
    
    // clear any pending rotation events
    self->accumulatedRotationDelta.x = 0.0;
    self->accumulatedRotationDelta.y = 0.0;
    
    [self scheduleRender];
    if (!widgetInteractionActive && lastRotationMotionNorm > 4.0f)
      {
      self->inertialRotationThread = [[NSThread alloc] initWithTarget:self selector:@selector(handleInertialRotation) object:nil];
      [inertialRotationThread start];
      }
    return;
    }
  
  [self stopInertialMotion];
  
  //
  // get current translation and (then zero it out so it won't accumulate)
  CGPoint currentTranslation = [sender translationInView:self];
  CGPoint currentLocation = [sender locationInView:self];
  [sender setTranslation:CGPointZero inView:self];

  if (sender.state == UIGestureRecognizerStateBegan)
    {
    self->renderer.app->handleSingleTouchDown(currentLocation.x, currentLocation.y);
    [self scheduleRender];
    }
  
  // 
  // update data for inertial rotation
  self->lastRotationMotionNorm = sqrtf(currentTranslation.x*currentTranslation.x + 
                                       currentTranslation.y*currentTranslation.y);
  if (self->lastRotationMotionNorm > 0)
    {
    self->lastMovementXYUnitDelta.x = currentTranslation.x / lastRotationMotionNorm;
    self->lastMovementXYUnitDelta.y = currentTranslation.y / lastRotationMotionNorm;
    
    //
    // apply the rotation and rerender
    [self scheduleRotate:currentTranslation];
    [self scheduleRender];
    }
  else
    {
    self->lastMovementXYUnitDelta.x = 0.0f;
    self->lastMovementXYUnitDelta.y = 0.0f;
    }
}

- (IBAction)handlePinchGesture:(UIPinchGestureRecognizer *)sender
{  
  if (sender.state == UIGestureRecognizerStateEnded ||
      sender.state == UIGestureRecognizerStateCancelled)
    {
    return;
    }
  
  [self stopInertialMotion];
  
  self->renderer.app->handleTwoTouchPinchGesture(sender.scale);
  
  //
  // reset scale so it won't accumulate
  sender.scale = 1.0;
  
  [self scheduleRender];
}

- (IBAction)handle2DRotationGesture:(UIRotationGestureRecognizer *)sender
{  
  if (sender.state == UIGestureRecognizerStateEnded ||
      sender.state == UIGestureRecognizerStateCancelled)
    {
    return;
    }

    [self stopInertialMotion];

  self->renderer.app->handleTwoTouchRotationGesture(sender.rotation);
  
  //
  // reset rotation so it won't accumulate
  [sender setRotation:0.0];
  
  [self scheduleRender];
}

- (IBAction)handleDoubleTapGesture:(UITapGestureRecognizer *)sender
{
  self->renderer.app->handleDoubleTap();
  [self scheduleRender];
}

- (IBAction)handleTapGesture:(UITapGestureRecognizer *)sender
{
  CGPoint currentLocation = [sender locationInView:self];
  self->renderer.app->handleSingleTouchTap(currentLocation.x, currentLocation.y);
  [self stopInertialMotion];
  [self scheduleRender];
}

-(void)scheduleRotate:(CGPoint)delta
{
  [self->rotationDataLock lock];
  self->accumulatedRotationDelta.x += delta.x;
  self->accumulatedRotationDelta.y += delta.y;
  [self->rotationDataLock unlock];
}

- (void)rotate
{
  [self->rotationDataLock lock];
  if (self->accumulatedRotationDelta.x != 0.0 ||
      self->accumulatedRotationDelta.y != 0.0)
  {
    self->renderer.app->handleSingleTouchPanGesture(self->accumulatedRotationDelta.x, 
                                                    self->accumulatedRotationDelta.y); 
    self->accumulatedRotationDelta.x = 0.0;
    self->accumulatedRotationDelta.y = 0.0;
  }
  [self->rotationDataLock unlock];
}

- (void)handleInertialRotation
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  CGPoint delta;
  while (lastRotationMotionNorm > 0.5)
    {
    [NSThread sleepForTimeInterval:1/30.0];
    
    if ([[NSThread currentThread] isCancelled])
      {
      break;
      }
    
    delta.x = lastRotationMotionNorm*lastMovementXYUnitDelta.x;
    delta.y = lastRotationMotionNorm*lastMovementXYUnitDelta.y;
    [self scheduleRotate:delta];
    
    [self scheduleRender];
    lastRotationMotionNorm *= 0.9;
    }
  lastRotationMotionNorm = 0;
  [pool release];
  //[NSThread exit];
}

- (void) stopInertialMotion
{
  if (inertialRotationThread)
    {
    [inertialRotationThread setThreadPriority:1.0f];
    [inertialRotationThread cancel];
    
    // TODO: something is wrong with this implementation so there is a hack here. 
    // This busy wait is causing a jerky stop to the inertial rotation so I'm setting the 
    // priorty of the inertia thread up high and pausing here a bit so that it can finish 
    // its render.  The priority and sleep should be removed when the wait is improved
    [NSThread sleepForTimeInterval:1/20.0];
    while (![inertialRotationThread isFinished])
      {
      // busy wait for the thread to exit
      }
    [inertialRotationThread release];
    inertialRotationThread = nil;
    }
}

#pragma mark -
#pragma mark Model information


-(int) getNumberOfFacetsForCurrentModel
{
  if (renderer)
    {
    return [renderer getNumberOfFacetsForCurrentModel];
    }
  return 0;
}

-(int) getNumberOfLinesForCurrentModel
{
  if (renderer)
    {
    return [renderer getNumberOfLinesForCurrentModel];
    }
  return 0;
}


-(int) getNumberOfVerticesForCurrentModel
{
  if (renderer)
    {
    return [renderer getNumberOfVerticesForCurrentModel];
    }
  return 0;
}



@end
