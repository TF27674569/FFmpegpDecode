package com.sample.decode;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import com.sample.decode.ffmpegp.VideoUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    public void decode(View view) {
        File file = new File(Environment.getExternalStorageDirectory().getPath(),"input.mp4");
        Log.e("TAG", file.getAbsolutePath() + "   exists = " + file.exists() );

        VideoUtils.decodeYUV420P(file.getAbsolutePath(),new File(Environment.getExternalStorageDirectory().getPath(),"outPut.yuv").getAbsolutePath());
    }
}
