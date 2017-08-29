//
//  PacketQueue.h
//  play
//
//  Created by zhangyun on 29/08/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef PacketQueue_h
#define PacketQueue_h


#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

typedef struct PacketQueue{
    AVPacketList *first_pkt;
    AVPacketList *last_pkt;
    int nb_packet;
    int size;
    SDL_mutex *muter;
    SDL_cond *cond;
}PacketQueue;

int packet_queue_put(PacketQueue *pq,AVPacket *pkt);

int packet_queue_get(PacketQueue *pq,AVPacket *pkt,int block);

void packet_queue_init(PacketQueue *pq);


#endif /* PacketQueue_h */
