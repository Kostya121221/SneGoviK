#include <iostream>
#include <shamir.h>
// побитовое возведение в степень
// 3^13 = 3 * 3 ^12 = 3 * 9^6 = 3 * 81^3 = 3 * 81 * 81^2 = 3 * 81 * 6561 
int mod_Pow(long long base, long long power, long long modul){
    //чтобы основание не было большим всегда уменьшаем
    base %= modul;
    long long result =  1;
    while(power > 0){
        if (power % 2 == 1){
            result = (result * base) % modul;
        }
        base = (base * base) % modul; 
        power /= 2;
    }
    return static_cast<int>(result);
}
//расширенный алгоритм евклида
int extended_Gcd(int a, int b, int &x, int &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    int x1, y1;
    int d = extended_gcd(b, a % b, x1, y1); //b*x+(a%b)*y = d
    x -= (a/b) * y; // bx + (a - (a/b) * b)) * y = d
    swap(x,y); // ay + b*(x - (a/b) * y) = d
    return d;
}


int main(){
    int n;
    cout << "Введите ваше открытое число для шифрования: " << endl;
    cin >> n;
    
    return 0;
}