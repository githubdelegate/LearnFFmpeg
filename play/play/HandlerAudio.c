//
//  HandlerAudio.c
//  play
//
//  Created by zhangyun on 29/08/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "HandlerAudio.h"


int prepare_audio(PlayProies *pp){
    
    pp->pAudioStrream = pp->pForamtCtx->streams[pp->audioStreamIndex];
    pp->pACodecCtx = pp->pAudioStrream->codec;
    pp->pACodec = avcodec_find_decoder(pp->pAudioStrream->codecpar->codec_id);
    if (pp->pACodec == NULL) {
        printf("find audio codec failed\n");
        return -1;
    }
    avcodec_open2(pp->pACodecCtx, pp->pACodec, NULL);
    return 0;
}


void audio_callback(void *userdata,uint8_t *stream,int len){

    
    
    PlayProies *pp = (PlayProies *)userdata;
    SDL_memset(stream, 0, len);
    int audio_size = 0;
    int send_data_size;
    
    while (len > 0) {
        if (pp->audioBufIndex >= pp->audioBufSize) {
            audio_size = audio_decode_frame(pp);
            if (audio_size < 0) {
                pp->audioBufSize = 1024;
                memset(pp->audioBuf, 0, pp->audioBufSize);
            }else{
                pp->audioBufSize = audio_size;
            }
            pp->audioBufIndex = 0;
        }
        
        send_data_size = pp->audioBufSize - pp->audioBufIndex;
        if (len < send_data_size) {
            send_data_size = len;
        }
        
        SDL_MixAudio(stream, (uint8_t *)pp->audioBuf, send_data_size, SDL_MIX_MAXVOLUME);
        len -= send_data_size;
        stream += send_data_size;
        pp->audioBufIndex += send_data_size;
    } // while
}


int audio_decode_frame(PlayProies *pp){
    
    uint8_t *audio_buf = pp->audioBuf;
    AVPacket packet;
    AVFrame *pFrame;
    enum AVSampleFormat dst_sample_fmt;
    uint64_t dst_channel_layout;
    uint64_t dst_nb_samples;
    int convert_len;
    SwrContext *swr_ctx = NULL;
    int data_size;
    int ret = 0;
    
    pFrame = av_frame_alloc();
    if (packet_queue_get(pp->audioQueue, &packet, 1) < 0) {
        printf("get audio packet failed\n");
        return -1;
    }
    
    if (packet.pts != AV_NOPTS_VALUE) {
        pp->audio_clock = packet.pts * av_q2d(pp->pAudioStrream->time_base);
    }
    
    ret = avcodec_send_packet(pp->pACodecCtx, &packet);
    if (ret < 0) {
        printf("send audio packet error \n");
        return -1;
    }
    
    ret = avcodec_receive_frame(pp->pACodecCtx, pFrame);
    if (ret < 0) {
        printf("audio receive frame failed\n");
        return -1;
    }
    
    if (packet.pts != AV_NOPTS_VALUE) {
        pp->audio_clock = packet.pts * av_q2d(pp->pAudioStrream->time_base);
        printf("packet pst=%lld--timebase=%d/%d--q2d=%f",packet.pts,pp->pAudioStrream->time_base.num,pp->pAudioStrream->time_base.den,av_q2d(pp->pAudioStrream->time_base));
        printf("音频时间：ps->audio_clock = %lf\n", pp->audio_clock);
    }
    
    if (pFrame->channels > 0 && pFrame->channel_layout == 0 ) {
        pFrame->channel_layout = av_get_default_channel_layout(pFrame->channels);
    }else if(pFrame->channels == 0 && pFrame->channel_layout > 0){
        pFrame->channels = av_get_channel_layout_nb_channels(pFrame->channel_layout);
    }
    
    dst_sample_fmt =  AV_SAMPLE_FMT_S16;
    dst_channel_layout = av_get_default_channel_layout(pFrame->channels);
    
    swr_ctx = swr_alloc_set_opts(swr_ctx, dst_channel_layout, dst_sample_fmt, pFrame->sample_rate,pFrame->channel_layout, (enum AVSampleFormat)pFrame->format, pFrame->sample_rate, 0, NULL);
    
    if (swr_ctx == NULL ) {
        printf("swr ctx alloc failed\n");
        return -1;
    }
    
    if (swr_init(swr_ctx) < 0) {
        printf("swr ctx init failed\n");
        return -1;
    }
    
    dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, pFrame->sample_rate) + pFrame->nb_samples, pFrame->sample_rate, pFrame->sample_rate, (enum AVRounding)(1));
    
    
    
    
    
    
    
    
    
    
    return 0;
}

int play_audio(PlayProies *pp){
    
    
    SDL_AudioSpec  wanted_spec;
    wanted_spec.freq = pp->pACodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = pp->pACodecCtx->channels;
    wanted_spec.silence = 0;
    //Audio buffer size in samples (
    wanted_spec.samples = 1024;
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = pp;
    
    SDL_AudioSpec real_spec;
    
    int ret = SDL_OpenAudio(&wanted_spec, &real_spec);
    if (ret < 0) {
        printf("open audio failed\n");
        return -1;
    }
    
    pp->wanted_frame.format = AV_SAMPLE_FMT_S16;
    pp->wanted_frame.sample_rate = real_spec.freq;
    pp->wanted_frame.channel_layout = av_get_default_channel_layout(real_spec.channels);
    pp->wanted_frame.channels = real_spec.channels;
    
    SDL_PauseAudio(0);
    return 0;
}
