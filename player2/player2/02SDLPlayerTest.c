////
////  02SDLPlayerTest.c
////  player2
////
////  Created by zhangyun on 10/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "02SDLPlayerTest.h"
//
//
//#ifndef ffmpeg_h
//#define ffmpeg_h
//#include "avcodec.h"
//#include "avformat.h"
//#include "avfilter.h"
//#include "swscale.h"
//#endif
//
//
//#ifndef sdl_h
//#define sdl_h
//#include "SDL.h"
//#include "SDL_thread.h"
//#endif
//
//
//int playTest2(){
//    
//    AVFormatContext *fctx = NULL;
//    int ret = -1;
//    int videoStreamIndex = -1;
//    AVCodecContext *codeCtx = NULL;
//    AVCodec *decodec = NULL;
//    AVCodecContext *copyCodeCtx = NULL;
//    AVFrame *frame = NULL;
//    AVPacket packet;
//    SDL_Window *window;
//    SDL_Renderer *render;
//    SDL_Texture *texture;
//    struct SwsContext *sws_ctx = NULL;
//    Uint8 *yPlane,*uPlane,*vPlane;
//    size_t yPlaneSz,uvPlaneSz;
//    int uvPitch;
//    SDL_Event event;
//    
//    char *file_path = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//    
//    av_register_all();
//    
////    fctx = avformat_alloc_context();
//   ret = avformat_open_input(&fctx, file_path, NULL, NULL);
//    
//    if (ret != 0) {
//        fprintf(stderr, "open input fail");
//   }
//    
//    avformat_find_stream_info(fctx, NULL);
//    
//    av_dump_format(fctx, 0, file_path, 0);
//    
//    for (int i = 0; i < fctx->nb_streams; i++) {
//        if (fctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoStreamIndex = i;
//            break;
//        }
//    }
//    
//    codeCtx = av_malloc(sizeof(AVCodecContext));
//    
//    ret = avcodec_parameters_to_context(codeCtx, fctx->streams[videoStreamIndex]->codecpar);
//    
//    if (ret < 0) {
//        fprintf(stderr, "avcodec param to context faile");
//    }
//    
//    decodec = avcodec_find_decoder(codeCtx->codec_id);
//    
//    copyCodeCtx = avcodec_alloc_context3(decodec);
//    if (copyCodeCtx == NULL) {
//        fprintf(stderr, "avcodec alloc failed");
//    }
//    
//    ret = avcodec_parameters_to_context(copyCodeCtx, fctx->streams[videoStreamIndex]->codecpar);
//    if (ret < 0) {
//        fprintf(stderr, "param to ctx failed");
//        return -1;
//    }
//    
//    // open decoder to decode packet
//    
//    ret = avcodec_open2(copyCodeCtx, decodec, NULL);
//    if (ret < 0) {
//        fprintf(stderr, "codec open failed");
//        return -1;
//    }
//    
//    
//    
//    if (SDL_Init(SDL_INIT_EVERYTHING)) {
//        fprintf(stderr, "init sdl failer");
//        exit(1);
//    }
//    
//    window = SDL_CreateWindow("player2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, copyCodeCtx->width, copyCodeCtx->height,0);
//    
//    if (window == NULL) {
//        fprintf(stderr, "window failed");
//        exit(1);
//    }
//    render = SDL_CreateRenderer(window, -1, 0);
//    if (render == NULL) {
//        fprintf(stderr, "render failed");
//        exit(1);
//    }
//    
//    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, copyCodeCtx->width, copyCodeCtx->height);
//    if (texture == NULL) {
//        fprintf(stderr, "texture failed");
//        exit(1);
//    }
//    
//    sws_ctx = sws_getContext(copyCodeCtx->width, copyCodeCtx->height, copyCodeCtx->pix_fmt, copyCodeCtx->width, copyCodeCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
//    
//    yPlaneSz = copyCodeCtx->width * copyCodeCtx->height;
//    uvPlaneSz = copyCodeCtx->width * copyCodeCtx->height/4;
//    yPlane = (Uint8 *)malloc(yPlaneSz);
//    uPlane = (Uint8 *)malloc(yPlaneSz);
//    vPlane = (Uint8 *)malloc(yPlaneSz);
//    if (!yPlane || !uPlane || !vPlane) {
//        fprintf(stderr, "yuv init failed");
//        exit(1);
//    }
//    
//    uvPitch = copyCodeCtx->width / 2;
//    
//    
//    
//    frame = av_frame_alloc();
//    while (av_read_frame(fctx, &packet) >= 0 ) {
//        if (packet.stream_index == videoStreamIndex) {
//            ret = avcodec_send_packet(copyCodeCtx, &packet);
//            if (ret < 0) {
//                fprintf(stderr, "send packet failed");
//            }
//            
//            while (ret >= 0) {
//                ret = avcodec_receive_frame(copyCodeCtx, frame);
//                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                    break;
//                }else if(ret < 0){
//                    fprintf(stderr, "receive frame error");
//                }
//                
//                if (ret >= 0) {
//                    uint8_t *dst[3] = {yPlane,uPlane,vPlane};
//                    int size[3] = {copyCodeCtx->width,uvPitch,uvPitch};
//                    
//                   ret = sws_scale(sws_ctx, (uint8_t const * const *)frame->data, frame->linesize, 0, copyCodeCtx->height, dst, size);
//                    
//                    if (ret < 0) {
//                        fprintf(stderr, "scale failed");
//                    }
//
//                    ret = SDL_UpdateYUVTexture(texture, NULL, yPlane, copyCodeCtx->width, uPlane, uvPitch, vPlane, uvPitch);
//                    
//                    if (ret < 0) {
//                        fprintf(stderr, "update texture failed");
//                    }
//                    SDL_RenderClear(render);
//                    SDL_RenderCopy(render, texture, NULL, NULL);
//                    SDL_RenderPresent(render);
//                }
//                
//            }
//        }
//        
//        
//        av_packet_unref(&packet);
//        SDL_PollEvent(&event);
//        switch (event.type) {
//            case SDL_QUIT:
//                SDL_DestroyTexture(texture);
//                SDL_DestroyRenderer(render);
//                SDL_DestroyWindow(window);
//                SDL_Quit();
//                exit(0);
//                break;
//            default:
//                break;
//        }
//    }// while
//    
//    av_frame_free(&frame);
//    free(yPlane);
//    free(uPlane);
//    free(vPlane);
//    
//    avcodec_close(copyCodeCtx);
//    avcodec_close(codeCtx);
//    
//    // Close the video file
//    avformat_close_input(&fctx);
//    
//    return 0;
//}
