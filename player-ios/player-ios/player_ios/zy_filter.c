//
//  zy_filter.c
//  player-ios
//
//  Created by zhangyun on 04/09/2017.
//  Copyright © 2017 zhangyun. All rights reserved.
//

#include "zy_filter.h"

//Enable SDL?
#define ENABLE_SDL 1
//Output YUV data?
#define ENABLE_YUVFILE 1


const char *filter_descr = "movie=/Users/zhangyun/Documents/LearnFFmpeg/play/play/my_logo.png[wm];[in][wm]overlay=5:5[out]";
const char *filter_descr_null = "null";
const char *filter_mirror = "crop=iw/2:ih:0:0,split[left][tmp];[tmp]hflip[right]; \
[left]pad=iw*2[a];[a][right]overlay=w";
const char *filter_watermark = "movie=/Users/zhangyun/Documents/LearnFFmpeg/play/play/my_logo.png[wm];[in][wm]overlay=5:5[out]";
const char *filter_negate = "negate[out]";
const char *filter_edge = "edgedetect[out]";
const char *filter_split4 = "scale=iw/2:ih/2[in_tmp];[in_tmp]split=4[in_1][in_2][in_3][in_4];[in_1]pad=iw*2:ih*2[a];[a][in_2]overlay=w[b];[b][in_3]overlay=0:h[d];[d][in_4]overlay=w:h[out]";
const char *filter_vintage = "curves=vintage";

static AVFormatContext *pFormatCtx;
static AVCodecContext *pCodecCtx;
AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
AVFilter *buffersrc;
AVFilter *buffersink;

static int video_stream_index = -1;

AVFilterContext* buffersink_ctx_ins(){
    return buffersink_ctx;
}

AVFilterContext* buffersrc_ctx_ins(){
    return buffersrc_ctx;
}


const char* filter_desc_ins(){
    return filter_descr;
}


void setCodecCtx(AVCodecContext *codec_ctx){
    pCodecCtx = codec_ctx;
}


void initFilter(){
    //Register Filter
    avfilter_register_all();
    buffersrc = avfilter_get_by_name("buffer");
    buffersink = avfilter_get_by_name("buffersink");
}

int apply_filters(AVFormatContext *ifmt_ctx,FILTERS type)
{
    char args[512];
    int ret;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    if (!outputs)
    {
        printf("Cannot alloc output\n");
        return -1;
    }
    AVFilterInOut *inputs = avfilter_inout_alloc();
    if (!inputs)
    {
        printf("Cannot alloc input\n");
        return -1;
    }
    
    if (filter_graph)
        avfilter_graph_free(&filter_graph);
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph){
        printf("Cannot create filter graph\n");
        return -1;
    }
    
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             ifmt_ctx->streams[0]->codec->width, ifmt_ctx->streams[0]->codec->height, ifmt_ctx->streams[0]->codec->pix_fmt,
             ifmt_ctx->streams[0]->time_base.num, ifmt_ctx->streams[0]->time_base.den,
             ifmt_ctx->streams[0]->codec->sample_aspect_ratio.num, ifmt_ctx->streams[0]->codec->sample_aspect_ratio.den);
    
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        printf("Cannot create buffer source\n");
        return ret;
    }
    
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        printf("Cannot create buffer sink\n");
        return ret;
    }
    
    /* Endpoints for the filter graph. */
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;
    
    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;
    
    
    switch (type) {
        case FILTER_NULL:
            filter_descr = filter_descr_null;
            break;
        case FILTER_MIRROR:
            filter_descr = filter_mirror;
            break;
        case FILTER_VINTAGE:
            filter_descr = filter_vintage;
            break;
        case FILTER_SPLIT4:
            filter_descr = filter_split4;
            break;
        case FILTER_EDGE:
            filter_descr = filter_edge;
            break;
        case FILTER_WATERMATK:
            filter_descr = filter_watermark;
            break;
        case FILTER_NEGATE:
            filter_descr = filter_negate;
            break;
        default:
            break;
    }
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                        &inputs, &outputs, NULL)) < 0)
        return ret;
    
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        return ret;
    
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    
    return 0;
}

int init_filters(const char *filters_descr)
{
    /*
     /**
     * A linked-list of the inputs/outputs of the filter chain.
     *
     * This is mainly useful for avfilter_graph_parse() / avfilter_graph_parse2(),
     * where it is used to communicate open (unlinked) inputs and outputs from and
     * to the caller.
     * This struct specifies, per each not connected pad contained in the graph, the
     * filter context and the pad index required for establishing a link.
     */
    //    typedef struct AVFilterInOut {
    /** unique name for this input/output in the list */
    //      char *name;
    
    /** filter context associated to this input/output */
    //    AVFilterContext *filter_ctx;
    
    /** index of the filt_ctx pad to use for linking */
    //  int pad_idx;
    
    /** next input/input in the list, NULL if this is the last */
    //struct AVFilterInOut *next;
    //} AVFilterInOut;
    
    char args[512];
    int ret;
    // Get a filter definition matching the given name.
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    //Allocate a single AVFilterInOut entry. Must be freed with avfilter_inout_free().
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;
    //Allocate a filter graph.
    filter_graph = avfilter_graph_alloc();
    
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
             pCodecCtx->time_base.num, pCodecCtx->time_base.den,
             pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);
    /*
     Create and add a filter instance into an existing graph. The filter instance is created from the filter filt and inited with the parameters args and opaque.
     In case of success put in *filt_ctx the pointer to the created filter instance, otherwise set *filt_ctx to NULL.
     */
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        printf("Cannot create buffer source\n");
        return ret;
    }
    
    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        printf("Cannot create buffer sink\n");
        return ret;
    }
    
    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
    
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0){
        printf("avfilter graph parse failed\n");
        return ret;
    }
    
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0){
        printf(" avfilter graph config failed \n");
        return ret;
    }
    return 0;
}
