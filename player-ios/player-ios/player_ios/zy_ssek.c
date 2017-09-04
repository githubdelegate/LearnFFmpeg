//
//  zy_ssek.c
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "zy_ssek.h"


void do_seek(ZYPlayerContext *ps, double increase)
{
    double     pos = 0.0;
    pos = get_audio_clock(ps);     //这里以什么为基准同步，就用哪个clock。
    pos += increase;
    if (ps->seek_req == 0)
    {
        ps->seek_req = 1;
        ps->seek_pos = (int64_t)(pos * AV_TIME_BASE);         //seek_pos用double会不会更好点？
        //AVSEEK_FLAG_BACKWARD，ffmpeg定义为1
        ps->seek_flags = increase > 0 ? 0 : AVSEEK_FLAG_BACKWARD;
        
        
    }
}


/*======================================================================\
 * Author     (作者): i.sshe
 * Date       (日期): 2016/10/10
 * Others     (其他): 查找相应位置
 \*=======================================================================*/
void seeking(ZYPlayerContext *ps)
{
    int     stream_index = -1;
    int64_t seek_target = ps->seek_pos;
    
    if (ps->video_stream_index >= 0)
    {
        stream_index = ps->video_stream_index;
    }
    else if (ps->audio_stream_index >= 0)
    {
        stream_index = ps->audio_stream_index;
    }
    
    if (stream_index >= 0)
    {
        //AV_TIME_BASE_Q是AV_TIME_BASE的倒数，用AVRational结构存储
        seek_target = av_rescale_q(seek_target, AV_TIME_BASE_Q,
                                   ps->pformat_ctx->streams[stream_index]->time_base);
    }
    // Seek to the keyframe at timestamp. 'timestamp' in 'stream_index'.
    if (av_seek_frame(ps->pformat_ctx, stream_index,
                      seek_target, ps->seek_flags) < 0)
    {
        fprintf(ERR_STREAM, "error while seeking\n");
    }
    else
    {
        if (ps->video_stream_index >= 0)
        {
            packet_queue_flush(&ps->video_packet_queue);
            //
        }
        
        if (ps->audio_stream_index >= 0)
        {
            packet_queue_flush(&ps->audio_packet_queue);
        }
    }
    
    ps->seek_req = 0;
}
