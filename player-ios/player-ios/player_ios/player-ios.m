//
//  player-ios.c
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include <stdio.h>
#include "zy__sync_player.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <SDL_main.h>

int main(int argc, char * argv[]) {

    
    UIWindow *myWindow = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    UIViewController *vc = [[UIViewController alloc] init];
    vc.view.frame = [UIScreen mainScreen].bounds;
    vc.view.backgroundColor = [UIColor redColor];
    myWindow.rootViewController = vc;
    [myWindow makeKeyAndVisible];
    
    
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *file = [[NSBundle mainBundle] pathForResource:@"output.mp4" ofType:nil];
        const char *fileP = [file cStringUsingEncoding:NSUTF8StringEncoding];
        playSync7(fileP);
    });


    //    [UIApplication sharedApplication].keyWindow = myWindow;
    


    
    return 0;
}

