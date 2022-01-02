#include<bits/stdc++.h>
using namespace std;

#define F(i,L,R) for(int i=L;i<R;i++)
#define FE(i,L,R) for(int i=L;i<=R;i++)

struct Ticker{
	string name;
	double expectedClose,close;
	double expectedEPS,expectedDividend;
	double delta;//expectClose-close
};

const int MIN_VOLUME=2000;

double close,volume;
map<string,double> dividend;
map<string,double> eps;
vector<Ticker> tickers;
bool isNum(char s){return '0'<=s&&s<='9';}
void clear(){dividend.clear();eps.clear();}
bool cmp(Ticker a,Ticker b){return a.delta>b.delta;}

int inputValue(string ticker){
	FILE *fp;
	ticker="value\\"+ticker+".csv";
	char filename[50];
	F(i,0,ticker.size()) filename[i]=ticker[i];
	filename[ticker.size()]='\0';
    if((fp=fopen(filename,"r"))==NULL){
    	cout<<filename<<" not found\n";
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
	F(i,0,22) getline(ss,token,',');
	F(i,0,22) getline(ss,token,',');
	F(i,0,22) getline(ss,token,',');
	while(getline(ss,token,',')){
		bool trash=0;
		string date;
		double money;
		
		//date
		getline(ss,token,',');
		if(!isNum(token[0])) trash=1;
		if(!trash) date=token;
		
		//trash
		F(i,0,17) getline(ss,token,',');
		
		//eps
		getline(ss,token,',');
		if(token==""||token=="-"||trash) money=-1000;
		else money=stod(token);
		
		//trash
		F(i,0,2) getline(ss,token,',');
		
		if(!trash) eps[date]=money;
	}
	//puts("EPS");
	//for(auto i:eps) cout<<i.first<<": "<<i.second<<"\n";
	fclose(fp);
	return eps.size();
}

int inputDividend(string ticker){
	FILE *fp;
	ticker="dividend\\"+ticker+".csv";
	char filename[50];
	F(i,0,ticker.size()) filename[i]=ticker[i];
	filename[ticker.size()]='\0';
    if((fp=fopen(filename,"r"))==NULL){
    	cout<<filename<<" not found\n";
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
	F(i,0,3) getline(ss,token,',');
	while(getline(ss,token,',')){
		//date
		getline(ss,token,',');
		if(token.length()!=10){
			getline(ss,token,',');
			continue;
		}
		string year=token.substr(0,4);
		
		//dividend
		getline(ss,token,',');
		if(token==""||token=="-") continue;
		double money=stod(token);
		
		if(dividend.find(year)!=dividend.end()) dividend[year]+=money;
		else dividend[year]=money;
	}
	//puts("DIVIDEND");
	//for(auto i:dividend) cout<<i.first<<": "<<i.second<<"\n";
	fclose(fp);
	return dividend.size();
}

int inputClose(string ticker){
	FILE *fp;
	ticker="data\\"+ticker+".csv";
	char filename[50];
	F(i,0,ticker.size()) filename[i]=ticker[i];
	filename[ticker.size()]='\0';
    if((fp=fopen(filename,"r"))==NULL){
    	cout<<filename<<" not found\n";
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
	while(getline(ss,token,',')){
		//date
		getline(ss,token,',');
		
		//open
		getline(ss,token,',');
		
		//high
		getline(ss,token,',');
		
		//low
		getline(ss,token,',');
		
		//close
		getline(ss,token,',');
		if(token!=""&&token!="-") close=stod(token);
		
		//adj close
		getline(ss,token,',');
		//volume
		getline(ss,token,',');
	}
	//printf("CLOSE: %lf\n",close);
	return close;
}

int inputVolume(string ticker){
	FILE *fp;
	ticker="data\\"+ticker+".csv";
	char filename[50];
	F(i,0,ticker.size()) filename[i]=ticker[i];
	filename[ticker.size()]='\0';
    if((fp=fopen(filename,"r"))==NULL){
    	cout<<filename<<" not found\n";
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
	while(getline(ss,token,',')){
		//date
		getline(ss,token,',');
		
		//open
		getline(ss,token,',');
		
		//high
		getline(ss,token,',');
		
		//low
		getline(ss,token,',');
		
		//close
		getline(ss,token,',');
		
		//adj close
		getline(ss,token,',');
		
		//volume
		getline(ss,token,',');
		if(token!=""&&token!="-") volume=stod(token);
	}
	//printf("CLOSE: %lf\n",close);
	return volume;
}

void solve(string ticker){
	double totRatio=0;
	for(auto i:eps){
		string year=i.first;
		if(year=="21Q1") continue;
		double money=0;
		int yearInt=stoi(year)-1;
		stringstream ss;
		ss<<yearInt;
		
		if(dividend.find(year)!=dividend.end()) money=dividend[year];
		double currRatio=money/i.second;
		if(currRatio<0) currRatio=1;
		totRatio+=currRatio;
	}
	double avgRatio=totRatio/(eps.size()-1);
	if(avgRatio>1) avgRatio=1;
	
	double expectEPS=eps["21Q1"]*4;
	double expectDividend=expectEPS*avgRatio;
	double expectClose=expectDividend*20;
	if(expectClose>close){
		Ticker now;
		now.name=ticker;
		now.expectedClose=expectClose;
		now.close=close;
		now.expectedEPS=expectEPS;
		now.expectedDividend=expectDividend;
		now.delta=expectClose-close;
		if(volume>=MIN_VOLUME*1000) tickers.push_back(now);
		//cout<<ticker<<"\n";
		//printf("Expect: %lf Current: %lf\n",expectClose,close);
		//printf("ExpectEPS: %lf ExpectDividend: %lf\n",expectEPS,expectDividend);
	}
}

int main(){
	FILE *fp;
	if((fp=fopen("TSE.tls","r"))==NULL){
		puts("TSE.tls not found");
		return 0;
	}
	char* line=NULL;
    size_t len=0;
    ssize_t read;
	tickers.clear();
	while((read=getline(&line,&len,fp))!=-1){
		int l=strlen(line);
		string ticker="";
		F(i,0,l-1) ticker+=line[i];
		string valueTicker="";
		for(char c:ticker){
			if(c!='.') valueTicker+=c;
			else break;
		}
		
		clear();
		int day=inputValue(valueTicker);
		if(day==0) continue;
		inputDividend(ticker);
		inputClose(ticker);
		inputVolume(ticker);
		solve(ticker);
	}
	
	sort(tickers.begin(),tickers.end(),cmp);
	for(auto i:tickers){
		cout<<i.name<<" Delta: "<<i.delta<<" ("<<i.delta*100.0/i.close<<"%)"<<"\n";
		printf("Expected: %lf Current: %lf\n",i.expectedClose,i.close);
		printf("ExpectedEPS: %lf ExpectedDividend: %lf\n",i.expectedEPS,i.expectedDividend);
		puts("");
	}
}