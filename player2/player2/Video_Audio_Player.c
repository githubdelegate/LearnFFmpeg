//
//  Video_Audio_Player.c
//  player2
//
//  Created by zhangyun on 22/08/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "Video_Audio_Player.h"

#ifndef ffmpeg_h
#define ffmpeg_h
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavutil/mathematics.h>
#endif

#ifndef sdl_h
#define sdl_h
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SIZE_16M (0x1000000)

typedef struct AudioBuf{
    uint8_t *buf;
    uint8_t *pos;
    uint32_t size;
    SDL_mutex *mutex;
    SDL_cond *cond;
}AudioBuf;

AudioBuf g_audiobuf;


int audioBufInit(AudioBuf *ab){
    ab->buf = (uint8_t *)malloc(sizeof(uint8_t) * SIZE_16M);
    ab->pos = ab->buf;
    ab->size = 0;
    ab->mutex = SDL_CreateMutex();
    ab->cond = SDL_CreateCond();
    return 0;
}

static int bquit = 0;
static int lastBufIndex = 0;
static int lastStreamIndex = 0;
//SDL音频回调函数
static void audio_callback(void *userdata, Uint8* stream, int len)
{
    memset(stream, 0, len);
    if(bquit){
        return;
    }
    //放及取数据的这部分烂死
    printf("callback lock-len=%d\n",len);
    SDL_LockMutex(g_audiobuf.mutex);
    
    while(!bquit)
    {
//        uInt32 idata = g_audiobuf.buf.GetSize();
        uint32_t idata  = g_audiobuf.size;
        if(idata > len){ // 数据够
            memcpy(stream+lastStreamIndex, g_audiobuf.buf + lastBufIndex, len);
            lastBufIndex += len;
            g_audiobuf.size-=len;
            
            break;
        }
        else{ // 数据不够
            if (idata > 0) { // 有一点数据
                memcpy(stream+lastStreamIndex, g_audiobuf.buf+lastBufIndex, idata);
                lastBufIndex+=idata;
                lastStreamIndex+=idata;
                g_audiobuf.size -= idata;
            }
            
            printf("callback cond wait\n");
            //Wait on the condition variable, unlocking the provided mutex.
            SDL_CondWait(g_audiobuf.cond, g_audiobuf.mutex);
        }
    }
//    memcpy(stream, g_audiobuf.buf, len);
    lastStreamIndex=0;
    printf("callback unlock\n");
    SDL_UnlockMutex(g_audiobuf.mutex);
    
}

int playVideoAudio()
{
    AVFormatContext *pformatContext = NULL;
    AVCodecContext  *pcodecContext  = NULL;
    
    AVCodecContext *paudiocodecContext = NULL;
    
    AVCodec         *pcodec         = NULL;
    AVCodec *paudiocodec = NULL;
//    const char      *pinputfile     = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
    char *pinputfile = "/Users/zhangyun/Downloads/simplest_ffmpeg_audio_player-master/simplest_ffmpeg_audio_player/xiaoqingge.mp3";
    ;
    unsigned int     videoStream    = -1;
    unsigned int audioStream = -1;
    int              iret           = 0;
    
    
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        
        exit(1);
    }
    /**
     * 初始化libavformat库，并注册所有的合成器(muxer)、分离器(demuxer)和协议(protocol)
     */
    av_register_all();
    
    /**
     *打开输入流，并读取流的头部(header)。
     *`pformatContext`可以是通过`avformat_alloc_context()`分配得到的`AVFormatContext`，也可以是`NULL`，如果是`NULL`，则该函数会给其分配空间并赋值。
     *   第三个参数是指定输入格式，直接用`NULL`，则自动检测。
     *   第四个参数是`AVDictionary **`，一般直接用`NULL`，除非给`AVFormatContext`传递一些私有参数。
     *此函数只是打开输入流，相应的解码器并没有打开。
     *打开的流必须使用`avforamt_close_input()`关闭。
     */
    iret = avformat_open_input(&pformatContext, pinputfile, NULL, NULL);
    if(iret < 0)
    {
        //        cerr << "avforamt_open_input fail:" << iret << endl;
        exit(1);
    }
    /**
     * 获取流的信息，一些文件格式（如MPEG）没有头部(header)，可以使用这个函数来获取流的信息
     */
    avformat_find_stream_info(pformatContext, NULL);
    
    /**
     * 打印输入或输出格式的详细信息，第4个参数指定是输入(0)还是输出(1)。
     */
    av_dump_format(pformatContext, 0, pinputfile, 0);
    
    /**
     * 查找视频流
     */
    for(unsigned int i = 0; i < pformatContext->nb_streams; ++i)
    {
        /**
         * 使用 AVStream 中的 codecpar 字段，codec 字段被标记为 deprecated
         */
        if(pformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }else if(pformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioStream = i;
        }
    }
    if(-1 == videoStream)
    {
        //        cerr << "could not find video stream in " << pinputfile << endl;
        exit(1);
    }
    
    if (-1 == audioStream) {
        fprintf(stderr, "audio stream index failed");
        return -1;
    }
    
    
    
    /**
     * 使用引用计数
     */
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "refconted_frames" , "1", 0);
    
    /**
     * 创建一个AVCodecContext,并设置其中的字段为默认值。
     *
     * 新创建一个AVCodecContext，AVStream 中的 codec 被标记为 deprecated
     *
     * 必须使用 avcodec_free_context() 释放。
     */
    pcodecContext = avcodec_alloc_context3(NULL);
    
    /**
     * 根据 codecpar 中的值填充 codeccontext 中的相应字段。
     * 返回负值时表示失败。
     */
    avcodec_parameters_to_context(pcodecContext, pformatContext->streams[videoStream]->codecpar);
    
    /**
     * 使用 codeid 找到已注册的解码器
     */
    pcodec = avcodec_find_decoder(pcodecContext->codec_id);
    
    /**
     * 初始化 codeccontext，以使用指定的 codec。
     */
    avcodec_open2(pcodecContext, pcodec, &opts);
    
    
    // audio
    paudiocodecContext = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(paudiocodecContext, pformatContext->streams[audioStream]->codecpar);
    paudiocodec = avcodec_find_decoder(paudiocodecContext->codec_id);
    avcodec_open2(paudiocodecContext, paudiocodec, &opts);
    
    
    
    SDL_AudioSpec wanted_spec, got_spec;
    wanted_spec.freq = paudiocodecContext->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = paudiocodecContext->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = NULL;
    if(SDL_OpenAudio(&wanted_spec, &got_spec) < 0)
    {
//        cerr << "SDL_OpenAudio fail:" << SDL_GetError() << endl;
        exit(1);
    }
    AVFrame audio_wanted_frame;
    audio_wanted_frame.format = AV_SAMPLE_FMT_S16;
    audio_wanted_frame.sample_rate = got_spec.freq;
    audio_wanted_frame.channel_layout = av_get_default_channel_layout(got_spec.channels);
    audio_wanted_frame.channels = got_spec.channels;
    
    SwrContext *pswrcontext = NULL;
    
    uint8_t audio_buff[(192000 * 3) / 2];
    uint8_t *paudio_buff = audio_buff;
    int audio_buff_size = 0;
    
    //解码后的数据直接简单地放入这个buf中
    g_audiobuf.cond = SDL_CreateCond();
    g_audiobuf.mutex = SDL_CreateMutex();
    g_audiobuf.buf = (uint8_t *)malloc(sizeof(uint8_t) * 10240);
    
    
    SDL_PauseAudio(0);
    
    AVPacket    pkt;
    AVFrame    *pframe;
    AVFrame    *pframeYUV;
    struct SwsContext *pswscontext = NULL;
    int         numBytes    = 0;
    uint8_t    *buffer      = NULL;
    
    /**
     * 初始化 AVPacket 中的可选字段为默认值。
     * 结构中的 data 和 size 未设置，需要单独初始化。
     *
     * AVPacket 结构中的一些成员：
     *    AVBufferRef *buf;  使用引用计数时保存packet数据的引用;若为NULL，则表示packet数据不是引用计数。
     *    int64_t pts;
     *    int64_t dts;
     *    uint8_t *data;
     *    int   size;
     *    int   stream_index; 用来判断这个packet是音频还是视频或字幕等。
     *
     */
    av_init_packet(&pkt);
    
    pkt.data = NULL;
    pkt.size = 0;
    
    pframe = av_frame_alloc();
    
    pframeYUV = av_frame_alloc();
    /**
     * 返回按指定格式保存图片时需要的大小。第4个参数指定 linesize alignment 。
     */
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pcodecContext->width,
                                        pcodecContext->height, 4);
    buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    /**
     * 按指定的格式，用提供的数组来填充 data 和 linesize 。
     * 当 buffer 为 NULL 时，返回 buffer 需要的大小。
     *
     * 返回 buffer 需要的大小，负值时表示失败。
     */
    av_image_fill_arrays(pframeYUV->data, pframeYUV->linesize, buffer, AV_PIX_FMT_YUV420P,
                         pcodecContext->width, pcodecContext->height, 4);
    
    /**
     * 用来进行缩放或转换操作。参见sws_scale()。
     *
     * 使用SDL来显示视频中的每一帧图像。显示时使用YUV420P格式，但视频流中解码后的格式可能不是这种格式，需要转换。
     *
     * 根据参数，若当前的swscontext还可以用，则继续使用；否则释放当前swscontext并使用新参数重新创建一个swscontext。
     * 若pswscontext为NULL，则等同于 sws_getContext()
     */
    pswscontext = sws_getCachedContext(pswscontext, pcodecContext->width, pcodecContext->height, pcodecContext->pix_fmt,
                                       pcodecContext->width, pcodecContext->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
    
    
    SDL_Window    *pwindow   = NULL;
    SDL_Renderer  *prenderer = NULL;
    SDL_Texture   *ptexture  = NULL;
    SDL_Rect       rect;
    
    SDL_CreateWindowAndRenderer(pcodecContext->width, pcodecContext->height, SDL_WINDOW_OPENGL,
                                &pwindow, &prenderer);
    ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                 pcodecContext->width, pcodecContext->height);
    rect.x = 0;
    rect.y = 0;
    rect.w = pcodecContext->width;
    rect.h = pcodecContext->height;
    

    
    /**
     * 读取输入流。
     * 该函数只返回存储在文件中的信息，并不保证得到的信息对解码器可用。
     *
     * 成功时返回0，若有错误或到达文件结尾，返回负值。
     */
    while(av_read_frame(pformatContext, &pkt) >= 0)
    {
        /**
         * 当 pkt.buf 为 NULL 时，这个 pkt 只在下一次调用av_read_frame()前或调用
         * avformat_close_input()前可用。否则，这个 pkt 一直可用，因为非NULL时，表示是
         * 引用计数的，pkt本身获取了一个引用。
         * 不论 pkt.buf 是否为 NULL ，当 pkt 不再需要时，都需要使用 av_packet_unref()来释放。
         *
         * 对视频，一个 pkt 正好包含一个 frame; 对于音频，如果每个 frame 是固定大小（如PCM、ADPCM），则一个 pkt 包含
         * 整数个 frame，如果音频frame是可变大小的（如 MPEG音频），则一个 pkt 包含一个 frame。
         *
         * pkt.pts, pkt.dts, pkt.duration会设置为基于 AVStream.time_base为单位的值。
         * pkt.pts 可能为 AV_NOPTS_VALUE.
         */
        
        /* 当前只处理视频流的packet */
        if(pkt.stream_index == videoStream)
        {
            /**
             * NEW API, legacy API avcodec_decode_video2()
             *
             * 给解码器提供裸包数据作为输入
             *
             * pkt的所有者不会改变，仍时调用者。
             * 与旧的API不同，此函数总是会把整个 pkt 消费掉。如果 pkt 含有多个 frame，则在下一次调用
             * 此函数前，需要多次调用 avcodec_receive_frame()。
             * pkt 可以为NULL，为NULL时，表示这是一个flush packet，代表到达流的结尾。
             * 如果解码器中还有frame,则在发送flush packet后，还是会返回这些frame。
             *
             * 成功时返回0，出错时返回负值。
             *  AVERROR(EAGAIN) : 需要receive读出已有的输出
             *  AVERROR_EOF : 解码器已被flush, 不能再发送新的pkt
//             */
//            iret = avcodec_send_packet(pcodecContext, &pkt);
//            for(;;)
//            {
//                /**
//                 * 返回解码器中已解码的数据。
//                 *
//                 * pframe：由解码器分配的引用计数的音频或视频frame。这个函数做的第一件事就是
//                 *  av_frame_unref(pframe)。
//                 *
//                 * 成功时返回0， 出错时返回负值。
//                 *   AVERROR(EAGAIN) : 需要再次send新的输入
//                 *   AVERROR_EOF : 解码器已被flush，不会再输出frame
//                 */
//                iret = avcodec_receive_frame(pcodecContext, pframe);
//                if(iret == 0)
//                {
//                    sws_scale(pswscontext, (uint8_t const* const*)pframe->data, pframe->linesize, 0,
//                              pcodecContext->height, pframeYUV->data, pframeYUV->linesize);
//                    
//                    SDL_UpdateTexture(ptexture, &rect, pframeYUV->data[0], pframeYUV->linesize[0]);
//                    SDL_RenderClear(prenderer);
//                    SDL_RenderCopy(prenderer, ptexture, &rect, &rect);
//                    SDL_RenderPresent(prenderer);
//                }
//                else
//                {
//                    break;
//                }
//            }
        }else if(pkt.stream_index == audioStream)
        {
            iret = avcodec_send_packet(paudiocodecContext, &pkt);
            for(;;)
            {
                iret = avcodec_receive_frame(paudiocodecContext, pframe);
                if(iret == 0)
                {
                    pswrcontext = swr_alloc();
                    pswrcontext = swr_alloc_set_opts(pswrcontext,
                                                     audio_wanted_frame.channel_layout,(enum AVSampleFormat)audio_wanted_frame.format, audio_wanted_frame.sample_rate,
                                                     pframe->channel_layout,(enum AVSampleFormat)pframe->format, pframe->sample_rate,
                                                     0, NULL);
                    
                    printf("out_ch_layout=%llu,out_sample_fmt=%d,out_sample_rate=%d \n",audio_wanted_frame.channel_layout,audio_wanted_frame.format,audio_wanted_frame.sample_rate);
                    
                    printf("in_ch_layout=%lld,in_sample_fmt=%d,in_sample_rate=%d \n",pframe->channel_layout,pframe->format,pframe->sample_rate);
                    
                    
                    if (!pswrcontext || swr_init(pswrcontext) < 0)
                    {
                        continue;
                    }else{
                        printf("swr init success\n");
                    }
                    
                    
                    int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(pswrcontext, pframe->sample_rate) + pframe->nb_samples,audio_wanted_frame.sample_rate, audio_wanted_frame.format, (enum AVRounding)1);
                    printf("dst_nb_sample %d\n",dst_nb_samples);
                    
                    int len2 = swr_convert(pswrcontext, &paudio_buff, dst_nb_samples, (const uint8_t**)pframe->data, pframe->nb_samples);
                    
                    if (len2 < 0) {
                        printf("swr convert failed\n");
                        return -1;
                    }else{
                        printf("swr convert %d \n",len2);
                    }
                    
                    audio_buff_size = audio_wanted_frame.channels * len2 * av_get_bytes_per_sample(( enum AVSampleFormat)audio_wanted_frame.format);
                    
                    printf("audio buffer size =%d\n",audio_buff_size);
                    
                    //放及取数据的这部分烂死
                    for(;;)
                    {
                        while (g_audiobuf.size > 0) {
                            printf("sdl delay \n");
                            SDL_Delay(1);
                        }
                        
                        printf(" decode lock \n");
                        SDL_LockMutex(g_audiobuf.mutex);
                        printf("decoded frame size =%d\n",audio_buff_size);
                        memcpy(g_audiobuf.buf, paudio_buff, audio_buff_size);
                        g_audiobuf.size = audio_buff_size;
                        g_audiobuf.pos = g_audiobuf.buf;
                        lastBufIndex=0;
                        
                        printf(" decode cond \n");
                        // Restart one of the threads that are waiting on the condition variable.
                        SDL_CondSignal(g_audiobuf.cond);
                        printf(" decode unlock \n");
                        SDL_UnlockMutex(g_audiobuf.mutex);
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        av_packet_unref(&pkt);
    }
    
    SDL_Quit();
    
    av_free(buffer);
    av_frame_free(&pframe);
    av_frame_free(&pframeYUV);
    avcodec_free_context(&pcodecContext);
    avformat_close_input(&pformatContext);
    
    return 0;
}

