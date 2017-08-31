////
////  PacketQueue.c
////  play
////
////  Created by zhangyun on 29/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "PacketQueue.h"
//
//void packet_queue_init(PacketQueue *pq){
//    memset(pq, 0, sizeof(PacketQueue));
//    pq->muter = SDL_CreateMutex();
//    pq->cond = SDL_CreateCond();
//}
//
//int packet_queue_put(PacketQueue *pq,AVPacket *pkt){
//    AVPacketList *pkt1 = NULL;
//    AVPacket *copyPkt = av_malloc(sizeof(AVPacket));
//    if (av_copy_packet(copyPkt, pkt) < 0) {
//        return -1;
//    }
//    pkt1 = av_malloc(sizeof(AVPacketList));
//    if (!pkt1) {
//        return -1;
//    }
//    pkt1->pkt = *copyPkt;
//    pkt1->next = NULL;
//    
//    //Lock the mutex.
//    SDL_LockMutex(pq->muter);
//    
//    if (!pq->last_pkt) { // 说明是空链表
//        pq->first_pkt = pkt1;
//    }else{
//        pq->last_pkt->next = pkt1;
//    }
//    
//    pq->last_pkt = pkt1;
//    pq->nb_packet++;
//    pq->size += pkt1->pkt.size;
//    //Restart one of the threads that are waiting on the condition variable.
//    SDL_CondSignal(pq->cond);
//    SDL_UnlockMutex(pq->muter);
//    return 0;
//}
//
//int packet_queue_get(PacketQueue *pq,AVPacket *pkt,int block){
//    AVPacketList *pkt1;
//    int ret;
//    SDL_LockMutex(pq->muter);
//    
//    for (;;) { // 死循环
//        //            if (quit) {
//        //                ret = -1;
//        //                break;
//        //            }
//        
//        pkt1 = pq->first_pkt;
//        if (pkt1) {
//            pq->first_pkt = pkt1->next;
//            if (!pq->first_pkt) { // 只有一个node
//                pq->last_pkt = NULL;
//            }
//            
//            pq->nb_packet--;
//            pq->size -= pkt1->pkt.size;
//            *pkt = pkt1->pkt;
//            av_free(pkt1);
//            ret = 1;
//            break;
//        }else if(!block){ // 是否阻塞等待 q 中来了数据
//            ret = 0;
//            break;
//        }else{
//            SDL_CondWait(pq->cond, pq->muter);
//        }
//    } // for
//    
//    SDL_UnlockMutex(pq->muter);
//    return ret;
//};
