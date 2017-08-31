////
////  PlayPro.h
////  play
////
////  Created by zhangyun on 29/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#ifndef PlayPro_h
//#define PlayPro_h
//
//#include "PacketQueue.h"
//
//#define MAX_AUDIO_FRAME_SIZW 192000
//#define SDL_AUDIO_BUF_SIZE 1024
//
//typedef struct PlayProies{
//    
//    // common
//    AVFormatContext *pForamtCtx;
//    
//    // audio
//    AVCodecContext *pACodecCtx;
//    AVCodec *pACodec;
//    PacketQueue *audioQueue;
//    AVStream *pAudioStrream;
//    int audioStreamIndex;
//    uint8_t audioBuf[MAX_AUDIO_FRAME_SIZW * 3 / 2];
//    unsigned int audioBufIndex;
//    unsigned int audioBufSize; //
//    AVFrame wanted_frame;
//    
//    // video
//    AVCodecContext *pVCodecCtx;
//    AVCodec *pVCodec;
//    AVStream *pVideoStream;
//    PacketQueue *videoQueue;
//    int videoStreamIndex;
//    int videoBufIndex;
//    
//    int pixel_w;
//    int pixel_h;
//    int window_w;
//    int window_h;
//    
//    SDL_Window *pWindow;
//    SDL_Renderer *pRender;
//    SDL_Texture *pTexture;
//    
//    // sync
//    
//    double audio_clock;
//    double video_clock;
//    double pre_frame_pts;
//    double cur_frame_pts;
//    double pre_cur_frame_deplay;
//    uint32_t delay;
//    double frame_timer;
//}PlayProies;
//
//
//#endif /* PlayPro_h */
