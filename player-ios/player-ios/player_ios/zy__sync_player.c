//
//  zy__sync_player.c
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "zy__sync_player.h"
#include "zy_packet_queue.h"
#include "zy_play_context.h"
#include "zy_audio.h"
#include "zy_video.h"
#include "zy_filter.h"
#include "zy_ssek.h"


void player_state_init(ZYPlayerContext *ps);
int prepare_common(ZYPlayerContext *ps);
int decode_thread(void *arg);

int playSync7(char *file){
    SDL_Event      event;
    ZYPlayerContext    *ps = NULL;
    //     uint8_t         *state = NULL;
    
    ps = (ZYPlayerContext *)av_malloc(sizeof(ZYPlayerContext));
    if (ps == NULL)
    {
        fprintf(ERR_STREAM, "malloc ps error\n");
    }
    
    player_state_init(ps);
    //    char *file = "/Users/zhangyun/Downloads/BBC.Wild.China.6.Tides.of.Change.美丽中国之六潮汐更迭.双语字幕.HR-HDTV.AC3.960×528.X264-人人影视制作.avi";
    //         char *file = "/Users/zhangyun/Downloads/player_v1.5/naxienian.mp4";
    //    char *file = "/Users/zhangyun/Downloads/output.mp4";
//    char *file = "/Users/zhangyun/Movies/desk/BaiduCLoud/07_Struts2/预习/视频/02.MVC设计思想.avi";
    memcpy(ps->filename,file,strlen(file)+1);
    
    if (prepare_common(ps) != 0)
    {
        fprintf(ERR_STREAM, "prepare common error\n");
        exit(-1);
    }
    
    av_dump_format(ps->pformat_ctx, 0, ps->filename, 0);
    //至少有一种流，读流，解码。
    
    SDL_CreateThread(decode_thread, "decode_thread", ps);
    
    if (ps->audio_stream_index != -1)
    {
        //         packet_queue_init(&ps->audio_packet_queue);
        prepare_audio(ps);
        play_audio(ps);
    }
    
    if ( ps->video_stream_index != -1)
    {
        //        packet_queue_init(&ps->video_packet_queue);
        prepare_video(ps);
        play_video(ps);
    }
    
    double     increase;
    while(1)
    {
        if (ps->player_state == -1)
        {
            break;
        }
        
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case ISSHE_REFRESH_EVENT:     //自定义的事件
            {
                decode_and_show(ps);
                break;
            }
            case SDL_WINDOWEVENT:         //窗口事件
            {   SDL_GetWindowSize(ps->pwindow, &ps->window_w, &ps->window_h);
                break;
            }
            case SDL_KEYDOWN:         //按键事件
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_SPACE:         //空格
                    {
                        if (ps->player_state == 1)
                        {
                            ps->player_state = 0;
                            SDL_PauseAudio(0);
                            
                        }
                        else if (ps->player_state == 0)
                        {
                            ps->player_state = 1;
                            SDL_PauseAudio(1);
                        }
                        break;
                    }
                    case SDLK_LEFT:     //左
                    {
                        increase = -10.0;
                        //                        ps->player_state = 2;
                        do_seek(ps, increase);
                        break;
                    }
                    case SDLK_RIGHT:     //右
                    {
                        increase = 10.0;
                        //                        ps->player_state = 3;
                        do_seek(ps, increase);
                        break;
                    }
                    case SDLK_1:{
                        ps->change_fiter = 1;
                        ps->filte_key = 1;
                        break;
                    }
                    case SDLK_2:{
                        ps->change_fiter = 1;
                        ps->filte_key = 2;
                        break;
                    }
                    case SDLK_3:{
                        //                        ps->change_fiter = 1;
                        //                        ps->filte_key = 3;
                        break;
                    }
                    case SDLK_4:{
                        ps->change_fiter = 1;
                        ps->filte_key = 4;
                        break;
                    }
                    case SDLK_5:{
                        ps->change_fiter = 1;
                        ps->filte_key = 5;
                        break;
                    }
                    case SDLK_6:{
                        ps->change_fiter = 1;
                        ps->filte_key = 6;
                        break;
                    }
                    case SDLK_7:{
                        ps->change_fiter = 1;
                        ps->filte_key = 7;
                        break;
                    }
                        
                        
                        
                        
                }
                break;     //不要忘了
            }
            case SDL_QUIT:             //退出
            {
                printf("SDL_QUIT！\n");
                ps->player_state = -1;
                SDL_CloseAudio();
                SDL_Quit();
                break;
            }
            default:
            {
                break;
            }
        }
        
    }
    
    //一些释放内存/清理工作。
    //如果太多就放到一个函数里。
    /*
     if (ps->video_stream_index != -1)
     {
     sws_freeContext(ps->psws_ctx);
     }
     */
    avformat_close_input(&ps->pformat_ctx);
    
    return 0;
}

int prepare_common(ZYPlayerContext *ps)
{
    
    av_register_all();
    // fliter
    avfilter_register_all();
    av_log_set_level(AV_LOG_VERBOSE);
    
    
    if (SDL_Init(SDL_INIT_VIDEO |SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        fprintf(ERR_STREAM, "init SDL error: %s\n", SDL_GetError());
        return -1;
    }
    
    //打开文件
    //pformat_ctx会有所指向，不用分配内存
    if (avformat_open_input(&ps->pformat_ctx, ps->filename, NULL, NULL) != 0)
    {
        fprintf(ERR_STREAM, "open input file error\n");
        return -1;
    }
    
    if (avformat_find_stream_info(ps->pformat_ctx, NULL) < 0)
    {
        fprintf(ERR_STREAM, "Couldn't find stream info\n");
        return -1;
    }
    
    ps->audio_stream_index = -1;
    ps->audio_stream_index = -1;
//    if (find_stream_index(ps->pformat_ctx,
//                          &ps->video_stream_index,
//                          &ps->audio_stream_index) == -2)
//    {
//        fprintf(ERR_STREAM, "Couldn't find any stream index\n");
//        return -1;
//    }
    
    
    for (int i = 0; i < ps->pformat_ctx->nb_streams; i++) {
        if (ps->pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            ps->video_stream_index = i;
        }
        if (ps->pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            ps->audio_stream_index = i;
        }
    }
    return 0;
}



int decode_thread(void *arg)
{
    ZYPlayerContext *ps = (ZYPlayerContext *)arg;
    AVPacket    *packet = av_packet_alloc();
    int         flag = 0;
    
    //初始化队列
    if (ps->audio_stream_index != -1)
    {
        packet_queue_init(&ps->audio_packet_queue);
    }
    
    if ( ps->video_stream_index != -1)
    {
        packet_queue_init(&ps->video_packet_queue);
    }
    
    while(1)
    {
        //如果队列数据过多，就等待以下
        if (ps->seek_req == 1)
        {
            seeking(ps);
        }
        
        if (ps->audio_packet_queue.nb_packets >=MAX_AUDIO_QUEUE_SIZE ||
            ps->video_packet_queue.nb_packets >= MAX_VIDEO_QUEUE_SIZE)
        {
            printf("过多数据，延时\n");
            SDL_Delay(100);
            continue;
        }
        
        //小于0代表读完或者出错，如果连续多次都<0，则认为没数据了
        if (av_read_frame(ps->pformat_ctx, packet) < 0)
        {
            if (flag < READ_FRAME_ERROR_TIMES)
            {
                flag++;
                continue;
            }
            else
            {
                printf("退出读pakcet线程!\n");
                break; //退出读packet线程
            }
        }
        
        flag = 0;
        //读取到数据了
        if (packet->stream_index == ps->video_stream_index)
        {
            packet_queue_put(&ps->video_packet_queue, packet);
        }
        
        if (packet->stream_index == ps->audio_stream_index)
        {
            packet_queue_put(&ps->audio_packet_queue, packet);
        }
        
    }
    
    av_packet_free(&packet);
    return 0;
}

void player_state_init(ZYPlayerContext *ps){
    ps->pformat_ctx             = NULL;
    ps->quit                     = 0;
    ps->player_state             = 0;
    
    ps->audio_stream_index         = -1;
    ps->paudio_stream             = NULL;
    
    ps->paudio_codec_ctx         = NULL;
    ps->paudio_codec             = NULL;
    ps->audio_buf_size            = 0;
    ps->audio_buf_index         = 0;
    ps->audio_quit                 = 0;
    //视频
    ps->video_stream_index         = -1;
    ps->pvideo_stream             = NULL;
    ps->pvideo_codec_ctx         = NULL;
    ps->pvideo_codec             = NULL;
    ps->video_buf                = NULL;
    ps->video_buf_size             = 0;
    ps->video_buf_index         = 0;
    ps->psws_ctx                 = NULL;
    ps->video_quit                 = 0;
    ps->zero_packet_count         = 0;
    
    ps->change_fiter = 1;
    ps->filte_key = 0;
    
    ps->pixel_w                 = 0;
    ps->pixel_h                 = 0;
    ps->window_w                 = 0;
    ps->window_h                 = 0;
    
    ps->pwindow                 = NULL;
    ps->prenderer                 = NULL;
    ps->ptexture                 = NULL;
    ps->pixfmt                     = AV_PIX_FMT_YUV420P;
    
    ps->audio_clock                 = 0.0;
    ps->video_clock                 = 0.0;
    ps->pre_frame_pts                = 0.0;        //前一帧显示时间
    //   ps->cur_frame_pkt_pts            = 0.0;         //当前帧在packet中标记的pts
    ps->pre_cur_frame_delay         = 40e-3;     //当前帧和前一帧的延时，前面两个相减的结果
    ps->cur_frame_pts                 = 0.0;        //packet.pts
    ps->delay                         = 40;

}

