//
//  GLViewController.m
//  gl
//
//  Created by zhangyun on 06/09/2017.
//  Copyright (c) 2017 ___FULLUSERNAME___. All rights reserved.
//

#import "GLViewController.h"
#import "ConstantsAndMacros.h"
#import "OpenGLCommon.h"
@implementation GLViewController

- (void)loadView
{
    GLView *viewGL = [[GLView alloc] init];
    viewGL.delegate = self;
    self.view = viewGL;
}


#pragma mark - GLViewDelegate

- (void)drawView:(UIView *)theView
{
//    Draw code here
    static      GLfloat rotation = 0.0;

    Vertex3D    vertex1 = Vertex3DMake(0.0, 1.0, -3.0);
    Vertex3D    vertex2 = Vertex3DMake(1.0, 0.0, -3.0);
    Vertex3D    vertex3 = Vertex3DMake(-1.0, 0.0, -3.0);
    Triangle3D  triangle = Triangle3DMake(vertex1, vertex2, vertex3);

    glLoadIdentity();
    glRotatef(rotation, 0.0, 0.0, 1.0);
//    glClearColor(0.7, 0.7, 0.7, 1.0);
    glClearColor(1, 1, 1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glVertexPointer(3, GL_FLOAT, 0, &triangle);
    glDrawArrays(GL_TRIANGLES, 0, 9);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    rotation+= 0.5;
}

- (void)setupView:(GLView*)view
{
	const GLfloat zNear = 0.01, zFar = 1000.0, fieldOfView = 45.0;
	GLfloat size;
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	size = zNear * tanf(DEGREES_TO_RADIANS(fieldOfView) / 2.0);
	CGRect rect = view.bounds;
	glFrustumf(-size, size, -size / (rect.size.width / rect.size.height), size / 
			   (rect.size.width / rect.size.height), zNear, zFar);
	glViewport(0, 0, rect.size.width, rect.size.height);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
}
@end
