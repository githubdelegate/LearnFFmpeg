//
//  device.c
//  play
//
//  Created by zhangyun on 01/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "device.h"


#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include <libavutil/imgutils.h>
#include "SDL/SDL.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
#include <SDL2/SDL.h>
#ifdef __cplusplus
};
#endif
#endif

//Output YUV420P
#define OUTPUT_YUV420P 0
//'1' Use Dshow
//'0' Use VFW
#define USE_DSHOW 0


//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

int thread_exit=0;

int sfp_refresh_thread(void *opaque)
{
    while (thread_exit==0) {
        SDL_Event event;
        event.type = SFM_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    return 0;
}


//Show Dshow Device
void show_dshow_device(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    printf("========Device Info=============\n");
    avformat_open_input(&pFormatCtx,"video=dummy",iformat,&options);
    printf("================================\n");
}

//Show Dshow Device Option
void show_dshow_device_option(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_options","true",0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    printf("========Device Option Info======\n");
    avformat_open_input(&pFormatCtx,"video=Integrated Camera",iformat,&options);
    printf("================================\n");
}

//Show VFW Device
void show_vfw_device(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVInputFormat *iformat = av_find_input_format("vfwcap");
    printf("========VFW Device Info======\n");
    avformat_open_input(&pFormatCtx,"list",iformat,NULL);
    printf("=============================\n");
}

//Show AVFoundation Device
void show_avfoundation_device(){
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    printf("==AVFoundation Device Info===\n");
    avformat_open_input(&pFormatCtx,"",iformat,&options);
    printf("=============================\n");
}


int device_main(int argc, char* argv[])
{
    
    AVFormatContext	*pFormatCtx;
    int				i, videoindex;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    
    //Open File
    //char filepath[]="src01_480x272_22.h265";
    //avformat_open_input(&pFormatCtx,filepath,NULL,NULL)
    
    //Register Device
    avdevice_register_all();
    
    //Windows
#ifdef _WIN32
    
    //Show Dshow Device
    show_dshow_device();
    //Show Device Options
    show_dshow_device_option();
    //Show VFW Options
    show_vfw_device();
    
#if USE_DSHOW
    AVInputFormat *ifmt=av_find_input_format("dshow");
    //Set own video device's name
    if(avformat_open_input(&pFormatCtx,"video=Integrated Camera",ifmt,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#else
    AVInputFormat *ifmt=av_find_input_format("vfwcap");
    if(avformat_open_input(&pFormatCtx,"0",ifmt,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#endif
#elif defined linux
    //Linux
    AVInputFormat *ifmt=av_find_input_format("video4linux2");
    if(avformat_open_input(&pFormatCtx,"/dev/video0",ifmt,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#else
    show_avfoundation_device();
    //Mac
    AVInputFormat *ifmt=av_find_input_format("avfoundation");
    //Avfoundation
    //[video]:[audio]
    AVDictionary *opt= NULL;
    av_dict_set(&opt, "framerate", "30",0);
    av_dict_set(&opt, "video_size", "640*480", 0);
    if(avformat_open_input(&pFormatCtx,"0",ifmt,&opt)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#endif
    
    
    if(avformat_find_stream_info(pFormatCtx,NULL)<0)
    {
        printf("Couldn't find stream information.\n");
        return -1;
    }
    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    if(videoindex==-1)
    {
        printf("Couldn't find a video stream.\n");
        return -1;
    }
    pCodecCtx=pFormatCtx->streams[videoindex]->codec;
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
    {
        printf("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
    {
        printf("Could not open codec.\n");
        return -1;
    }
    AVFrame	*pFrame,*pFrameYUV;
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    //uint8_t *out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    //avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
    //SDL----------------------------
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }
    int screen_w=0,screen_h=0;
//    SDL_Surface *screen;
    screen_w = pCodecCtx->width;
    screen_h = pCodecCtx->height;
//    screen = SDL_SetVideoMode(screen_w, screen_h, 0,0);
    
    //----------
    
    SDL_Window         *pwindow;
    SDL_Renderer       *prenderer;
    SDL_Texture        *ptexture;
//    SDL_Rect           sdl_rect;
//    enum AVPixelFormat      pixfmt;
    
    //创建窗口
    //SDL_WINDOW_RESIZABLE: 使窗口可以拉伸
    pwindow = SDL_CreateWindow("Isshe Video Player!",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   screen_w, screen_h,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (pwindow == NULL){
        fprintf(stderr, "Couldn't Create Window\n");
        exit(-1);        //
    }
    
    //新建一个渲染器
    prenderer = SDL_CreateRenderer(pwindow, -1, 0);
    ptexture  = SDL_CreateTexture(prenderer,
                                      SDL_PIXELFORMAT_UYVY, SDL_TEXTUREACCESS_STREAMING,
                                      screen_w,screen_h);
    
    
    //
    
    
    AVFrame out_frame;
    out_frame.format = AV_PIX_FMT_YVYU422;
    out_frame.width = screen_w;
    out_frame.height = screen_h;
    //用av_image_fill_arrays代替。
    //根据所给参数和提供的数据设置data指针和linesizes。
//    av_image_fill_arrays(out_frame->data,out_frame->linesize, video_buf, AV_PIX_FMT_YVYU422, screen_w, screen_h, 0);
    
    int size =  av_image_get_buffer_size(AV_PIX_FMT_YVYU422, screen_w, screen_h, 1);
    uint8_t *video_buf = (uint8_t *)av_malloc(size);
    avpicture_fill((AVPicture *)&out_frame,video_buf,AV_PIX_FMT_UYVY422,screen_w,screen_h);

    //--------
//    if(!screen) {
//        printf("SDL: could not set video mode - exiting:%s\n",SDL_GetError());
//        return -1;
//    }
//    SDL_Overlay *bmp;
//    bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height,SDL_YV12_OVERLAY, screen);
//    SDL_Rect rect;
//    rect.x = 0;
//    rect.y = 0;
//    rect.w = screen_w;
//    rect.h = screen_h;
    //SDL End------------------------
    int ret, got_picture;
    
    AVPacket *packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    
#if OUTPUT_YUV420P
    FILE *fp_yuv=fopen("output.yuv","wb+");
#endif
    
    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YVYU422, SWS_BICUBIC, NULL, NULL, NULL);
    //------------------------------
    SDL_Thread *video_tid = SDL_CreateThread(sfp_refresh_thread,"refresh thread",NULL);
    //
//    SDL_WM_SetCaption("Simplest FFmpeg Read Camera",NULL);
    //Event Loop
    SDL_Event event;
    
    for (;;) {
        //Wait
        SDL_WaitEvent(&event);
        if(event.type==SFM_REFRESH_EVENT){
            //------------------------------
            if(av_read_frame(pFormatCtx, packet)>=0){
                if(packet->stream_index==videoindex){
                    ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
                    if(ret < 0){
                        printf("Decode Error.\n");
                        return -1;
                    }
                    if(got_picture){
                        sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, out_frame.data, out_frame.linesize);
                        
#if OUTPUT_YUV420P  
                        int y_size=pCodecCtx->width*pCodecCtx->height;    
                        fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y   
                        fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U  
                        fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V  
#endif
                        SDL_UpdateTexture(ptexture, NULL,
                                          out_frame.data[0],out_frame.linesize[0]);
                        SDL_RenderClear(prenderer);
                        SDL_RenderCopy(prenderer,ptexture, NULL, NULL);
                        SDL_RenderPresent(prenderer);
                    }
                }
                av_free_packet(packet);
            }else{
                //Exit Thread
                thread_exit=1;
                break;
            }
        }else if(event.type==SDL_QUIT){
            thread_exit=1;
            break;
        }
        
    } // for
    
    
    sws_freeContext(img_convert_ctx);
    
#if OUTPUT_YUV420P 
    fclose(fp_yuv);
#endif 
    
    SDL_Quit();
    
    //av_free(out_buffer);
    av_free(pFrameYUV);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    
    return 0;
}

