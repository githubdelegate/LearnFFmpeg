//
//  MainPlayer.c
//  play
//
//  Created by zhangyun on 29/08/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "MainPlayer.h"


int playSync(char *file){

    PlayProies *ps = NULL;
    play_pro_init(ps);
    
    int ret = 0;
    
    av_register_all();
    
    ret = avformat_open_input(&ps->pForamtCtx, file, NULL, NULL);
    if (ret < 0) {
        printf("open input failed");
        return -1;
    }
    
    if (avformat_find_stream_info(ps->pForamtCtx, NULL) < 0){
        fprintf(stderr, "Couldn't find stream info\n");
        return -1;
    }
    
    ps->audioStreamIndex = -1;
    ps->videoStreamIndex = -1;
    for (int i =0 ; i < ps->pForamtCtx->nb_streams; i++) {
        if (ps->pForamtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && ps->audioStreamIndex < 0) {
            ps->audioStreamIndex = i;
        }

        if (ps->pForamtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && ps->videoStreamIndex < 0) {
            ps->videoStreamIndex = i;
        }
    }
    
    if (ps->audioStreamIndex < 0) {
        printf("audio stream index failed\n");
        return -1;
    }
    
    if (ps->videoStreamIndex < 0) {
        printf("video stream index failed\n");
        return -1;
    }
    
    av_dump_format(ps->pForamtCtx, 0, file, 0);
    
    //
    
    
    //
    
    
    
    
    
    
    
    return 0;
}




int play_pro_init(PlayProies *pp){
    memset(pp, 0, sizeof(PlayProies));
    pp->pForamtCtx = NULL;
    pp->audioStreamIndex = -1;
    pp->videoStreamIndex = -1;
    return 0;
}
