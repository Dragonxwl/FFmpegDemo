package com.xwl.ffmpegdemo

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        btn_protocol.setOnClickListener {
            tv_info!!.text = urlprotocolinfo()
        }
        btn_codec.setOnClickListener {
            tv_info!!.text = avcodecinfo()
        }
        btn_filter.setOnClickListener {
            tv_info!!.text = avfilterinfo()
        }
        btn_format.setOnClickListener {
            tv_info!!.text = avformatinfo()
        }

    }

    external fun stringFromJNI(): String?
    private external fun urlprotocolinfo(): String?
    private external fun avformatinfo(): String?
    private external fun avcodecinfo(): String?
    private external fun avfilterinfo(): String?

}