package com.xwl.ffmpegdemo

import android.os.Bundle
import android.os.Environment
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File


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

        btn_create_path.setOnClickListener {
            Environment.getDataDirectory().parentFile
        }
        btn_set_callback.setOnClickListener {
            setCallback(object : PushCallback {
                override fun videoCallback(pts: Long, dts: Long, duration: Long, index: Long) {
                    val sb = StringBuilder()
                    sb.append("pts: ").append(pts).append("\n")
                    sb.append("dts: ").append(dts).append("\n")
                    sb.append("duration: ").append(duration).append("\n")
                    sb.append("index: ").append(index).append("\n")
                    tv_info!!.text = sb.toString()
                }

            })
        }
        btn_get_file.setOnClickListener {
            path =
                Environment.getExternalStorageDirectory().absolutePath + File.separator.toString() + "cs.mp4"
            val file = File(path)
            tv_info!!.text = "path ${file.exists()}"
        }
        btn_push.setOnClickListener {
            object : Thread() {
                override fun run() {
                    super.run()
                    tv_info!!.text = "result ${pushRtspFile(path)}"
                }
            }.start()
        }
    }

    var path:String = ""

    external fun stringFromJNI(): String?
    private external fun urlprotocolinfo(): String?
    private external fun avformatinfo(): String?
    private external fun avcodecinfo(): String?
    private external fun avfilterinfo(): String?
    private external fun pushRtspFile(path:String): Int
    private external fun setCallback(pushCallback:PushCallback): Int
}