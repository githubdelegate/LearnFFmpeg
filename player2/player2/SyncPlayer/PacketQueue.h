//
//  PacketQueue.h
//  player2
//
//  Created by zhangyun on 29/08/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef PacketQueue_h
#define PacketQueue_h

#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>
#include <time.h>


typedef struct TPacketQueue{
    
    AVPacketList *firstList;

}TPacketQueue;

#endif /* PacketQueue_h */
