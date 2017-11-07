#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# pip install baidu-aip
import sys
# print(sys.path)
sys.path.append('/usr/local/lib/python2.7/site-packages')

from aip import AipSpeech
import json


AppID = '10324542'
APIKey = 'IL3tOhEjuAOIV0dzpCVgQDnR'
SecretKey = '8vQCSNmbQV8Oo8i5SdIzlIyTdA0r5dvd'

host = 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s' %(APIKey, SecretKey)

aipSpeech = AipSpeech(AppID, APIKey, SecretKey)


def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()


def get_asr_result(filePath):
    # read file
    result = aipSpeech.asr(get_file_content(filePath), 'wav', 16000, {
        'lan': 'zh',
    })
    json_result = json.dumps(result)
    strtestObj = json.loads(json_result)
    lists = strtestObj["result"]
    res = lists[0]  #.encode('UTF-8')
    #print res, len(res)

    if res.find(u'猫') != -1:
        print("hello")
    elif res.find(u"沙发") != -1:
        print("hello")
    else:
        pass
    print "RESULT: ".decode('utf-8').encode('gbk'), lists[0]


if __name__ == "__main__":
    asrfile = './1.wav'
    get_asr_result(asrfile)
