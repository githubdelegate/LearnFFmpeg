////
////  audio_player.c
////  player2
////
////  Created by zhangyun on 24/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "audio_player.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <assert.h>
//#include <unistd.h>
//
//#define __STDC_CONSTANT_MACROS      //ffmpeg要求
//
//#ifdef __cplusplus
//extern "C"
//{
//#endif
//    
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libswresample/swresample.h>
//#include <libswscale/swscale.h>
//#include <SDL2/SDL.h>
//    
//#ifdef __cplusplus
//}
//#endif
//
//
//#include "packet_queue.h"
//
//#define AVCODE_MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
//#define SDL_AUDIO_BUFFER_SIZE   1024    //
//
//#define FILE_NAME               "/home/isshe/Music/WavinFlag.aac"
//#define ERR_STREAM              stderr
//#define OUT_SAMPLE_RATE         44100
//
//AVFrame         wanted_frame;
//PacketQueue     audio_queue;
//int      quit = 0;
//
//void audio_callback(void *userdata, Uint8 *stream, int len);
//int audio_decode_frame(AVCodecContext *pcodec_ctx, uint8_t *audio_buf, int buf_size);
//
//int playAudio()
//{
//    AVFormatContext     *pformat_ctx = NULL;
//    int                 audio_stream = -1;
//    AVCodecContext      *pcodec_ctx = NULL;
////    AVCodecContext      *pcodec_ctx_cp = NULL;
//    AVCodec             *pcodec = NULL;
//    AVPacket            packet ;        //!
////    AVFrame             *pframe = NULL;
//    char                filename[256] = FILE_NAME;
//    
//    //SDL
//    SDL_AudioSpec       wanted_spec;
//    SDL_AudioSpec       spec ;
//    
//    //ffmpeg 初始化
//    av_register_all();
//    
//    //SDL初始化
//    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)){
//        fprintf(ERR_STREAM, "Couldn't init SDL:%s\n", SDL_GetError());
//        exit(-1);
//    }
//    
////    char *inputfile = "/Users/zhangyun/Downloads/simplest_ffmpeg_audio_player-master/simplest_ffmpeg_audio_player/xiaoqingge.mp3";
//    
//    char *inputfile = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//
//    
//    //打开文件
//    if (avformat_open_input(&pformat_ctx, inputfile, NULL, NULL) != 0){
//        fprintf(ERR_STREAM, "Couldn't open input file\n");
//        exit(-1);
//    }
//    
//    //检测文件流信息
//    //旧版的是av_find_stream_info()
//    if (avformat_find_stream_info(pformat_ctx, NULL) < 0){
//        fprintf(ERR_STREAM, "Not Found Stream Info\n");
//        exit(-1);
//    }
//    
//    //显示文件信息，十分好用的一个函数
//    av_dump_format(pformat_ctx, 0, filename, 0);
//    
//    int videoStream = -1;
//    
//    for (int i = 0; i < pformat_ctx->nb_streams; i++) {
//        if (pformat_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream < 0) {
//            audio_stream = i;
//        }else if (pformat_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0){
//            videoStream = i;
//        }
//    }
//    
//    printf("audio_stream = %d\n", audio_stream);
//    //找到对应的解码器
//    pcodec_ctx = pformat_ctx->streams[audio_stream]->codec;
//    pcodec = avcodec_find_decoder(pcodec_ctx->codec_id);
//    if (!pcodec){
//        fprintf(ERR_STREAM, "Couldn't find decoder\n");
//        exit(-1);
//    }
//    
//    //设置音频信息, 用来打开音频设备。
//    wanted_spec.freq        = pcodec_ctx->sample_rate;
//    wanted_spec.format      = AUDIO_S16SYS;
//    wanted_spec.channels    = pcodec_ctx->channels;        //通道数
//    wanted_spec.silence     = 0;    //设置静音值
//    wanted_spec.samples     = pcodec_ctx->frame_size;        //读取第一帧后调整?
//    wanted_spec.callback    = audio_callback;
//    wanted_spec.userdata    = pcodec_ctx;
//    
//    //wanted_spec:想要打开的
//    //spec: 实际打开的,可以不用这个，函数中直接用NULL，下面用到spec的用wanted_spec代替。
//    //这里会开一个线程，调用callback。
//    //SDL_OpenAudio->open_audio_device(开线程)->SDL_RunAudio->fill(指向callback函数）
//    //可以用SDL_OpenAudioDevice()代替
//    if (SDL_OpenAudio(&wanted_spec, &spec) < 0){
//        fprintf(ERR_STREAM, "Couldn't open Audio:%s\n", SDL_GetError());
//        exit(-1);
//    }
//    
//    //设置参数，供解码时候用, swr_alloc_set_opts的in部分参数
//    wanted_frame.format         = AV_SAMPLE_FMT_S16;
//    wanted_frame.sample_rate    = spec.freq;
//    wanted_frame.channel_layout = av_get_default_channel_layout(spec.channels);
//    wanted_frame.channels       = spec.channels;
//    
//    //初始化AVCondecContext，以及进行一些处理工作。
//    avcodec_open2(pcodec_ctx, pcodec, NULL);
//    
//    //初始化队列
//    packet_queue_init(&audio_queue);
//    
//    //可以用SDL_PauseAudioDevice()代替,目前用的这个应该是旧的。
//    //0是不暂停，非零是暂停
//    //如果没有这个就放不出声音
//    SDL_PauseAudio(0);              //为什么要这个？
//    
//    //------------------------------
//    SDL_Window *window;
//    SDL_Renderer *render;
//    SDL_Texture *texture;
//    SDL_Event event;
//    AVCodecContext *vCodecCtx;
//    AVCodec *vCodec;
//    AVFrame *vFrame = av_frame_alloc();
//    int ret =-1;
//    
//    //pcodec
//    vCodec = avcodec_find_decoder(pformat_ctx->streams[videoStream]->codecpar->codec_id);
//    if (!vCodec) {
//        fprintf(stderr, "find video codec failed");
//        return -1;
//    }
//    // pcodecCtx
//    vCodecCtx = avcodec_alloc_context3(vCodec);
//    if(avcodec_parameters_to_context(vCodecCtx, pformat_ctx->streams[videoStream]->codecpar) < 0){
//        fprintf(stderr, "video param to context failed");
//        return -1;
//    }
//    
//    if(avcodec_open2(vCodecCtx, vCodec,NULL) < 0){
//        fprintf(stderr, "open2 video codec ctx failed");
//        return -1;
//    }
//    
//    window = SDL_CreateWindow("player2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, vCodecCtx->width, vCodecCtx->height,0);
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
//    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, vCodecCtx->width, vCodecCtx->height);
//    if (texture == NULL) {
//        fprintf(stderr, "texture failed");
//        exit(1);
//    }
//    
//    struct SwsContext *sws_ctx = NULL;
//    // initialize SWS context for software scaling
//    sws_ctx = sws_getContext(vCodecCtx->width, vCodecCtx->height,
//                             vCodecCtx->pix_fmt, vCodecCtx->width, vCodecCtx->height,
//                             AV_PIX_FMT_YUV420P,
//                             SWS_BILINEAR,
//                             NULL,
//                             NULL,
//                             NULL);
//    
//    
//    Uint8 *yPlane,*uPlane,*vPlane;
//    size_t yPlaneSz,uvPlaneSz;
//    int uvPitch;
//    
//    // set up YV12 pixel array (12 bits per pixel)
//    yPlaneSz = vCodecCtx->width * vCodecCtx->height;
//    uvPlaneSz = vCodecCtx->width * vCodecCtx->height / 4;
//    yPlane = (Uint8*)malloc(yPlaneSz);
//    uPlane = (Uint8*)malloc(uvPlaneSz);
//    vPlane = (Uint8*)malloc(uvPlaneSz);
//    if (!yPlane || !uPlane || !vPlane) {
//        fprintf(stderr, "Could not allocate pixel buffers - exiting\n");
//        exit(1);
//    }
//    uvPitch = vCodecCtx->width / 2;
//
//    //-----------------------------
//    //读一帧数据
//    while(av_read_frame(pformat_ctx, &packet) >= 0)     //读一个packet，数据放在packet.data
//    {
//        if (packet.stream_index == audio_stream){
//            packet_queue_put(&audio_queue, &packet);
//        }
//        else if(packet.stream_index == videoStream){
//            
//            
//            ret = avcodec_send_packet(vCodecCtx,&packet);
//            if (ret < 0) {
//                fprintf(stderr, "send packet error");
//            }
//            while (ret >= 0) {
//                ret = avcodec_receive_frame(vCodecCtx, vFrame);
//                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                    break;
//                }else if (ret < 0){
//                    fprintf(stderr, "receive frame error");
//                }
//                
//                if (ret >= 0) {
//                    uint8_t *dst[3];
//                    dst[0] = yPlane;
//                    dst[1] = uPlane;
//                    dst[2] = vPlane;
//                    
//                    int size[3];
//                    size[0] = vCodecCtx->width;
//                    size[1] = uvPitch;
//                    size[2] = uvPitch;
//                    
//                    // Convert the image into YUV format that SDL uses
//                    int oh = sws_scale(sws_ctx, (uint8_t const * const *) vFrame->data,
//                                       vFrame->linesize, 0, vCodecCtx->height, dst,
//                                       size);
//                    
//                    if (oh < 0) {
//                        fprintf(stderr, "sws scale errro");
//                    }
//                    
//                    int res = SDL_UpdateYUVTexture(
//                                                   texture,
//                                                   NULL,
//                                                   yPlane,
//                                                   vCodecCtx->width,
//                                                   uPlane,
//                                                   uvPitch,
//                                                   vPlane,
//                                                   uvPitch
//                                                   );
//                    
//                    if (res != 0) {
//                        fprintf(stderr, "sdl update texture error");
//                    }
//                    SDL_RenderClear(render);
//                    SDL_RenderCopy(render, texture, NULL, NULL);
//                    SDL_RenderPresent(render);
//                    
//                    double frame_delay = av_q2d(vCodecCtx->time_base);
//                    frame_delay += vFrame->repeat_pict * (frame_delay * 0.5);
//                    av_usleep(frame_delay * 1000 * 1000);
//                } // if
//            } // while
//            
//        }else{
//            av_free_packet(&packet);
//        }
//        
//        // Free the packet that was allocated by av_read_frame
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
//
//    }// while
//    
//    
//    // Free the YUV frame
//    av_frame_free(&vFrame);
//    free(yPlane);
//    free(uPlane);
//    free(vPlane);
//    
//    // Close the codec
//    avcodec_close(vCodecCtx);
//    avcodec_close(pcodec_ctx);
//    
//    // Close the video file
//    avformat_close_input(&pformat_ctx);
//
//    getchar();      //...
//    return 0;
//}
//
//
////注意userdata是前面的AVCodecContext.
////len的值常为2048,表示一次发送多少。
////audio_buf_size：一直为样本缓冲区的大小，wanted_spec.samples.（一般每次解码这么多，文件不同，这个值不同)
////audio_buf_index： 标记发送到哪里了。
////这个函数的工作模式是:
////1. 解码数据放到audio_buf, 大小放audio_buf_size。(audio_buf_size = audio_size;这句设置）
////2. 调用一次callback只能发送len个字节,而每次取回的解码数据可能比len大，一次发不完。
////3. 发不完的时候，会len == 0，不继续循环，退出函数，继续调用callback，进行下一次发送。
////4. 由于上次没发完，这次不取数据，发上次的剩余的，audio_buf_size标记发送到哪里了。
////5. 注意，callback每次一定要发且仅发len个数据，否则不会退出。
////如果没发够，缓冲区又没有了，就再取。发够了，就退出，留给下一个发，以此循环。
////三个变量设置为static就是为了保存上次数据，也可以用全局变量，但是感觉这样更好。
//void audio_callback(void *userdata, Uint8 *stream, int len)
//{
//    AVCodecContext *pcodec_ctx     = (AVCodecContext *)userdata;
//    int len1 = 0;
//    int audio_size = 0;
//    
//    //注意是static
//    //为什么要分那么大？
//    static uint8_t         audio_buf[(AVCODE_MAX_AUDIO_FRAME_SIZE * 3) / 2];
//    static unsigned int    audio_buf_size = 0;
//    static unsigned int    audio_buf_index = 0;
//    
//    //初始化stream，每次都要。
//    SDL_memset(stream, 0, len);
//    
//    while(len > 0)
//    {
//        if (audio_buf_index >= audio_buf_size){
//            audio_size = audio_decode_frame(pcodec_ctx, audio_buf, sizeof(audio_buf));
//            if (audio_size < 0){
//                //错误则静音
//                audio_buf_size = 1024;
//                memset(audio_buf, 0, audio_buf_size);
//            }
//            else{
//                audio_buf_size = audio_size;
//            }
//            audio_buf_index = 0;      //回到缓冲区开头
//        } // if
//        
//        len1 = audio_buf_size - audio_buf_index;
//        //          printf("len1 = %d\n", len1);
//        if (len1 > len)       //len1常比len大，但是一个callback只能发len个
//        {
//            len1 = len;
//        }
//        
//        //新程序用 SDL_MixAudioFormat()代替
//        //混合音频， 第一个参数dst， 第二个是src，audio_buf_size每次都在变化
////        SDL_MixAudio(stream, (uint8_t*)audio_buf + audio_buf_index, len, SDL_MIX_MAXVOLUME);
//        memcpy(stream, (uint8_t *)audio_buf + audio_buf_index,len1);
//        len -= len1;
//        stream += len1;
//        audio_buf_index += len1;
//    } // while
//}
//
//
//#define USEME 1
////对于音频来说，一个packet里面，可能含有多帧(frame)数据。
//int audio_decode_frame(AVCodecContext *pcodec_ctx,uint8_t *audio_buf, int buf_size)
//{
//    AVPacket   packet;
//    AVFrame    *frame;
//    int        pkt_size = 0;
//    long long  audio_buf_index = 0;
//    SwrContext *swr_ctx = NULL;
//    int        convert_len = 0;
//    
//    if (packet_queue_get(&audio_queue, &packet, 1) < 0){
//        fprintf(ERR_STREAM, "Get queue packet error\n");
//        return -1;
//    }
//    pkt_size = packet.size;
//    fprintf(ERR_STREAM, "pkt_size = %d\n", pkt_size);
//    frame = av_frame_alloc();
//    //---------------------------------------------------------------------------
//#ifdef USEME
//    int ret =  avcodec_send_packet(pcodec_ctx, &packet);
//    if (ret == 0) {
//        for (;;) {
//            ret =  avcodec_receive_frame(pcodec_ctx, frame);
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
//                    fprintf(ERR_STREAM, "swr_init error\n");
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
//                audio_buf_index += wanted_frame.channels * convert_len * av_get_bytes_per_sample((enum AVSampleFormat)wanted_frame.format);
//            }else{ // 数据已经取完了。
//                break;
//            }
//        } // for
//    } // if
//    
//    return (int)audio_buf_index;
//#endif
//}
//
//
//void decode_display_video(){
//    
//}
