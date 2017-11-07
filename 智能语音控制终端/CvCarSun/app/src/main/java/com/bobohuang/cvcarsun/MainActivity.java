package com.bobohuang.cvcarsun;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.unisound.client.SpeechConstants;
import com.unisound.client.SpeechUnderstander;
import com.unisound.client.SpeechUnderstanderListener;

import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;
import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, View.OnTouchListener {

    /**
     * 当前识别状态
     */
    enum AsrStatus {
        idle, recording, recognizing
    }

    private TextView statusTextView;
    private TextView statusShowTextView;
    private EditText resultEditText;
    private ProgressBar volumeProgressBar;
    private Button recognizerBtn;

    List<NameValuePair> params;

    private AsrStatus status = AsrStatus.idle;
    private SpeechUnderstander mUnderstander;

    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    @SuppressLint("NewApi")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }

        initData();
        initRecognizer();
    }

    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg){
            super.handleMessage(msg);
            Bundle data = msg.getData();
            String val = data.getString("value");
        }
    };

    Runnable runnable = new Runnable(){
        @Override
        public void run() {
            //
            // TODO: http request.
            //
            Message msg = new Message();
            Bundle data = new Bundle();
            data.putString("value","请求结果");
            msg.setData(data);
            handler.sendMessage(msg);
        }
    };

    private void initData() {
        this.statusTextView = (TextView) this.findViewById(R.id.status_textview);
        this.statusShowTextView = (TextView) this.findViewById(R.id.status_show_textview);
        this.resultEditText = (EditText) this.findViewById(R.id.result_textview);
        this.volumeProgressBar = (ProgressBar) this.findViewById(R.id.volume_progressbar);
        this.recognizerBtn = (Button) this.findViewById(R.id.recognizer_btn);

        this.recognizerBtn.setOnClickListener(this);

    }

    private void initRecognizer() {
        // 创建识别对象，appKey通过 http://dev.hivoice.cn/ 网站申请
        mUnderstander = new SpeechUnderstander(this, Config.appKey, Config.secret);
        // 设置参数: 在线识别
        mUnderstander.setOption(SpeechConstants.ASR_SERVICE_MODE, SpeechConstants.ASR_SERVICE_MODE_NET);

        mUnderstander.setListener(new SpeechUnderstanderListener() {
            @Override
            public void onEvent(int type, int timeMs) {
                switch (type) {
                    case SpeechConstants.ASR_EVENT_VAD_TIMEOUT:
                        // 收到用户停止说话事件，停止录音
                        stopRecord();
                        break;
                    case SpeechConstants.ASR_EVENT_VOLUMECHANGE:
                        // 说话音量实时返回
                        int volume = (Integer) mUnderstander.getOption(SpeechConstants.GENERAL_UPDATE_VOLUME);
                        volumeProgressBar.setProgress(volume);
                        break;
                    case SpeechConstants.ASR_EVENT_SPEECH_DETECTED:
                        //用户开始说话
                        statusShowTextView.setText(R.string.speaking);
                        break;
                    case SpeechConstants.ASR_EVENT_RECORDING_START:
                        //录音设备打开，开始识别，用户可以开始说话
                        statusShowTextView.setText(R.string.please_speak);
                        recognizerBtn.setEnabled(true);
                        status = AsrStatus.recording;
                        recognizerBtn.setText(R.string.say_over);
                        break;
                    case SpeechConstants.ASR_EVENT_RECORDING_STOP:
                        // 停止录音，请等待识别结果回调
                        log_v("onRecordingStop");
                        status = AsrStatus.recognizing;
                        recognizerBtn.setText(R.string.give_up);
                        statusShowTextView.setText(R.string.just_recognizer);
                        break;
                    case SpeechConstants.ASR_EVENT_USERDATA_UPLOADED:
                        log_v("onUploadUserData");
                        Toast.makeText(MainActivity.this,
                                R.string.info_upload_success, Toast.LENGTH_LONG)
                                .show();
                        break;
                    case SpeechConstants.ASR_EVENT_NET_END:
                        recognizerBtn.setText(R.string.click_say);
                        status = AsrStatus.idle;
                        break;
                }
            }

            @Override
            public void onError(int type, final String errorMSG) {
                if (errorMSG != null) {
                    // 显示错误信息
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            resultEditText.setText(errorMSG);
                        }

                    });
                } else {
                    if ("".equals(resultEditText.getText().toString())) {
                        resultEditText.setText(R.string.no_hear_sound);
                    }

                }
            }

            @Override
            public void onResult(int type, String jsonResult) {
                switch (type) {
                    case SpeechConstants.ASR_RESULT_NET:
                        if (jsonResult.contains("net_asr")) {
                            try {
                                JSONObject json = new JSONObject(jsonResult);
                                JSONArray jsonArray = json.getJSONArray("net_asr");
                                JSONObject jsonObject = jsonArray.getJSONObject(0);
                                String status = jsonObject.getString("result_type");
                                if (status.equals("partial")) {
                                    String result = (String) jsonObject.get("recognition_result");
                                    resultEditText.append((String) jsonObject.get("recognition_result"));
                                    //发送指令
                                    String cmd = judgeVoiceText(result);
                                    if (cmd != null && !cmd.contentEquals("")) {
                                        Map<String, String> rsMap = new HashMap<String, String>();
                                        rsMap.put("result", cmd);
                                        params = new ArrayList<NameValuePair>();
                                        params.add(new BasicNameValuePair("result", cmd));
                                        Thread t = new Thread(){
                                            @Override
                                            public void run() {
                                                HttpHelper.doPost(Config.server, params);
                                            }
                                        };
                                        t.start();

                                    }
                                }

                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }

                        break;
                    default:
                        break;
                }
            }
        });

        // 识别引擎初始化
        mUnderstander.init("");

    }

    public String judgeVoiceText(String voiceText) {

        if (voiceText.contains("前")) {
            return "up";
        } else if (voiceText.contains("后")) {
            return "down";
        } else if (voiceText.contains("左")) {
            return "left";
        } else if (voiceText.contains("右")) {
            return "right";
        } else if (voiceText.contains("停")) {
            return "stop";
        }
        return null;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.recognizer_btn:
                if (status == AsrStatus.idle) {
                    resultEditText.setVisibility(View.VISIBLE);
                    recognizerBtn.setEnabled(false);
                    resultEditText.setText("");
                    statusTextView.setVisibility(View.VISIBLE);
                    statusShowTextView.setText(R.string.opening_recode_devices);

                    mUnderstander.setOption(SpeechConstants.ASR_DOMAIN, "general");
                    mUnderstander.start();
                } else if (status == AsrStatus.recording) {

                } else if (status == AsrStatus.recognizing) {
                    mUnderstander.cancel();

                    recognizerBtn.setText(R.string.click_say);
                    status = AsrStatus.idle;
                }
                break;
            default:
                break;
        }
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
//        if(v.getId() == R.id.button1){
        if (event.getAction() == MotionEvent.ACTION_UP) {
            Log.d("test", "cansal button ---> cancel");
//                mButton.setBackgroundResource(R.drawable.green);
        }
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            Log.d("test", "cansal button ---> down");
//                mButton.setBackgroundResource(R.drawable.yellow);
        }
//        }
        return false;
    }

    /**
     * 打印日志信息
     *
     * @param msg
     */
    public void log_v(String msg) {
        Log.v("demo", msg);
    }

    /**
     * 停止录音
     */
    public void stopRecord() {
        statusShowTextView.setText(R.string.just_recognizer);
        mUnderstander.stop();
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (mUnderstander != null) {
            mUnderstander.stop();
        }
    }

}
