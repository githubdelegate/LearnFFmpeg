////
////  03SDLPlayer.c
////  player2
////
////  Created by zhangyun on 11/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "03SDLPlayer.h"
//
//#ifndef ffmpeg_h
//#define ffmpeg_h
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/avfilter.h>
//#include <libswscale/swscale.h>
//#include <libavutil/channel_layout.h>
//#include <libavutil/samplefmt.h>
//#include <libswresample/swresample.h>
//#include <libavutil/mathematics.h>
//#endif
//
//#ifndef sdl_h
//#define sdl_h
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_thread.h>
//#endif
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//
//
//#define SDL_AUDIO_BUFFER_SIZE 1024
////#define MAX_AUDIO_FRAME_SIZE 192000
//
//#define AVCODE_MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
//
//typedef struct PacketQueue{
//    AVPacketList *first_pkt,*last_pkt;
//    int nb_packets;
//    int size;
//    SDL_mutex *mutex;
//    SDL_cond *cond;
//}PacketQueue;
//
//PacketQueue audioq;
//
//AVFrame wanted_frame;
//
//int quit = 0;
//
//void packet_queue_init(PacketQueue *q){
//    memset(q, 0, sizeof(PacketQueue));
//    q->mutex = SDL_CreateMutex();
//    q->cond = SDL_CreateCond();
//}
//
//// 添加 packet
//int packet_queue_put(PacketQueue *q,AVPacket *pkt){
////    typedef struct AVPacketList {
////        AVPacket pkt;
////        struct AVPacketList *next;
////    } AVPacketList;
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
//    SDL_LockMutex(q->mutex);
//    
//    if (!q->last_pkt) { // 说明是空链表
//        q->first_pkt = pkt1;
//    }else{
//        q->last_pkt->next = pkt1;
//    }
//    
//    q->last_pkt = pkt1;
//    q->nb_packets++;
//    q->size += pkt1->pkt.size;
//    //Restart one of the threads that are waiting on the condition variable.
//    SDL_CondSignal(q->cond);
//    SDL_UnlockMutex(q->mutex);
//    return 0;
//}
//
//// 取packet
//static int packet_queue_get(PacketQueue *q,AVPacket *pkt,int block){
//    AVPacketList *pkt1;
//    int ret;
//    SDL_LockMutex(q->mutex);
//    
//    for (;;) { // 死循环
//        if (quit) {
//            ret = -1;
//            break;
//        }
//        
//        pkt1 = q->first_pkt;
//        if (pkt1) {
//            q->first_pkt = pkt1->next;
//            if (!q->first_pkt) { // 只有一个node
//                q->last_pkt = NULL;
//            }
//            
//            q->nb_packets--;
//            q->size -= pkt1->pkt.size;
//            *pkt = pkt1->pkt;
//            av_free(pkt1);
//            ret = 1;
//            break;
//        }else if(!block){ // 是否阻塞等待 q 中来了数据
//            ret = 0;
//            break;
//        }else{
//            
//            
//            /**
//             *  Wait on the condition variable, 
//             unlocking the provided mutex.
//             The mutex must be locked before
//             entering this function!
//             *  The mutex is re-locked once the condition 
//             variable is signaled.
//             *  0 when it is signaled,
//             or -1 on error.
//             */
//            SDL_CondWait(q->cond, q->mutex);
//        }
//    } // for
//    
//    SDL_UnlockMutex(q->mutex);
//    return ret;
//}
//
//// 解码音频packet
//static int audio_decode_frame(AVCodecContext *aCodecCtx,uint8_t *audio_buf,int buf_size){
//    AVPacket pkt;
//    uint8_t *audio_pkt_data = NULL;
//    int audio_pkt_size = 0;
//    AVFrame *frame = NULL;
//    SwrContext *swr_ctx = NULL;
//    int        convert_len = 0;
//    int        convert_all = 0;
//    long long  audio_buf_index = 0;
//    
//    int len1, data_size = 0;
//    int ret = -1;
//    
//    frame = av_frame_alloc();
//    
//    if (packet_queue_get(&audioq, &pkt, 1) < 0){
//        return -1;
//    }
//    int pkt_size = pkt.size;
//
//    
//    int decoded_size = 0;
//    ret =  avcodec_send_packet(aCodecCtx, &pkt);
//    if (ret == 0) {
//        for (;;) {
//            ret =  avcodec_receive_frame(aCodecCtx, frame);
//            if (ret == 0) { // 解码成功，转码，
//                if (frame->channels > 0 && frame->channel_layout == 0){
//                    frame->channel_layout = av_get_default_channel_layout(frame->channels);
//                }
//                else if (frame->channels == 0 && frame->channel_layout > 0){
//                    frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
//                }
//                
//                if (swr_ctx != NULL){
//                    swr_free(&swr_ctx);
//                    swr_ctx = NULL;
//                }
//                
//                swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout,
//                                             (enum AVSampleFormat)wanted_frame.format,
//                                             wanted_frame.sample_rate, frame->channel_layout,
//                                             (enum AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);
//                //初始化
//                if (swr_ctx == NULL || swr_init(swr_ctx) < 0){
////                    fprintf(ERR_STREAM, "swr_init error\n");
//                    break;
//                }
//                ////number of samples output per channel
//                convert_len = swr_convert(swr_ctx,
//                                          &audio_buf + audio_buf_index,
//                                          AVCODE_MAX_AUDIO_FRAME_SIZE,
//                                          (const uint8_t **)frame->data,
//                                          frame->nb_samples);
//                if (convert_len <=0) { // 转换失败，就丢弃这一帧，继续下一帧
//                    continue;
//                }
//                
//                audio_buf_index += wanted_frame.channels * convert_len * av_get_bytes_per_sample((enum AVSampleFormat)wanted_frame.format);
//            }else{ // 数据已经取完了。
//                break;
//            }
//        } // for
//    } // if
//    
//    return audio_buf_index;
//
//    
////    for (;;) {
////        // 如果有数据 就开始解码
////        while(audio_pkt_size > 0) {
////            
////            // 发送数据给解码器 解码
////            ret = avcodec_send_packet(aCodecCtx, &pkt);
////            
////            if (ret < 0) {// 解码失败，直接返回。
////                fprintf(stderr, "send packet failed");
////                return -1;
////            }
////            while(ret >= 0) { // 解码成功，读取解码数据
////                ret = avcodec_receive_frame(aCodecCtx, frame);
////                if (ret == AVERROR(EAGAIN) || ret ==  AVERROR_EOF) { //
////                    break; // 数据读取完毕了，没有数据可以读取了，退出了。
////                }else if(ret < 0){
////                    fprintf(stderr, "receive frame error");
////                    return -1;
////                }else if (ret >= 0){ // 读取数据成功，把数据copy过去。
////                    // handler audio frame
////                    data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame->nb_samples, aCodecCtx->sample_fmt, 1);
////                    memcpy(audio_buf, frame->data[0], data_size);
////                    if (data_size <= 0) {
////                        continue;
////                    }
////                    return data_size;
////                }
////            } // while
////        } // while
////        if (pkt.data) {
////            av_packet_unref(&pkt);
////        }
////        
////        if (quit) {
////            return -1;
////        }
////        
////        if (packet_queue_get(&audioq, &pkt, 1) < 0) {
////            return -1;
////        }
////        audio_pkt_data = pkt.data;
////        audio_pkt_size = pkt.size;
////    }
//}
//
//void audio_callback(void *userdata,Uint8 *stream,int len){
//    AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
//    int len1,audio_size;
//    
//    SDL_memset(stream, 0, len);
//    
//    static uint8_t audio_buf[(AVCODE_MAX_AUDIO_FRAME_SIZE * 3) / 2];
//    static unsigned int audio_buf_size = 0;
//    static unsigned int audio_buf_index = 0;
//    // 去取音频数据 如果数据够就填充返回，剩下的下次使用，所以使用static ，
//    // 不够就循环的去解码 取数据
//    while (len > 0) {
//        if (audio_buf_index >= audio_buf_size) {
//            audio_size = audio_decode_frame(aCodecCtx, audio_buf, audio_buf_size);
//            if (audio_size < 0) {
//                audio_buf_size = 1024;
//                memset(audio_buf, 0, audio_buf_size);
//            }else{
//                audio_buf_size = audio_size;
//            }
//            audio_buf_index = 0;
//        } // i
//        
//        len1 = audio_buf_size - audio_buf_index;
//        if (len1 > len) {
//            len1 = len;
//        }
//        
//        memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
//        len -= len1;
//        stream += len1;
//        audio_buf_index += len1;
//    } // while
//    
//}
//
//int play03(){
//    AVFormatContext *pFormatCtx = NULL;
//    int videoStream,audioStream;
//    AVCodecContext *pCodecCtx = NULL;
//    AVCodec *pCodec = NULL;
//    AVFrame *pFrame = NULL;
//    AVPacket packet;
////    int frameFinished;
//    
//    AVCodecContext *aCodecCtx = NULL;
//    AVCodec *aCodec = NULL;
//    
//    
//    SDL_Window *window;
//    SDL_Renderer *render;
//    SDL_Texture *texture;
////    SDL_Rect rect;
//    SDL_Event event;
//    SDL_AudioSpec wanted_spec,spec;
//
//    Uint8 *yPlane,*uPlane,*vPlane;
//    size_t yPlaneSz,uvPlaneSz;
//    int uvPitch;
//    struct SwsContext *sws_ctx = NULL;
//    
//    AVDictionary *videoOptionDict = NULL;
//    AVDictionary *audioOPtionsDict = NULL;
//    int ret = -1;
//    
//    char *file_path = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//    
//    av_register_all();
//    
//    
//    // SDL
//    if(SDL_Init(SDL_INIT_EVERYTHING)){
//        fprintf(stderr, "sdl_init failed");
//        exit(1);
//    }
//    
//    if(avformat_open_input(&pFormatCtx, file_path, NULL, NULL) != 0){
//        fprintf(stderr, "open file failed");
//        return -1;
//    }
//    
//    if(avformat_find_stream_info(pFormatCtx, NULL) < 0 ){
//        fprintf(stderr, "find stream info failed");
//        return -1;
//    }
//    
//    av_dump_format(pFormatCtx, 0, file_path, 0);
//    
//    videoStream = -1;
//    audioStream = -1;
//    
//    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
//        if (videoStream < 0 &&  pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoStream = i;
//        }
//        
//        if (audioStream < 0 && pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//            audioStream = i;
//        }
//    } // for
//    
//    if (videoStream == -1) {
//        return -1;
//    }
//    
//    if (audioStream == -1) {
//        return -1;
//    }
//    
//    
//    // acodec
//    aCodec = avcodec_find_decoder(pFormatCtx->streams[audioStream]->codecpar->codec_id);
//    if (!aCodec) {
//        fprintf(stderr, "find audio codec failed");
//        return -1;
//    }
//
//    // aCodecCtx
//    aCodecCtx = avcodec_alloc_context3(aCodec);
//    if(avcodec_parameters_to_context(aCodecCtx, pFormatCtx->streams[audioStream]->codecpar)< 0){
//        fprintf(stderr, "audio param to context failed");
//        return -1;
//    }
//    if(avcodec_open2(aCodecCtx, aCodec, &audioOPtionsDict) < 0){
//        fprintf(stderr, "open2 audioo codecctx failed");
//        return -1;
//    }
//    
//    wanted_spec.freq = aCodecCtx->sample_rate;
//    //Signed 16-bit samples
//    wanted_spec.format = AUDIO_S16SYS;
//    wanted_spec.channels = aCodecCtx->channels;
//    wanted_spec.silence = 0;
//    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
//    wanted_spec.callback = audio_callback;
//    wanted_spec.userdata = aCodecCtx;
//    
//    if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
//        fprintf(stderr, "open audio failed");
//        return -1;
//    }
//    
//    wanted_frame.sample_rate = spec.freq;
//    wanted_frame.format = AV_SAMPLE_FMT_S16;
//    wanted_frame.channels = spec.channels;
//    wanted_frame.channel_layout = av_get_default_channel_layout(spec.channels);
//    
//    packet_queue_init(&audioq);
//    
//    // begin to play audio.
//    SDL_PauseAudio(0);
//    
//    //pcodec
//    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
//    if (!pCodec) {
//        fprintf(stderr, "find video codec failed");
//        return -1;
//    }
//    // pcodecCtx
//    pCodecCtx = avcodec_alloc_context3(pCodec);
//    
//    if(avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) < 0){
//        fprintf(stderr, "video param to context failed");
//        return -1;
//    }
//    
//    if(avcodec_open2(pCodecCtx, pCodec, &videoOptionDict) < 0){
//        fprintf(stderr, "open2 video codec ctx failed");
//        return -1;
//    }
//    
//    // play
//    pFrame = av_frame_alloc();
//    
//    window = SDL_CreateWindow("player2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, pCodecCtx->width, pCodecCtx->height,0);
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
//    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
//    if (texture == NULL) {
//        fprintf(stderr, "texture failed");
//        exit(1);
//    }
//    
//        // initialize SWS context for software scaling
//        sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//                                 pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
//                                 AV_PIX_FMT_YUV420P,
//                                 SWS_BILINEAR,
//                                 NULL,
//                                 NULL,
//                                 NULL);
//    
//        // set up YV12 pixel array (12 bits per pixel)
//        yPlaneSz = pCodecCtx->width * pCodecCtx->height;
//        uvPlaneSz = pCodecCtx->width * pCodecCtx->height / 4;
//        yPlane = (Uint8*)malloc(yPlaneSz);
//        uPlane = (Uint8*)malloc(uvPlaneSz);
//        vPlane = (Uint8*)malloc(uvPlaneSz);
//        if (!yPlane || !uPlane || !vPlane) {
//            fprintf(stderr, "Could not allocate pixel buffers - exiting\n");
//            exit(1);
//        }
//    
//        uvPitch = pCodecCtx->width / 2;
//    
//        /*
//         */
//        while (av_read_frame(pFormatCtx, &packet) >= 0) {
//            // Is this a packet from the video stream?
//            if (packet.stream_index == videoStream) {
//                // Decode video frame
//                ret = avcodec_send_packet(pCodecCtx,&packet);
//                if (ret < 0) {
//                    fprintf(stderr, "send packet error");
//                }
//                while (ret >= 0) {
//                    ret = avcodec_receive_frame(pCodecCtx, pFrame);
//                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                        break;
//                    }else if (ret < 0){
//                        fprintf(stderr, "receive frame error");
//                    }
//    
//                    if (ret >= 0) {
//                        uint8_t *dst[3];
//                        dst[0] = yPlane;
//                        dst[1] = uPlane;
//                        dst[2] = vPlane;
//    
//                        int size[3];
//                        size[0] = pCodecCtx->width;
//                        size[1] = uvPitch;
//                        size[2] = uvPitch;
//    
//                        // Convert the image into YUV format that SDL uses
//                        int oh = sws_scale(sws_ctx, (uint8_t const * const *) pFrame->data,
//                                           pFrame->linesize, 0, pCodecCtx->height, dst,
//                                           size);
//    
//                        if (oh < 0) {
//                            fprintf(stderr, "sws scale errro");
//                        }
//        
//                        int res = SDL_UpdateYUVTexture(
//                                                       texture,
//                                                       NULL,
//                                                       yPlane,
//                                                       pCodecCtx->width,
//                                                       uPlane,
//                                                        uvPitch,
//                                                       vPlane,
//                                                       uvPitch
//                                                       );
//    
//                        if (res != 0) {
//                            fprintf(stderr, "sdl update texture error");
//                        }
//                        SDL_RenderClear(render);
//                        SDL_RenderCopy(render, texture, NULL, NULL);
//                        SDL_RenderPresent(render);
////                        av_packet_unref(&packet);
//                    } // if
//                } // while
//            }else if (packet.stream_index == audioStream){
//                packet_queue_put(&audioq, &packet);
//            }else{
//                av_packet_unref(&packet);
//            }
//            
//            // Free the packet that was allocated by av_read_frame
//            av_packet_unref(&packet);
//            SDL_PollEvent(&event);
//            switch (event.type) {
//                case SDL_QUIT:
//                    SDL_DestroyTexture(texture);
//                    SDL_DestroyRenderer(render);
//                    SDL_DestroyWindow(window);
//                    SDL_Quit();
//                    exit(0);
//                    break;
//                default:
//                    break;
//            }
//            
//        }// while
//        
//        // Free the YUV frame
//        av_frame_free(&pFrame);
//        free(yPlane);
//        free(uPlane);
//        free(vPlane);
//        
//        // Close the codec
//        avcodec_close(pCodecCtx);
//        
//        // Close the video file
//        avformat_close_input(&pFormatCtx);
//        return 0;
//}
