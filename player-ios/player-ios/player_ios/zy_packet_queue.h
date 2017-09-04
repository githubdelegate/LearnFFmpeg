//
//  zy_packet_queue.h
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef zy_packet_queue_h
#define zy_packet_queue_h

#include <stdio.h>
#include <libavformat/avformat.h>
#include <SDL.h>

typedef struct ZYPacketQueue{
    AVPacketList *first_pkt;
    AVPacketList *last_pkt;
    
    int size;
    int nb_packets;
    
    SDL_cond *cond;
    SDL_mutex *mutex;
    
}ZYPacketQueue;



int packet_queue_init(ZYPacketQueue *pq);
int packet_queue_get(ZYPacketQueue *pq,AVPacket *pkt);
int packet_queue_put(ZYPacketQueue *pq,AVPacket *pkt);
void packet_queue_flush(ZYPacketQueue *queue);
#endif /* zy_packet_queue_h */
