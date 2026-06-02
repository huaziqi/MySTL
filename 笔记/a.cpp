#include<iostream>
using namespace std;

template<class T>
T clac(T a, T b){
    return a+b;
}

int main(){
    cout<<clac(1.2,2.0)<<endl;

}