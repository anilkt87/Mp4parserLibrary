package com.samoon.sammp4;

import com.samoon.sammp4.bean.AssistInfo;

import java.security.PublicKey;
import java.util.List;

public
class MP4Help {
    static {
        System.loadLibrary("native-lib");
    }

    public static native List<AssistInfo> checkPlatform(String path);
    public static native List<AssistInfo> parseMp4File(String path);

    //public static native int testMp42H264(String path,String outFile);
}
