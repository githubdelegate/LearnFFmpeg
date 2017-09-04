//
//  zy_filter.h
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#ifndef zy_filter_h
#define zy_filter_h

#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>

typedef enum{
    FILTER_NULL = 48,
    FILTER_MIRROR,
    FILTER_WATERMATK,
    FILTER_NEGATE,
    FILTER_EDGE,
    FILTER_SPLIT4,
    FILTER_VINTAGE
}FILTERS;


int apply_filters(AVFormatContext *ifmt_ctx,FILTERS type);
void initFilter();
//int init_filters(const char *filters_descr);

AVFilterContext* buffersink_ctx_ins();

AVFilterContext* buffersrc_ctx_ins();

//const char* filter_desc_ins();

void setCodecCtx(AVCodecContext *codec_ctx);

#endif /* zy_filter_h */
