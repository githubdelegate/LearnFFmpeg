//
//  zy_play_context.h
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef zy_play_context_h
#define zy_play_context_h

#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include "zy_packet_queue.h"
#include <SDL.h>


#define MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE   1024    //

#define FILE_NAME               "/home/isshe/Music/WavinFlag.aac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100
#define OUT_STREAM              stdout
#define WINDOW_W                640
#define WINDOW_H                320
#define ISSHE_REFRESH_EVENT     (SDL_USEREVENT + 1)
#define BREAK_EVENT               (SDL_USEREVENT + 2)
#define VIDEO_QUIT_EVENT         (SDL_USEREVENT + 3)
#define AUDIO_QUIT_EVENT        (SDL_USEREVENT + 4)
#define MAX_AUDIO_QUEUE_SIZE     128
#define MAX_VIDEO_QUEUE_SIZE     64
#define READ_FRAME_ERROR_TIMES     10         //连续没读到packet10次，认为没有了

typedef struct  ZYPlayerContext{
    //公共
    AVFormatContext    *pformat_ctx;
    char               filename[1024];
    int                quit;
    int                 player_state;
    /*
     SDL_Thread         *audio_decode_tid;
     SDL_Thread         *audio_tid;
     SDL_Thread         *video_decode_tid;
     SDL_Thread         *video_tid;
     */
    //音频
    int                audio_stream_index;
    AVStream           *paudio_stream;
    AVCodecContext     *paudio_codec_ctx;
    AVCodec            *paudio_codec;
    ZYPacketQueue      audio_packet_queue;
    uint8_t            audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int       audio_buf_size;
    unsigned int       audio_buf_index;
    int                 audio_quit;
    
    //视频
    int                video_stream_index;
    AVStream           *pvideo_stream;
    AVCodecContext     *pvideo_codec_ctx;
    AVCodec            *pvideo_codec;
    ZYPacketQueue       video_packet_queue;
    uint8_t            *video_buf;
    unsigned int       video_buf_size;
    unsigned int       video_buf_index;
    struct SwsContext  *psws_ctx;
    int                 video_quit;
    int                 zero_packet_count; //计算获取不到packet的次数，退出的依据
    
    int                pixel_w;
    int                pixel_h;
    int                window_w;
    int                window_h;
    
    SDL_Window         *pwindow;
    SDL_Renderer       *prenderer;
    SDL_Texture        *ptexture;
    SDL_Rect           sdl_rect;
    enum AVPixelFormat      pixfmt;
    //     AVFrame            frame;
    AVFrame            out_frame;
    
    // filter
    uint8_t change_fiter;
    uint8_t filte_key;
    
    //同步相关
    double             audio_clock;
    double             video_clock;
    double             pre_frame_pts;             //前一帧显示时间
    double             cur_frame_pts;             //packet.pts
    double             pre_cur_frame_delay;     //当前帧和前一帧的延时，前面两个相减的结果
    uint32_t            delay;
    double             frame_timer;
    
    //快进快退
    int                 seek_req;                 //request
    int                 seek_flags;             //向前向后之类的
    int64_t            seek_pos;                 //position，用double会不会更好点？
}ZYPlayerContext;
#endif /* zy_play_context_h */
