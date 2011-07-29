//
//  SceneController.mm
//

#import "SceneController.h"
#import "ESRenderView.h"
#import "ESRenderer.h"

#include "vesActor.h"
#include "vesMapper.h"
#include "vesRenderer.h"
#include "vesPolyDataToTriangleData.h"

#include "vtkSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkSphereSource.h"
#include "vtkTubeFilter.h"

//------------------------------------------------------------------------------
class SceneControllerImpl
{
public:

  NSMutableArray* Actors;
};

//------------------------------------------------------------------------------
@interface SceneController ()
-(vesActor*)addActorWithData:(vesTriangleData*)data;
-(void)cleanupActors;
@end

@implementation SceneController
@synthesize renderView;


//------------------------------------------------------------------------------
- (id)init
{
  self = [super init];
  if (self)
    {
    self->Internal = new SceneControllerImpl;
    self->Internal->Actors = [NSMutableArray new];
    }
  return self;
}

//------------------------------------------------------------------------------
- (void)dealloc
{
  [self cleanupActors];
  [self->Internal->Actors release];
  delete self->Internal;
  [super dealloc];
}

//------------------------------------------------------------------------------
-(vesActor*)addActorWithData:(vesTriangleData*)data
{
  vesMapper* mapper = new vesMapper();
  mapper->SetTriangleData(data);
  vesActor* actor = new vesActor([self.renderView.renderer getShader], mapper);
  [self.renderView.renderer getRenderer]->AddActor(actor);
  [self->Internal->Actors addObject:[NSValue valueWithPointer:actor]];
  return actor;
}

//------------------------------------------------------------------------------
-(void)cleanupActors
{
  for (NSValue* actorPointerValue in self->Internal->Actors)
    {
    vesActor* tempActor = (vesActor*)[actorPointerValue pointerValue];
    if (self->renderView)
      {
      [self->renderView.renderer getRenderer]->RemoveActor(tempActor);
      delete tempActor->GetMapper()->GetTriangleData();
      delete tempActor->GetMapper();
      delete tempActor;
      }
    }
  [self->Internal->Actors removeAllObjects];
}

//------------------------------------------------------------------------------
-(vesTriangleData*)makeSphereWithCenter:(double[3])center Radius:(double)radius
{
  vtkSmartPointer<vtkSphereSource> cone = vtkSmartPointer<vtkSphereSource>::New();
  cone->SetCenter(center);
  cone->SetRadius(radius);
  cone->SetPhiResolution(24);
  cone->SetThetaResolution(24);
  cone->Update();
  return vesPolyDataToTriangleData::Convert(cone->GetOutput());
}

//------------------------------------------------------------------------------
-(vesTriangleData*)makeConeWithCenter:(double[3])center Scale:(double)scale
{
  vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetCenter(center);
  cone->SetDirection(0,1,0);
  cone->SetRadius(cone->GetRadius()*scale);
  cone->SetHeight(cone->GetHeight()*scale);
  cone->SetResolution(24);
  cone->Update();
  return vesPolyDataToTriangleData::Convert(cone->GetOutput());
}

//------------------------------------------------------------------------------
-(vesTriangleData*)makeTubeWithEndPoints:(double[6])endPoints Radius:(double)radius
{
  vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(endPoints);
  line->SetPoint2(endPoints+3);
  vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
  tube->SetInputConnection(line->GetOutputPort());
  tube->SetNumberOfSides(24);
  tube->SetRadius(radius);
  tube->Update();
  return vesPolyDataToTriangleData::Convert(tube->GetOutput());
}

//------------------------------------------------------------------------------
-(void)initializeScene
{
  vesActor* actor;

  double endPoints[6] = {-100,-200,200,  100,-200,200};

  actor = [self addActorWithData:[self makeTubeWithEndPoints:endPoints Radius:10]];
  actor->SetColor(0.8, 0, 0, 1.0);

  actor = [self addActorWithData:[self makeSphereWithCenter:endPoints Radius:20]];
  actor->SetColor(0, 0.8, 0, 1.0);

  actor = [self addActorWithData:[self makeSphereWithCenter:endPoints+3 Radius:20]];
  actor->SetColor(0, 0, 0.8, 1.0);

  [self resetView];
}


//------------------------------------------------------------------------------
-(void)resetView
{
  vesMultitouchCamera* camera = [self->renderView.renderer getCamera];

  // reset the camera
  camera->Reset();

  // set the camera orientation
  double angleAxis[4] = {-M_PI_2, 0, 0, 1};
  camera->RotateAngleAxis(angleAxis[0], angleAxis[1], angleAxis[2], angleAxis[3]);

  // todo-
  // set camera focal point to [-100,-200,200] so the green sphere is centered in the view
  // set camera position to be [-100,-200,200] + X*[0,0,-1] so that the red cylinder is perpendicular to the view direction

  [self->renderView drawView:nil];
}

@end
