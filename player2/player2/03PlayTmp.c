////
////  03PlayTmp.c
////  player2
////
////  Created by zhangyun on 17/08/2017.
////  Copyright © 2017 zhangyun. All rights reserved.
////
//
//#include "03PlayTmp.h"
//
//#include <stdio.h>
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
//#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
//#define av_frame_alloc avcodec_alloc_frame
//#define av_frame_free avcodec_free_frame
//#endif
//
//#define SDL_AUDIO_BUFFER_SIZE 1024
//#define MAX_AUDIO_FRAME_SIZE 192000
//
//
//typedef struct AudioParams {
//    int freq;
//    int channels;
//    int64_t channel_layout;
//    enum AVSampleFormat fmt;
//    int frame_size;
//    int bytes_per_sec;
//} AudioParams;
//int sample_rate, nb_channels;
//int64_t channel_layout;
//AudioParams audio_hw_params_tgt;
//AudioParams audio_hw_params_src;
//
//int resample(AVFrame * af, uint8_t * audio_buf, int * audio_buf_size);
//
//struct SwrContext * swr_ctx = NULL;
//
//int resample(AVFrame * af, uint8_t * audio_buf, int * audio_buf_size)
//{
//    int data_size = 0;
//    int resampled_data_size = 0;
//    int64_t dec_channel_layout;
//    data_size = av_samples_get_buffer_size(NULL,
//                                           av_frame_get_channels(af),
//                                           af->nb_samples,
//                                           af->format,
//                                           1);
//    
//    dec_channel_layout =
//    (af->channel_layout && av_frame_get_channels(af) == av_get_channel_layout_nb_channels(af->channel_layout)) ?
//    af->channel_layout : av_get_default_channel_layout(av_frame_get_channels(af));
//    if( 	af->format 		 	!= audio_hw_params_src.fmt 				||
//       af->sample_rate 	!= audio_hw_params_src.freq 	 		||
//       dec_channel_layout 	!= audio_hw_params_src.channel_layout 	||
//       !swr_ctx) {
//        swr_free(&swr_ctx);
//        swr_ctx = swr_alloc_set_opts(NULL,
//                                     audio_hw_params_tgt.channel_layout, audio_hw_params_tgt.fmt, audio_hw_params_tgt.freq,
//                                     dec_channel_layout, af->format, af->sample_rate,
//                                     0, NULL);
//        if (!swr_ctx || swr_init(swr_ctx) < 0) {
//            av_log (NULL, AV_LOG_ERROR,
//                   "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
//                   af->sample_rate, av_get_sample_fmt_name(af->format), av_frame_get_channels(af),
//                   audio_hw_params_tgt.freq, av_get_sample_fmt_name(audio_hw_params_tgt.fmt), audio_hw_params_tgt.channels);
//            swr_free(&swr_ctx);
//            return -1;
//        }
//        printf("swr_init\n");
//        audio_hw_params_src.channels = av_frame_get_channels(af);
//        audio_hw_params_src.fmt = af->format;
//        audio_hw_params_src.freq = af->sample_rate;
//    }
//    
//    if (swr_ctx) {
//        const uint8_t **in = (const uint8_t **)af->extended_data;
//        uint8_t **out = &audio_buf;
//        int out_count = (int64_t)af->nb_samples * audio_hw_params_tgt.freq / af->sample_rate + 256;
//        int out_size  = av_samples_get_buffer_size(NULL, audio_hw_params_tgt.channels, out_count, audio_hw_params_tgt.fmt, 0);
//        int len2;
//        if (out_size < 0) {
//            av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
//            return -1;
//        }
//        av_fast_malloc(&audio_buf, audio_buf_size, out_size);
//        if (!audio_buf)
//            return AVERROR(ENOMEM);
//        len2 = swr_convert(swr_ctx, out, out_count, in, af->nb_samples);
//        if (len2 < 0) {
//            av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
//            return -1;
//        }
//        if (len2 == out_count) {
//            av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
//            if (swr_init(swr_ctx) < 0)
//                swr_free(&swr_ctx);
//        }
//        resampled_data_size = len2 * audio_hw_params_tgt.channels * av_get_bytes_per_sample(audio_hw_params_tgt.fmt);
//    } else {
//        audio_buf = af->data[0];
//        resampled_data_size = data_size;
//    }
//    
//    return resampled_data_size;
//}
//
//static void sigterm_handler(int sig)
//{
//    exit(123);
//}
//
//typedef struct PacketQueue {
//    AVPacketList *first_pkt, *last_pkt;
//    int nb_packets;
//    int size;
//    SDL_mutex *mutex;
//    SDL_cond *cond;
//} PacketQueue;
//
//static PacketQueue audioq;
//
//static int quit = 0;
//
//static void packet_queue_init(PacketQueue *q) {
//    memset(q, 0, sizeof(PacketQueue));
//    q->mutex = SDL_CreateMutex();
//    q->cond = SDL_CreateCond();
//}
//static int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
//    
//    AVPacketList *pkt1;
//    if(av_dup_packet(pkt) < 0) {
//        return -1;
//    }
//    pkt1 = av_malloc(sizeof(AVPacketList));
//    if (!pkt1)
//        return -1;
//    pkt1->pkt = *pkt;
//    pkt1->next = NULL;
//    
//    
//    SDL_LockMutex(q->mutex);
//    
//    if (!q->last_pkt)
//        q->first_pkt = pkt1;
//    else
//        q->last_pkt->next = pkt1;
//    q->last_pkt = pkt1;
//    q->nb_packets++;
//    q->size += pkt1->pkt.size;
//    SDL_CondSignal(q->cond);
//    
//    SDL_UnlockMutex(q->mutex);
//    return 0;
//}
//static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
//{
//    AVPacketList *pkt1;
//    int ret;
//    
//    SDL_LockMutex(q->mutex);
//    
//    for(;;) {
//        
//        if(quit) {
//            ret = -1;
//            break;
//        }
//        
//        pkt1 = q->first_pkt;
//        if (pkt1) {
//            q->first_pkt = pkt1->next;
//            if (!q->first_pkt)
//                q->last_pkt = NULL;
//            q->nb_packets--;
//            q->size -= pkt1->pkt.size;
//            *pkt = pkt1->pkt;
//            av_free(pkt1);
//            ret = 1;
//            break;
//        } else if (!block) {
//            ret = 0;
//            break;
//        } else {
//            SDL_CondWait(q->cond, q->mutex);
//        }
//    }
//    SDL_UnlockMutex(q->mutex);
//    return ret;
//}
//
//AVFrame frame;
//static int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {
//    
//    static AVPacket pkt;
//    static uint8_t *audio_pkt_data = NULL;
//    static int audio_pkt_size = 0;
//    
//    int len1, data_size = 0;
//    
//    for(;;) {
//        while(audio_pkt_size > 0) {
//            int got_frame = 0;
//            len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
//            if(len1 < 0) {
//                /* if error, skip frame */
//                audio_pkt_size = 0;
//                break;
//            }
//            audio_pkt_data += len1;
//            audio_pkt_size -= len1;
//            data_size = 0;
//            if(got_frame) {
//                data_size = resample(&frame, audio_buf, &buf_size);
//                // data_size = av_samples_get_buffer_size(NULL,
//                // 		aCodecCtx->channels,
//                // 		frame.nb_samples,
//                // 		aCodecCtx->sample_fmt,
//                // 		1);
////                assert(data_size <= buf_size);
//                // memcpy(audio_buf, frame.data[0], data_size);
//            }
//            if(data_size <= 0) {
//                /* No data yet, get more frames */
//                continue;
//            }
//            // memcpy(audio_buf, frame.data[0], data_size);
//            
//            /* We have data, return it and come back for more later */
//            return data_size;
//        }
//        if(pkt.data)
//            av_free_packet(&pkt);
//        
//        if(quit) {
//            return -1;
//        }
//        
//        if(packet_queue_get(&audioq, &pkt, 1) < 0) {
//            return -1;
//        }
//        audio_pkt_data = pkt.data;
//        audio_pkt_size = pkt.size;
//    }
//}
//
//static void audio_callback(void *userdata, Uint8 *stream, int len) {
//    
//    AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
//    int len1, audio_size;
//    
//    static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
//    static unsigned int audio_buf_size = 0;
//    static unsigned int audio_buf_index = 0;
//    
//    while(len > 0) {
//        if(audio_buf_index >= audio_buf_size) {
//            /* We have already sent all our data; get more */
//            audio_size = audio_decode_frame(aCodecCtx, audio_buf, sizeof(audio_buf));
//            if(audio_size < 0) {
//                /* If error, output silence */
//                audio_buf_size = 1024; // arbitrary?
//                memset(audio_buf, 0, audio_buf_size);
//            } else {
//                audio_buf_size = audio_size;
//            }
//            audio_buf_index = 0;
//        }
//        len1 = audio_buf_size - audio_buf_index;
//        if(len1 > len)
//            len1 = len;
//        memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
//        len -= len1;
//        stream += len1;
//        audio_buf_index += len1;
//    }
//}
//
//int playAudioTmp(){
//    
//    AVFormatContext *pFormatCtx = NULL;
//    int             i, audioStream;
//    AVPacket        packet;
//    
//    AVCodecContext  *aCodecCtxOrig = NULL;
//    AVCodecContext  *aCodecCtx = NULL;
//    AVCodec         *aCodec = NULL;
//    
//    SDL_Event       event;
//    SDL_AudioSpec   wanted_spec, spec;
//    
//    signal(SIGINT , sigterm_handler); /* Interrupt (ANSI).    */
//    signal(SIGTERM, sigterm_handler); /* Termination (ANSI).  */
//    
//    
//    char *file_path = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
//    
//
//    // Register all formats and codecs
//    av_register_all();
//    
//    if(SDL_Init(SDL_INIT_AUDIO)) {
//        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
//        exit(1);
//    }
//    
//    // Open video file
//    if(avformat_open_input(&pFormatCtx, file_path, NULL, NULL)!=0)
//        return -1; // Couldn't open file
//    
//    // Retrieve stream information
//    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
//        return -1; // Couldn't find stream information
//    
//    // Dump information about file onto standard error
//    av_dump_format(pFormatCtx, 0, file_path, 0);
//    
//    // Find the first video stream
//    audioStream=-1;
//    for(i=0; i<pFormatCtx->nb_streams; i++) {
//        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO &&
//           audioStream < 0) {
//            audioStream=i;
//        }
//    }
//    // if(videoStream==-1)
//    //   return -1; // Didn't find a video stream
//    if(audioStream==-1)
//        return -1;
//    
//    aCodecCtxOrig=pFormatCtx->streams[audioStream]->codec;
//    aCodec = avcodec_find_decoder(aCodecCtxOrig->codec_id);
//    if(!aCodec) {
//        fprintf(stderr, "Unsupported codec!\n");
//        return -1;
//    }
//    
//    // Copy context
//    aCodecCtx = avcodec_alloc_context3(aCodec);
//    if(avcodec_copy_context(aCodecCtx, aCodecCtxOrig) != 0) {
//        fprintf(stderr, "Couldn't copy codec context");
//        return -1; // Error copying codec context
//    }
//    
//    avcodec_open2(aCodecCtx, aCodec, NULL);
//    
//    sample_rate = aCodecCtx->sample_rate;
//    nb_channels = aCodecCtx->channels;
//    channel_layout = aCodecCtx->channel_layout;
//    
//    printf("channel_layout=%" PRId64 "\n", channel_layout);
//    printf("nb_channels=%d\n", nb_channels);
//    printf("freq=%d\n", sample_rate);
//    
//    if (!channel_layout || nb_channels != av_get_channel_layout_nb_channels(channel_layout)) {
//        channel_layout = av_get_default_channel_layout(nb_channels);
//        channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
//        printf("correction\n");
//    }
//    
//    // Set audio settings from codec info
//    wanted_spec.freq = sample_rate;
//    wanted_spec.format = AUDIO_S16SYS;
//    wanted_spec.channels = nb_channels;
//    wanted_spec.silence = 0;
//    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
//    wanted_spec.callback = audio_callback;
//    wanted_spec.userdata = aCodecCtx;
//    
//    if(SDL_OpenAudio(&wanted_spec, &spec) < 0) {
//        fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
//        return -1;
//    }
//    printf("freq: %d\tchannels: %d\n", spec.freq, spec.channels);
//    
//    audio_hw_params_tgt.fmt = AV_SAMPLE_FMT_S16;
//    audio_hw_params_tgt.freq = spec.freq;
//    audio_hw_params_tgt.channel_layout = channel_layout;
//    audio_hw_params_tgt.channels =  spec.channels;
//    audio_hw_params_tgt.frame_size = av_samples_get_buffer_size(NULL, audio_hw_params_tgt.channels, 1, audio_hw_params_tgt.fmt, 1);
//    audio_hw_params_tgt.bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params_tgt.channels, audio_hw_params_tgt.freq, audio_hw_params_tgt.fmt, 1);
//    if (audio_hw_params_tgt.bytes_per_sec <= 0 || audio_hw_params_tgt.frame_size <= 0) {
//        printf("size error\n");
//        return -1;
//    }
//    audio_hw_params_src = audio_hw_params_tgt;
//    
//    // audio_st = pFormatCtx->streams[index]
//    packet_queue_init(&audioq);
//    SDL_PauseAudio(0);
//    
//    // Read frames and save first five frames to disk
//    i=0;
//    while(av_read_frame(pFormatCtx, &packet)>=0) {
//        if(packet.stream_index==audioStream) {
//            packet_queue_put(&audioq, &packet);
//        } else {
//            av_free_packet(&packet);
//        }
//        // Free the packet that was allocated by av_read_frame
//        SDL_PollEvent(&event);
//        switch(event.type) {
//            case SDL_QUIT:
//                quit = 1;
//                SDL_Quit();
//                exit(0);
//                break;
//            default:
//                break;
//        }
//        
//    }
//    
//    while(1) SDL_Delay(1000);
//    
//    // Close the codecs
//    avcodec_close(aCodecCtxOrig);
//    avcodec_close(aCodecCtx);
//    
//    // Close the video file
//    avformat_close_input(&pFormatCtx);
//    
//    return 0;
//}
