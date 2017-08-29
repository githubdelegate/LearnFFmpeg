//////
//////  AudioPlayer.c
//////  player2
//////
//////  Created by zhangyun on 22/08/2017.
//////  Copyright © 2017 zhangyun. All rights reserved.
//////
//
//#include "AudioPlayer.h"
//
//#ifndef ffmpeg_h
//#define ffmpeg_h
//
//
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/avfilter.h>
//#include <libswscale/swscale.h>
//#include <libavutil/channel_layout.h>
//#include <libavutil/samplefmt.h>
//#include <libswresample/swresample.h>
//
////#include "avcodec.h"
////#include "avformat.h"
////#include "avfilter.h"
////#include "swscale.h"
//#endif
//
//
//#ifndef sdl_h
//#define sdl_h
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_thread.h>
////#include "SDL.h"
////#include "SDL_thread.h"
//#endif
//
//
//#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
//
//
//static Uint8 *audio_chunk;
//static Uint32 audio_len;
//static Uint8 *audio_pos;
//
//void fill_audio(void *udate,Uint8 *stream,int len){
//    
//    SDL_memset(stream, 0, len);
//    if (audio_len == 0) {
//        return;
//    }
//    
//    len = (len > audio_len ? audio_len : len);
//    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
//    audio_pos +=len;
//    audio_len -= len;
//}
//
//int playAudio(){
//    
//    AVFormatContext *pFormatCtx = NULL;
//    int ret = -1;
//    char *filePath = "/Users/zhangyun/Downloads/simplest_ffmpeg_audio_player-master/simplest_ffmpeg_audio_player/xiaoqingge.mp3";
//    av_register_all();
//    pFormatCtx = avformat_alloc_context();
//    ret =  avformat_open_input(&pFormatCtx, filePath, NULL, NULL);
//    if (ret < 0) {
//        printf(" couldn't open inout file");
//        return -1;
//    }
//    
//    ret = avformat_find_stream_info(pFormatCtx, NULL);
//    if (ret < 0) {
//        printf("find stream info failed");
//        return -1;
//    }
//    
//     av_dump_format(pFormatCtx, 0, filePath,0);
//    
//    int audioStream = -1;
//    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
//        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
//            audioStream = i;
//            break;
//        }
//    }
//    
//    if (audioStream < 0) {
//        printf("audio strea index failed");
//        return -1;
//    }
//    
//    AVCodecContext *pCodexCtx = pFormatCtx->streams[audioStream]->codec;
//    AVCodec *pCodec = avcodec_find_decoder(pCodexCtx->codec_id);
//    if (!pCodec) {
//        printf("could not find codec ");
//        return -1;
//    }
//    
//    ret = avcodec_open2(pCodexCtx, pCodec, NULL);
//    if (ret < 0) {
//        printf("could not open codec");
//        return -1;
//    }
//    
//    AVPacket *packt = (AVPacket *)av_malloc(sizeof(AVPacket));
//    av_init_packet(packt);
//    
//    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
//    
//    int out_nb_samples = pCodexCtx->frame_size;
//    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
//    int out_sample_rate = 44100;
//    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
//    
//    int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
//    
//    uint8_t *outbuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
//    
//    AVFrame *pFrame = av_frame_alloc();
//    
//    SDL_Init(SDL_INIT_EVERYTHING);
//    
//    SDL_AudioSpec wanted_spec;
//    wanted_spec.freq = out_sample_rate;
//    wanted_spec.format = AUDIO_S16SYS;
//    wanted_spec.channels = out_channels;
//    wanted_spec.silence = 0;
//    wanted_spec.callback = fill_audio;
//    wanted_spec.userdata = pCodexCtx;
//    wanted_spec.samples = out_nb_samples;
//    if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
//        printf("open audio failed");
//        return -1;
//    }
//    
//    
//    int64_t in_channel_layout;
//    in_channel_layout = av_get_default_channel_layout(pCodexCtx->channels);
//    
//    struct SwsContext *au_convert_ctx;
//    au_convert_ctx = swr_alloc();
//    au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate, in_channel_layout, pCodexCtx->sample_fmt, pCodexCtx->sample_rate, 0, NULL);
//    printf("out_ch_layout=%llu,out_sample_fmt=%d,out_sample_rate=%d \n",out_channel_layout,out_sample_fmt,out_sample_rate);
//    printf("in_ch_layout=%lld,in_sample_fmt=%d,in_sample_rate=%d \n",in_channel_layout,pCodexCtx->sample_fmt,pCodexCtx->sample_rate);
//
//    
//   ret = swr_init(au_convert_ctx);
//    if (ret < 0) {
//        printf("swr init failed");
//        return -1;
//    }
//    
//    
//    SDL_PauseAudio(0);
//    
//    int got_picture;
//    int index = 0;
//    
//    while (av_read_frame(pFormatCtx, packt) >= 0) {
//        if (packt->stream_index == audioStream) {
//            ret = avcodec_decode_audio4(pCodexCtx, pFrame, &got_picture, packt);
//            if (ret < 0) {
//                printf("decode error");
//                return -1;
//            }
//            
//            if (got_picture > 0) {
//                swr_convert(au_convert_ctx, &outbuffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data, pFrame->nb_samples);
//                printf("index:%5d\t pts:%lld\t packet size:%d\n",index,packt->pts,packt->size);
//                index++;
//            }else{
//                printf("got picture failed");
//            }
//            
//            while (audio_len > 0) {
//                printf("sdl delay");
//                SDL_Delay(1);
//            }
//            audio_chunk = (Uint8 *)outbuffer;
//            audio_len = out_buffer_size;
//            audio_pos = audio_chunk;
//        }// if
//            av_free_packet(packt);
//    } // while
//    
//    swr_free(&au_convert_ctx);
//    SDL_CloseAudio();
//    SDL_Quit();
//    
//    av_free(outbuffer);
//    avcodec_close(pCodexCtx);
//    avformat_close_input(&pFormatCtx);
//    return 0;
//}
