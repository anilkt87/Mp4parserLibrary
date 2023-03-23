//
// Created by administrator on 2021/7/15.
//

#ifndef YEECORECAMVIEWER_ANDROID_FILELIST_H
#define YEECORECAMVIEWER_ANDROID_FILELIST_H

typedef enum {

    UNKNOW_TYPE = 0,
    DEV_A12,
    DEV_A7L,
    DEV_SUNPLUNS,
    //DEV_NVT,
    DEV_NVT_65X,
    DEV_NVT_66X,
    DEV_HISILICON,
    DEV_NT9667XA,
}enDevType;


typedef struct tagFileNode {

    char*	filepath;
    char* filename;
    char* assist_filepath;
    char* file_dt;

    int		duration;
    int     rd_only;

    int     dev_type;

}FileNode_t, *PFileNode_t;

#endif //YEECORECAMVIEWER_ANDROID_FILELIST_H
