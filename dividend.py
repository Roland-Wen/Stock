import time
import datetime
import pandas as pd

period1 = int(time.mktime(datetime.datetime(2011, 6, 18, 23, 59).timetuple()))
period2 = int(time.mktime(datetime.datetime(2021, 7, 5, 23, 59).timetuple()))
f=open('TSE.tls','r');
total=821
cnt=1
success=0
for x in f:
    ticker = x
    ticker = ticker[:-1]
    print("TRY:"+ticker)
    print(str(cnt)+' / '+str(total))
    interval = '1m' # 1d, 1m
    cnt+=1

    query_string = f'https://query1.finance.yahoo.com/v7/finance/download/{ticker}?period1=946944000&period2=1625356800&interval=1d&events=div&includeAdjustedClose=true'
    
    try:
        df = pd.read_csv(query_string)
        df.to_csv(ticker + '.csv')
        success+=1
        print("ok")
    except:
        interval=interval
        print(ticker+' does not exist')
    time.sleep(1)

print(str(success)+' / '+str(total)+' success!');