//
//  zy_packet_queue.c
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "zy_packet_queue.h"
#include <libavresample/avresample.h>
#include <libavcodec/avcodec.h>


int packet_queue_init(ZYPacketQueue *pq){
    memset(pq, 0, sizeof(ZYPacketQueue));
    pq->first_pkt = NULL;
    pq->last_pkt = NULL;
    pq->size = 0;
    pq->nb_packets = 0;
    pq->cond = SDL_CreateCond();
    pq->mutex = SDL_CreateMutex();
    return 0;
}
int packet_queue_get(ZYPacketQueue *pq,AVPacket *pkt){
    AVPacketList   *pkt_list = NULL;
    int            ret = -1;

    SDL_LockMutex(pq->mutex);
    while(1){
        pkt_list = pq->first_pkt;
        if (pkt_list != NULL)         //队不空，还有数据
        {
            pq->first_pkt = pq->first_pkt->next;    //pkt_list->next
            if (pq->first_pkt == NULL)
            {
                pq->last_pkt = NULL;
            }
            
            pq->nb_packets--;
            pq->size -= pkt_list->pkt.size;
            *pkt = pkt_list->pkt;          // 复制给packet。
            av_free(pkt_list);
            ret = 0;
            break;
        }
        else{
            SDL_CondWaitTimeout(pq->cond, pq->mutex, 100);
            ret = -1;
            break;
        }
    }
    SDL_UnlockMutex(pq->mutex);
    return ret;
}

int packet_queue_put(ZYPacketQueue *pq,AVPacket *pkt){
    AVPacketList   *pkt_list;
    if (av_dup_packet(pkt) < 0){
        return -1;
    }
    
    pkt_list = (AVPacketList *)av_malloc(sizeof(AVPacketList));
    if (pkt_list == NULL){
        return -1;
    }
    
    pkt_list->pkt   = *pkt;
    pkt_list->next  = NULL;
    
    //上锁
    SDL_LockMutex(pq->mutex);
    
    if (pq->last_pkt == NULL)    //空队
    {
        pq->first_pkt = pkt_list;
    }
    else
    {
        pq->last_pkt->next = pkt_list;
    }
    pq->last_pkt = pkt_list;  //这里queue->last_pkt = queue->last_pkt->next 的意思，但是，处理了更多的情况。
    pq->nb_packets++;
    pq->size += pkt->size;
    SDL_CondSignal(pq->cond);      //???
    SDL_UnlockMutex(pq->mutex);
    return 0;
}

void packet_queue_flush(ZYPacketQueue *queue)
{
    AVPacketList     *pkt = NULL;
    AVPacketList     *pkt1 = NULL;
    
    SDL_LockMutex(queue->mutex);
    
    for(pkt = queue->first_pkt; pkt != NULL; pkt = pkt1)
    {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    
    //packet_queue_init(queue);
    queue->first_pkt = NULL;
    queue->last_pkt = NULL;
    queue->nb_packets = 0;
    queue->size = 0;
    
    SDL_UnlockMutex(queue->mutex);
}
