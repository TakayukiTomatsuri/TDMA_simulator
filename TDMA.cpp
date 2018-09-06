//============================================================================
// Name        : ALOHASimu.cpp
// Author      : tt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cmath>
#include <random>

#define MAXTIME 100000	//スロットの数
using namespace std;

double LAMBDA = 0.03;	//通常時のラムダ
double DELAYLAMBDA = 0.1;	//遅延再送時のラムダ
int N = 100;	//放送局数
int MAXSTDRAND = 0;	//乱数の最大値

class Broadcaster{
public:
    int duration;		//遅延時間(スロット単位なので整数値で良い)
    int fireTime;		//発火までの残り時間(スロット単位なので整数値で良い)
    bool isDelaying;	//現在、遅延状態か
    
    Broadcaster(){
        duration = 0;
        fireTime = 0;
        isDelaying = false;
    }
    
    //インターバル計算
    double interval(double random, double lambda){
        
        //printf("RAND:%lf  \n", random);
        return( ( -1.0 / lambda) * log(random) ) +1.0;
    }
    
    //通常時のインターバル計算
    void setInterval(double random){
        fireTime = (int) interval(random, LAMBDA);
        isDelaying = false;
    }
    
    //遅延時のインターバル計算
    void setDelayInterval(double random){
        fireTime = (int) interval(random, DELAYLAMBDA);
        isDelaying = true;
    }
    
    //発火するか？
    bool isFiring(){
        return fireTime == 0;
    }
    
    //時間を進ませる
    void advancingTime(){
        if(fireTime>=1) fireTime--;
        //遅延状態かどうかで遅延時間をかえる
        if(isDelaying) duration ++;
        else duration = 0;
    }
};

void simu(){
    // 乱数シード生成のため
    std::random_device rndmm;
    // メルセンヌ・ツイスタ
    std::mt19937 mt(rndmm());
    std::uniform_real_distribution<double> rnd(0,1); //0~1の実数の乱数
    Broadcaster broadcaster[N];
    int ackSlot=0;
    int duration = 0;
    
    //最初の一回！
    for(int time=0; time<1; time++){
        //全放送局を走査
        for(int index_broadcast=0; index_broadcast<N; index_broadcast++){
            broadcaster[index_broadcast].setInterval(rnd(mt));	//通常インターバル設定
        }
    }
    
    //時間を進める
    for(int slot=1; slot<MAXTIME; slot++){
        //全放送局を走査
        for(int index_broadcast=0; index_broadcast<N; index_broadcast++){
            //各局の時間をすすめる
            broadcaster[index_broadcast].advancingTime();
            
            //発火する予定のスロット(発火までの残りスロット数==0)だったならば
            if(broadcaster[index_broadcast].isFiring()){
                //TDMAなので自分が発火するべきスロットじゃないときもある
                if(slot % N != index_broadcast){
                    broadcaster[index_broadcast].isDelaying = true;	//遅延状態にする
                }
                //発火できる自分のスロットだったら次の発火までの残り時間を計算する
                else{
                    ackSlot++;	//衝突とかないから必ずACKになる
                    duration += broadcaster[index_broadcast].duration;	//発火から自分のスロットがくるまでの時間
                    broadcaster[index_broadcast].setInterval(rnd(mt));	//通常のインターバルセット。遅延状態でなくなる
                }
            }
        }
    }
    
    printf("%d %lf %lf %lf %lf %d\n", N, N*LAMBDA,LAMBDA, DELAYLAMBDA,(double) ackSlot/MAXTIME, duration);
    return;
}

int main() {
    N = 100;
    //ラムダを変えてシミュレーション
    for(LAMBDA = 0.0005; LAMBDA<=0.03; LAMBDA += 0.0005){
        simu();
    }
}
