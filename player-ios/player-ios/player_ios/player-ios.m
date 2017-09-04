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
#include <SDL_main.h>

int main(int argc, char * argv[]) {
    
    NSString *file = [[NSBundle mainBundle] pathForResource:@"output.mp4" ofType:nil];
    
    const char *fileP = [file cStringUsingEncoding:NSUTF8StringEncoding];
    playSync7(fileP);
        return 0;
}

