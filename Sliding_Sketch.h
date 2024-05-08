#include "inc.h"
struct Sliding_Sketch {
	int m,w,d,key_len,l,sub_win;//w为行，d为列
    int time = 0;
    int head_i = 0;
    int head_j = 0;

	uint32_t ***a;
	BOBHash32 **hash;

	Sliding_Sketch(int _m, int _w, int _d,int _l, int _s,int _k) : m(_m), w(_w), d(_d),l(_l), sub_win(_s),key_len(_k) {
		a = new uint32_t**[w];
		rep2 (i, 0, w) {
			a[i] = new uint32_t*[d];
			rep2 (j, 0, d) {
                a[i][j] = new uint32_t[2];
                rep2 (k,0,2){
                    a[i][j][k] = 0;
                }
            }
		}
		hash = new BOBHash32*[w];
		rep2 (i, 0, w) hash[i] = new BOBHash32(uint8_t(rd() % MAX_PRIME32));
	}

    void update(){
        int cnt = w*d / sub_win + 1;
        bool stop = false;
        for (; head_i < w; ++head_i) {
            for (; head_j < d; ++head_j) {
                a[head_i][head_j][0] =a[head_i][head_j][1];
                a[head_i][head_j][1] = 0;
                cnt--;
                if (!cnt)
                    stop = true;
            }
            if (stop)
                break;
        }

        if (head_i == w && head_j == d)
            head_j = head_i = 0;
        return;
    }

	void insert(uint8_t *key, uint64_t clock,bool scan = 0) {
		int hashid[w];
		rep2 (i, 0, w) hashid[i] = hash[i]->run((char *)key, key_len) % d;
        for(int i = 0; i < w;++i){
			if(a[i][hashid[i]][1] < (1 << l -1)){
				a[i][hashid[i]][1] += 1;
			}
        }
        if(scan) update();
        return;
	}

	double query(uint8_t *key, uint64_t clock) {
		int hashid[w];
		rep2 (i, 0, w) hashid[i] = hash[i]->run((char *)key, key_len) % d;
        double res = 2e9;
        for(int i=0;i < w;++i){
            int tmp = 0;
            if(a[i][hashid[i]][0] < (1 << l - 1)) tmp += a[i][hashid[i]][0];
			if(a[i][hashid[i]][1] < (1 << l - 1)) tmp += a[i][hashid[i]][1];
            if(tmp != 0) res = min(res,double(tmp));
        }
        return res;
	}
    void clear(){
        for(int i = 0;i < w;++i){
            for(int j = 0;j < d;++j){
                for(int k = 0; k < 2; ++k){
                    a[i][j][k] = 0;
                }
            }
        }
        return;
    }
    // void get_situation(){
    //     int sum = 0;
    //     for(int i = 0;i < w;++i){
    //         for(int j = 0;j < d;++j){
    //             a[i][j] = 0;
    //         }
    //     }
    //     return;
    // }
};
