import time
import datetime
import pandas as pd

period1 = int(time.mktime(datetime.datetime(2020, 6, 18, 23, 59).timetuple()))
period2 = int(time.mktime(datetime.datetime(2021, 8, 10, 15, 59).timetuple()))
f=open('TSE.tls','r');
total=821
cnt=1
success=0
for x in f:
    ticker = x
    ticker = ticker[:-1]
    print(ticker)
    print(str(cnt)+' / '+str(total))
    interval = '1d' # 1d, 1m
    cnt+=1

    query_string = f'https://query1.finance.yahoo.com/v7/finance/download/{ticker}?period1={period1}&period2={period2}&interval={interval}&events=history&includeAdjustedClose=true'
    
    try:
        df = pd.read_csv(query_string)
        df.to_csv(ticker + '.csv')
        success+=1
        print("ok")
    except:
        interval=interval
        print('Does not exist')
    time.sleep(1)

print(str(success)+' / '+str(total)+' success!');
f.close();