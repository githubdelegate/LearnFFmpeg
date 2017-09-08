//
//  ViewController.m
//  opengl-ios
//
//  Created by zhangyun on 05/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#import "ViewController.h"
#import <GLKit/GLKit.h>

@interface ViewController ()
@property (nonatomic,strong) GLKView  *glView;
@property (nonatomic,strong) EAGLContext  *eactx;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    self.glView = [[GLKView alloc] initWithFrame:self.view.bounds];
    self.eactx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    self.glView.context = self.eactx;
    self.glView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    
    [EAGLContext setCurrentContext:self.eactx];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
