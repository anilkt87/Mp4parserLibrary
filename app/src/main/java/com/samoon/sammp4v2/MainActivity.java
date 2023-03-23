package com.samoon.sammp4v2;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.samoon.sammp4.MP4Help;
import com.samoon.sammp4.bean.AssistInfo;

import java.io.File;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = MainActivity.class.getSimpleName();

    private List<AssistInfo> assistInfos;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String path = "/storage/emulated/0/Movies/RoveDashCam/Rove-R24K-PRO/Videos/2023_0321_222343_0133.MP4";
        if(new File(path).exists()){
            assistInfos = MP4Help.checkPlatform(path);
            for (int i = 0; i <assistInfos.size() ; i++) {
                AssistInfo assistInfo = assistInfos.get(i);
                Log.d(TAG, assistInfo.getGps_lat()+","+assistInfo.getGps_lgt()+","+assistInfo.getSpd());
            }
        }
    }
}