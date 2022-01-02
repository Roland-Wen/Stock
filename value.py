import time
import random
import datetime
import requests
from bs4 import BeautifulSoup
import pandas as pd


f=open('TSE.tls','r');
total=821
cnt=1
success=0
for x in f:
    ticker = x
    
    ticker = ticker[:4]
    print("TRY:"+ticker)
    print(str(cnt)+' / '+str(total))
    cnt+=1
    url = f'https://goodinfo.tw/StockInfo/StockBzPerformance.asp?STOCK_ID={ticker}'
    headers = {
        "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }
    print("url:"+url)
    try:
        res = requests.get(url,headers = headers)
        res.encoding = "utf-8"
        soup = BeautifulSoup(res.text,"lxml")
        data = soup.select_one("#txtFinDetailData")
        dfs = pd.read_html(data.prettify())
        df = dfs[0]
        df.to_csv(ticker+".csv")
        success+=1
        print("OK")
    except:
        url=url
        print(ticker+' does not exist')
    time.sleep(random.randint(15,20))
    
print(str(success)+' / '+str(total)+' success!');