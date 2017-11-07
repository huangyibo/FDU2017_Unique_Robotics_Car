package com.bobohuang.cvcarsun;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.params.CoreConnectionPNames;
import org.apache.http.params.HttpParams;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;


/**
 * Created by bob on 2017/11/5.
 */

public class HttpHelper {

    public static boolean httpPostWithJson(JSONObject jsonObject, String url){
        boolean isSuccess = false;

        HttpPost post = null;
        try{
            HttpClient httpClient = new DefaultHttpClient();
            // 设置超时时间
            httpClient.getParams().setParameter(CoreConnectionPNames.CONNECTION_TIMEOUT, 2000);
            httpClient.getParams().setParameter(CoreConnectionPNames.SO_TIMEOUT, 2000);

            post = new HttpPost(url);
            // 构造消息头
            post.setHeader("Content-type", "application/json; charset=utf-8");

            // 构建消息实体
            StringEntity entity = new StringEntity(jsonObject.toString(), "UTF-8");
            entity.setContentEncoding("UTF-8");
            // 发送Json格式的数据请求
            entity.setContentType("application/json");
            post.setEntity(entity);

            HttpResponse response = httpClient.execute(post);

        }catch (Exception e) {
            e.printStackTrace();
            isSuccess = false;
        }finally{
            if(post != null){
                try {
                    post.abort();
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        return isSuccess;
    }

    public static boolean doPost(String url, List<NameValuePair> params){
        HttpClient httpClient = null;
        HttpPost httpPost = null;
        boolean result = false;
        try{
            httpClient = new DefaultHttpClient();
            httpPost = new HttpPost(url);
            //设置参数
            httpPost.setEntity(new UrlEncodedFormEntity(params, "UTF-8"));
            HttpResponse response = httpClient.execute(httpPost);
            if(response.getStatusLine().getStatusCode() == 200){
                result = true;
            }
        }catch(Exception ex){
            ex.printStackTrace();
        }
        return result;
    }

    public static boolean httpPost(String url, List<NameValuePair> params){
        HttpClient httpClient = null;
//        HttpPost httpPost = null;
        HttpPost httpPost = null;
        boolean result = false;
        try{
            httpClient = new DefaultHttpClient();
            httpPost = new HttpPost(url);
            //设置参数
            httpPost.setEntity(new UrlEncodedFormEntity(params));
            HttpResponse response = httpClient.execute(httpPost);
            if(response.getStatusLine().getStatusCode() == 200){
                result = true;
            }
        }catch(Exception ex){
            ex.printStackTrace();
        }
        return result;
    }

}
