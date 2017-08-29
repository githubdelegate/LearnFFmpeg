////
////  VideoPlayer2.c
////  player2
////
////  Created by zhangyun on 22/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "VideoPlayer2.h"
//
//
//
//#ifndef ffmpeg_h
//#define ffmpeg_h
//
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/avfilter.h>
//#include <libswscale/swscale.h>
//#include <libswresample/swresample.h>
//#include <libavutil/avutil.h>
//#include <libavutil/samplefmt.h>
//#include <libavutil/pixdesc.h>
//#include <libavutil/mathematics.h>
//#include <libavutil/opt.h>
//
////#include "avcodec.h"
////#include "avformat.h"
////#include "avfilter.h"
////#include "swscale.h"
////#include "swresample.h"
////#include "samplefmt.h"
////#include "avutil.h"
////#include "opt.h"
////#include "pixdesc.h"
////#include "mathematics.h"
//#endif
//
//#ifndef sdl_h
//#define sdl_h
//
//#include <SDL2/SDL.h>
////#include "SDL.h"
//#include <SDL2/SDL_thread.h>
////#include "SDL_thread.h"
//#endif
//
//
//#define SDL_AUDIO_BUFFER_SIZE 1024
//#define MAX_AUDIO_FRAME_SIZE 192000
//
//typedef struct PacketQueue{
//    
//    AVPacketList *first_pkt,*last_pkt;
//    int nb_packets;
//    int size;
//    SDL_mutex *mutex;
//    SDL_cond *cond;
//}PacketQueue;
//
//PacketQueue audioq;
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
//    AVPacketList *pkt1 = NULL;
//    AVPacket *copyPkt = av_malloc(sizeof(AVPacket));
//    
//    if (av_packet_ref(copyPkt, pkt) < 0) {
//        return -1;
//    }
//    pkt1 = av_malloc(sizeof(AVPacketList));
//    if (!pkt1) {
//        return -1;
//    }
//    pkt1->pkt = *copyPkt;
//    pkt1->next = NULL;
//    //Lock the mutex.
//    SDL_LockMutex(q->mutex);
//    if (!q->last_pkt) { // 说明是空链表
//        q->first_pkt = pkt1;
//    }else{
//        q->last_pkt->next = pkt1;
//    }
//    q->last_pkt = pkt1;
//    q->nb_packets++;
//    q->size += pkt1->pkt.size;
//    SDL_CondSignal(q->cond);
//    SDL_UnlockMutex(q->mutex);
//    return 0;
//}
//
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
//            SDL_CondWait(q->cond, q->mutex);
//        }
//    } // for
//    
//    SDL_UnlockMutex(q->mutex);
//    return ret;
//}
//
//SwrContext *pswrcontext = NULL;
//AVFrame audio_wanted_frame;
//
//
//uint8_t audio_buff[(192000 * 3) / 2];
//uint8_t *paudio_buff = audio_buff;
//static int audio_decode_frame(AVCodecContext *aCodecCtx,
//                              uint8_t *audio_buf,int buf_size){
//    
//    static AVPacket pkt;
//    static uint8_t *audio_pkt_data = NULL;
//    static int audio_pkt_size = 0;
//    static AVFrame *frame = NULL;
//    
//    int len1, data_size = 0;
//    int ret = -1;
//    
//    frame = av_frame_alloc();
//    
//    for (;;) {
//        // 如果有数据 就开始解码
//        while(audio_pkt_size > 0) {
//            // 发送数据给解码器 解码
//            ret = avcodec_send_packet(aCodecCtx, &pkt);
//            
//            if (ret < 0) {// 解码失败，直接返回。
//                fprintf(stderr, "send packet failed");
//                return -1;
//            }
//            while(ret >= 0) { // 解码成功，读取解码数据
//                ret = avcodec_receive_frame(aCodecCtx, frame);
//                 if (ret == 0){ // 读取数据成功，把数据copy过去。
//                     pswrcontext = swr_alloc_set_opts(pswrcontext,audio_wanted_frame.channel_layout,audio_wanted_frame.format, audio_wanted_frame.sample_rate,av_get_default_channel_layout(frame->channels),frame->format,frame->sample_rate,0, NULL);
//                     
//                     if (!pswrcontext) {
//                         fprintf(stderr, "init swr ctx failed");
//                         continue;
//                     }
//                     
//                     int ret = swr_init(pswrcontext);
//                     if (ret < 0) {
//                         fprintf(stderr, "swr init failed");
//                         continue;
//                     }
//                     
//                     //Rescale a 64-bit integer with specified rounding.
//                     int stn_nb_samples = av_rescale_rnd(swr_get_delay(pswrcontext,frame->sample_rate)+frame->nb_samples, audio_wanted_frame.sample_rate, audio_wanted_frame.format,AV_ROUND_INF);
//                     //number of samples output per channe
//                     int len = swr_convert(pswrcontext, &paudio_buff, stn_nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
//                     
//                     int audio_buf_size = audio_wanted_frame.channels * len * av_get_bytes_per_sample(audio_wanted_frame.format);
//                     
////                    // handler audio frame
////                    data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame->nb_samples, aCodecCtx->sample_fmt, 1);
//                     
//                     if (audio_buf_size <= 0) {
//                         continue;
//                     }
//                    memcpy(audio_buf,paudio_buff, audio_buf_size);
////                    if (data_size <= 0) {
////                        continue;
////                    }
//                    return audio_buf_size;
//                } // if
//            } // while
//        } // while
//        if (pkt.data) {
//            av_packet_unref(&pkt);
//        }
//        
//        if (quit) {
//            return -1;
//        }
//        
//        if (packet_queue_get(&audioq, &pkt, 1) < 0) {
//            return -1;
//        }
//        audio_pkt_data = pkt.data;
//        audio_pkt_size = pkt.size;
//    } // for
//}
//
//static void audio_callback(void *userdata,Uint8 *stream,int len){
//    AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
//    int len1,audio_size;
//    
//    static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE *3) / 2];
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
//}
//
//
//int playVideo()
//{
//    AVFormatContext *pformatContext = NULL;
//    AVCodecContext  *pcodecContext  = NULL;
//    AVCodec         *pcodec         = NULL;
//    const char      *pinputfile     = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//;
//     int     videoStream    = -1;
//     int audioStream = -1;
//    int              iret           = 0;
//    
//    
//    if(SDL_Init(SDL_INIT_VIDEO))
//    {
//        
//        exit(1);
//    }
//    /**
//     * 初始化libavformat库，并注册所有的合成器(muxer)、分离器(demuxer)和协议(protocol)
//     */
//    av_register_all();
//    
//    /**
//     *打开输入流，并读取流的头部(header)。
//     *`pformatContext`可以是通过`avformat_alloc_context()`分配得到的`AVFormatContext`，也可以是`NULL`，如果是`NULL`，则该函数会给其分配空间并赋值。
//     *   第三个参数是指定输入格式，直接用`NULL`，则自动检测。
//     *   第四个参数是`AVDictionary **`，一般直接用`NULL`，除非给`AVFormatContext`传递一些私有参数。
//     *此函数只是打开输入流，相应的解码器并没有打开。
//     *打开的流必须使用`avforamt_close_input()`关闭。
//     */
//    iret = avformat_open_input(&pformatContext, pinputfile, NULL, NULL);
//    if(iret < 0)
//    {
////        cerr << "avforamt_open_input fail:" << iret << endl;
//        exit(1);
//    }
//    /**
//     * 获取流的信息，一些文件格式（如MPEG）没有头部(header)，可以使用这个函数来获取流的信息
//     */
//    avformat_find_stream_info(pformatContext, NULL);
//    
//    /**
//     * 打印输入或输出格式的详细信息，第4个参数指定是输入(0)还是输出(1)。
//     */
//    av_dump_format(pformatContext, 0, pinputfile, 0);
//    
//    /**
//     * 查找视频流
//     */
//    for(unsigned int i = 0; i < pformatContext->nb_streams; ++i)
//    {
//        /**
//         * 使用 AVStream 中的 codecpar 字段，codec 字段被标记为 deprecated
//         */
//        if(pformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0)
//        {
//            videoStream = i;
//        }else if(pformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audioStream < 0){
//            audioStream = i;
//        }
//    }
//
//    if (audioStream < 0) {
//        fprintf(stderr, "audio stream failed");
//        return  -1;
//    }
//    
//    if (videoStream < 0) {
//        fprintf(stderr, "video stream failed");
//        return -1;
//    }
//    
//    /**
//     * 使用引用计数
//     */
//    AVDictionary *opts = NULL;
//    av_dict_set(&opts, "refconted_frames" , "1", 0);
//    
//    /**
//     * 创建一个AVCodecContext,并设置其中的字段为默认值。
//     *
//     * 新创建一个AVCodecContext，AVStream 中的 codec 被标记为 deprecated
//     *
//     * 必须使用 avcodec_free_context() 释放。
//     */
//    pcodecContext = avcodec_alloc_context3(NULL);
//    
//    /**
//     * 根据 codecpar 中的值填充 codeccontext 中的相应字段。
//     * 返回负值时表示失败。
//     */
//    avcodec_parameters_to_context(pcodecContext, pformatContext->streams[videoStream]->codecpar);
//    
//    /**
//     * 使用 codeid 找到已注册的解码器
//     */
//    pcodec = avcodec_find_decoder(pcodecContext->codec_id);
//    
//    /**
//     * 初始化 codeccontext，以使用指定的 codec。
//     */
//    avcodec_open2(pcodecContext, pcodec, &opts);
//    
//    AVPacket    pkt;
//    AVFrame    *pframe;
//    AVFrame    *pframeYUV;
//    struct SwsContext *pswscontext = NULL;
//    int         numBytes    = 0;
//    uint8_t    *buffer      = NULL;
//    
//    /**
//     * 初始化 AVPacket 中的可选字段为默认值。
//     * 结构中的 data 和 size 未设置，需要单独初始化。
//     *
//     * AVPacket 结构中的一些成员：
//     *    AVBufferRef *buf;  使用引用计数时保存packet数据的引用;若为NULL，则表示packet数据不是引用计数。
//     *    int64_t pts;
//     *    int64_t dts;
//     *    uint8_t *data;
//     *    int   size;
//     *    int   stream_index; 用来判断这个packet是音频还是视频或字幕等。
//     *
//     */
//    av_init_packet(&pkt);
//    
//    pkt.data = NULL;
//    pkt.size = 0;
//    
//    pframe = av_frame_alloc();
//    
//    pframeYUV = av_frame_alloc();
//    /**
//     * 返回按指定格式保存图片时需要的大小。第4个参数指定 linesize alignment 。
//     */
//    numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pcodecContext->width,
//                                        pcodecContext->height, 4);
//    buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
//    /**
//     * 按指定的格式，用提供的数组来填充 data 和 linesize 。
//     * 当 buffer 为 NULL 时，返回 buffer 需要的大小。
//     *
//     * 返回 buffer 需要的大小，负值时表示失败。
//     */
//    av_image_fill_arrays(pframeYUV->data, pframeYUV->linesize, buffer, AV_PIX_FMT_YUV420P,
//                         pcodecContext->width, pcodecContext->height, 4);
//    
//    /**
//     * 用来进行缩放或转换操作。参见sws_scale()。
//     *
//     * 使用SDL来显示视频中的每一帧图像。显示时使用YUV420P格式，但视频流中解码后的格式可能不是这种格式，需要转换。
//     *
//     * 根据参数，若当前的swscontext还可以用，则继续使用；否则释放当前swscontext并使用新参数重新创建一个swscontext。
//     * 若pswscontext为NULL，则等同于 sws_getContext()
//     */
//    pswscontext = sws_getCachedContext(pswscontext, pcodecContext->width, pcodecContext->height, pcodecContext->pix_fmt,
//                                       pcodecContext->width, pcodecContext->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
//    
//    
//    SDL_Window    *pwindow   = NULL;
//    SDL_Renderer  *prenderer = NULL;
//    SDL_Texture   *ptexture  = NULL;
//    SDL_Rect       rect;
//    
//    SDL_CreateWindowAndRenderer(pcodecContext->width, pcodecContext->height, SDL_WINDOW_OPENGL,
//                                &pwindow, &prenderer);
//    ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
//                                 pcodecContext->width, pcodecContext->height);
//    rect.x = 0;
//    rect.y = 0;
//    rect.w = pcodecContext->width;
//    rect.h = pcodecContext->height;
//    
//    
//    
//    //--------------------------audio-------------
//    
////    AVDictionary *opts = NULL;
////    av_dict_set(&opts, "refconted_frames" , "1", 0);
//    AVCodecContext *audioCtx = NULL;
//    AVCodec *audioCodec = NULL;
//    audioCtx = avcodec_alloc_context3(NULL);
//    avcodec_parameters_to_context(audioCtx, pformatContext->streams[audioStream]->codecpar);
//    audioCodec = avcodec_find_decoder(audioCtx->codec_id);
//    avcodec_open2(audioCtx,audioCodec,NULL);
//    
//    SDL_AudioSpec wanted_spec, got_spec;
//    wanted_spec.freq = audioCtx->sample_rate;
//    wanted_spec.format = AUDIO_S16SYS;
//    wanted_spec.channels = audioCtx->channels;
//    wanted_spec.silence = 0;
//    wanted_spec.samples = 1024;
//    wanted_spec.callback = audio_callback;
//    wanted_spec.userdata = audioCtx;
//    if(SDL_OpenAudio(&wanted_spec, &got_spec) < 0){
//            exit(1);
//    }
//    
//    audio_wanted_frame.format = AV_SAMPLE_FMT_S16;
//    audio_wanted_frame.sample_rate = got_spec.freq;
//    audio_wanted_frame.channel_layout = av_get_default_channel_layout(got_spec.channels);
//    audio_wanted_frame.channels = got_spec.channels;
//    
//    packet_queue_init(&audioq);
//    SDL_PauseAudio(0);
//    
//    //----------------------------------
//    
//    /**
//     * 读取输入流。
//     * 该函数只返回存储在文件中的信息，并不保证得到的信息对解码器可用。
//     *
//     * 成功时返回0，若有错误或到达文件结尾，返回负值。
//     */
//    while(av_read_frame(pformatContext, &pkt) >= 0){
//        /**
//         * 当 pkt.buf 为 NULL 时，这个 pkt 只在下一次调用av_read_frame()前或调用
//         * avformat_close_input()前可用。否则，这个 pkt 一直可用，因为非NULL时，表示是
//         * 引用计数的，pkt本身获取了一个引用。
//         * 不论 pkt.buf 是否为 NULL ，当 pkt 不再需要时，都需要使用 av_packet_unref()来释放。
//         *
//         * 对视频，一个 pkt 正好包含一个 frame; 对于音频，如果每个 frame 是固定大小（如PCM、ADPCM），则一个 pkt 包含
//         * 整数个 frame，如果音频frame是可变大小的（如 MPEG音频），则一个 pkt 包含一个 frame。
//         *
//         * pkt.pts, pkt.dts, pkt.duration会设置为基于 AVStream.time_base为单位的值。
//         * pkt.pts 可能为 AV_NOPTS_VALUE.
//         */
//        
//        /* 当前只处理视频流的packet */
//        if(pkt.stream_index == videoStream){
//            /**
//             * NEW API, legacy API avcodec_decode_video2()
//             *
//             * 给解码器提供裸包数据作为输入
//             *
//             * pkt的所有者不会改变，仍时调用者。
//             * 与旧的API不同，此函数总是会把整个 pkt 消费掉。如果 pkt 含有多个 frame，则在下一次调用
//             * 此函数前，需要多次调用 avcodec_receive_frame()。
//             * pkt 可以为NULL，为NULL时，表示这是一个flush packet，代表到达流的结尾。
//             * 如果解码器中还有frame,则在发送flush packet后，还是会返回这些frame。
//             *
//             * 成功时返回0，出错时返回负值。
//             *  AVERROR(EAGAIN) : 需要receive读出已有的输出
//             *  AVERROR_EOF : 解码器已被flush, 不能再发送新的pkt
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
//                else{
//                    break;
//                }
//            }
//        }
//        else if (pkt.stream_index == 1){
//            packet_queue_put(&audioq,&pkt);
//        }
//        av_packet_unref(&pkt);
//    }
//    
//    SDL_Quit();
//    
//    av_free(buffer);
//    av_frame_free(&pframe);
//    av_frame_free(&pframeYUV);
//    avcodec_free_context(&pcodecContext);
//    avformat_close_input(&pformatContext);
//    
//    return 0;
//}
