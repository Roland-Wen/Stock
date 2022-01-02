#include<bits/stdc++.h>
using namespace std;

#define F(i,L,R) for(int i=L;i<R;i++)
#define FE(i,L,R) for(int i=L;i<=R;i++)

double open[1005],high[1005],low[1005],close[1005],volume[1005];
string date[1005];
int buy[1005],sell[1005];
double k[1005],d[1005],rsv[1005],ema12[1005],ema26[1005],macd9[1005];
const double initMoney=170000.0,buyTax=0.001425,sellTax=0.004425;
const int infoLevel=0;

struct trade{
	int buyDay,share;
	double buyPrice;
};

struct info{
	int amt;
	string ticker;
	double profit;
};
vector<info> infos;
bool cmp(info a,info b){
	if(a.profit>b.profit) return 1;
	if(a.amt>b.amt) return 1;
	return 1;
}

int input(string ticker){
	FILE *fp;
	ticker="data\\"+ticker+".csv";
	char filename[50];
	F(i,0,ticker.size()) filename[i]=ticker[i];
	filename[ticker.size()]='\0';
    if((fp=fopen(filename,"r"))==NULL){
    	if(infoLevel>=1) cout<<filename<<" not found\n";
    	return 0;
    }
    char ch;
	string input="";
    while((ch=getc(fp))!=EOF){
		if(ch=='\n') ch=',';
		input+=ch;
	}
	stringstream ss(input);
	string token;
	//header
	F(i,0,8) getline(ss,token,',');
	int index=0;
	while(getline(ss,token,',')){
		//date
		getline(ss,token,',');
		date[index]=token;
		
		//open
		getline(ss,token,',');
		if(token=="") open[index]=-1;
		else open[index]=stod(token);
		//high
		getline(ss,token,',');
		if(token=="") high[index]=-1;
		else high[index]=stod(token);
		//low
		getline(ss,token,',');
		if(token=="") low[index]=-1;
		else low[index]=stod(token);
		//close
		getline(ss,token,',');
		if(token=="") close[index]=-1;
		else close[index]=stod(token);
		//adj close
		getline(ss,token,',');
		//volume
		getline(ss,token,',');
		if(token=="") volume[index]=-1;
		else volume[index]=stod(token);
		index++;
	}
	//F(i,0,index) printf("%lf %lf %lf %lf %lf\n",open[i],high[i],low[i],close[i],volume[i]);
	fclose(fp);
	return index;
}

void clean(int day){
	F(i,0,day) buy[i]=sell[i]=0;
}

//return profit percent
double sim(int day,string ticker){
	double bank=initMoney;
	int totalShare=0;
	bool giveProfit=0;
	info now;
	F(i,0,day){
		//inform trader to sale or buy a certain quote today
		if(i==day-1){
			if(sell[i]>0){
				now.ticker=ticker;
				now.amt=-sell[i];
				giveProfit=1;
				if(infoLevel>=2) printf("SELL %d TODAY\n",sell[i]);
			}
			if(buy[i]>0){
				now.ticker=ticker;
				now.amt=buy[i];
				giveProfit=1;
				if(infoLevel>=2) printf("BUY %d TODAY\n",buy[i]);
			}
		}
		
		//last day, sell all
		if(i==day-1&&totalShare>0) sell[i]=totalShare;
		
		//buy and sale stocks with the closing price
		bank+=sell[i]*close[i];
		bank-=buy[i]*close[i];
		totalShare+=(buy[i]-sell[i]);
		
		//totalShare shouldn't be negative at any given time
		if(totalShare<0){
			if(infoLevel>=1) cout<<"IMPOSSIBLE TRADE on "<<date[i]<<" !!!\n";
			return -10000.0;
		}
		
		//tax
		bank-=buy[i]*close[i]*buyTax;
		bank-=sell[i]*close[i]*sellTax;
		
		//handle bankrupt
		if(bank<=0){
			if(infoLevel>=1) cout<<"BANKRUPT on "<<date[i]<<" !!!\n";
			return -10000.0;
		}
		
		//printing out info
		cout<<fixed;
		cout<<setprecision(2);
		if(buy[i]>0&&infoLevel>=4)  cout<<date[i]<<": Buy  "<<buy[i]<<" with "<<close[i]<<" Bank "<<bank<<"\n";
		if(sell[i]>0&&infoLevel>=4) cout<<date[i]<<": Sell "<<sell[i]<<" with "<<close[i]<<" Bank "<<bank<<"\n";
	}
	if(infoLevel>=3) printf("Init  : %lf\n",initMoney);
	if(infoLevel>=3) printf("Final : %lf\n",bank);
	
	//profit percent
	double profit=(bank-initMoney)/initMoney;
	profit*=100;
	now.profit=profit;
	if(infoLevel>=3) printf("Profit: %lf%%\n",profit);
	
	
	if(giveProfit) infos.push_back(now);
	return profit;
}

void build(int day){
	//share I have now, KD-period, ?
	int totalShare=0,period=9,macdMargin;
	//money I have now
	double bank=initMoney;
	//sell the share once KD crossover deadly over this
	const double KDsellBar=0.7;
	//init kd,ema,macd9
	F(i,0,period-1) k[i]=d[i]=0.5;
	ema12[0]=ema26[0]=macd9[0]=close[0];
	
	//calc macd9(first period=9 day) with ema12 and ema26
	F(i,1,period-1){
		ema12[i]=(ema12[i-1]*(12-1)+close[i]*2)/(12+1);
		ema26[i]=(ema26[i-1]*(26-1)+close[i]*2)/(26+1);
		double dif=ema12[i]-ema26[i];
		macd9[i]=(macd9[i-1]*(9-1)+dif*2)/(9+1);
		//printf("%lf\n",macd9[i]);
	}
	
	vector<trade> trades;
	//main builder
	F(i,period-1,day){
		
		// KD
		double H=0,L=10000.0,C=close[i];
		//no trade today, keep the KD
		if(close[i]==-1){
			rsv[i]=rsv[i-1];
			k[i]=k[i-1];
			d[i]=d[i-1];
			continue;
		}
		
		//find the min, max among the last period(9) days
		FE(j,i-period+1,i){
			H=max(H,high[j]);
			L=min(L,low[j]);
		}
		
		//calc rsv, then kd
		if(H==L) rsv[i]=rsv[i-1];
		else rsv[i]=(C-L)/(H-L);
		k[i]=2.0/3.0*k[i-1]+1.0/3.0*rsv[i];
		d[i]=2.0/3.0*d[i-1]+1.0/3.0*k[i];
		//printf("%lf %lf\n",k[i]*100,d[i]*100);
		
		// MACD
		ema12[i]=(ema12[i-1]*(12-1)+close[i]*2)/(12+1);
		ema26[i]=(ema26[i-1]*(26-1)+close[i]*2)/(26+1);
		double dif=ema12-ema26;
		macd9[i]=(macd9[i-1]*(9-1)+dif*2)/(9+1);
		//printf("%lf\n",macd9[i]);
		
		//buy 1000 share if golden crossover and bank has enough money to afford
		if(k[i-1]<=d[i-1]&&k[i]>d[i]){
			if(bank-close[i]*1000>=1000){
				trade tmp;
				tmp.buyDay=i;
				tmp.buyPrice=close[i];
				tmp.share=1000;
				trades.push_back(tmp);
				
				buy[i]=1000;
				bank-=close[i]*1000;
				totalShare+=1000;
			}
		}
		
		//sell the stock once the deadly croossover occured and today's d is higher
		//than the bar
		if(k[i-1]>=d[i-1]&&k[i]<d[i]&&d[i]>=KDsellBar){
			sell[i]+=totalShare;
			bank+=totalShare*close[i];
			totalShare=0;
			trades.clear();
		}
		
		//stop losing money
		double stopBar=-0.15;
		F(j,0,trades.size()){
			double profit=(close[i]-trades[j].buyPrice)/trades[j].buyPrice;
			if(profit<=stopBar){
				sell[i]=trades[j].share;
				bank+=sell[i]*close[i];
				totalShare-=sell[i];
				trades.erase(trades.begin()+j);
				j--;
			}
		}
	}
}

int main(){
	puts("This strategy is composed of the following rules:");
	puts("1.Buy 1000 share (if possible) when the KD crossover goldenly");
	puts("2.Sell all the shares when the KD croossover deadly");
	puts("3.Sell 1000 share if its current profit is less than -15%");
	puts("\nDefinitions:");
	puts("KD crossover goldenly: previous day K<=D while today K>D");
	puts("KD crossover deadly: previous day K>D while today K<D AND today's D>=75");
	puts("\n\n");
	
	
	
	
	
	
	
	FILE *fp;
	if((fp=fopen("allQuotes.csv","r"))==NULL){
		puts("allQuotes.csv not found");
		return 0;
	}
	
	//loop through each tickers and deal with them
	char* line=NULL;
    size_t len=0;
    ssize_t read;
	double totalProfit=0.0;
	int success=0;
	while((read=getline(&line,&len,fp))!=-1){
		//get ticker
		int l=strlen(line);
		string ticker="";
		F(i,0,l-1) ticker+=line[i];
		if(infoLevel>=3) cout<<ticker<<"\n";
		
		//get input
		int day=input(ticker);
		if(day<=0) continue;
		
		//clean the old data and build the KD strategy 
		clean(day);
		build(day);
		
		//run the simulator on that strategy
		double profit=sim(day,ticker);
		
		//prob bankrupted
		if(profit<=-5000.0) continue;
		totalProfit+=profit;
		success++;
	}
	
	if(infoLevel>=2) printf("Average profit: %.2lf%%\n",totalProfit/success);
	
	puts("Suggestion for today:");
	for(auto i:infos){
		if(i.amt>0) cout<<i.ticker<<" buy  "<<i.amt<<" profit: "<<i.profit<<"%\n";
		if(i.amt<0) cout<<i.ticker<<" sell "<<(-i.amt)<<" profit: "<<i.profit<<"%\n";
	}
	
	puts("\n=======================");
	puts("Tickers that the KD strategy works");
	for(auto i:infos){
		if(i.profit>0&&i.amt>0){
			cout<<i.ticker<<"\tprofit:"<<i.profit<<"%\n";
		} 
	}
	fclose(fp);
}
