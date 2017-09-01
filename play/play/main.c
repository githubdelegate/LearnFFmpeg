//
//  main.c
//  play
//
//  Created by zhangyun on 29/08/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include <stdio.h>
#include "player7.h"
#include "device.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
//    playSync7();
    device_main(0, NULL);
    return 0;
}
