#include "inc.h"
struct Omni_Window {
	int m,w,d,key_len,l,win,sub_win;//w为行，d为列
    int time = 0;
    int cnt = 0;
    int head_i = 0;
    int head_j = 0;

	uint32_t ***a;
	BOBHash32 **hash;

	Omni_Window(int _m, int _w, int _d,int _l, int _win, int _s,int _k) : m(_m), w(_w), d(_d),l(_l), win(_win), sub_win(_s),key_len(_k) {
		a = new uint32_t**[w];
		rep2 (i, 0, w) {
			a[i] = new uint32_t*[d];
			rep2 (j, 0, d) {
                a[i][j] = new uint32_t[sub_win];
                rep2 (k,0,sub_win){
                    a[i][j][k] = 0;//相当于sub_win个cm sketch
                }
            }
		}
		hash = new BOBHash32*[w];
		rep2 (i, 0, w) hash[i] = new BOBHash32(uint8_t(rd() % MAX_PRIME32));
	}

    void clear(){
        for(int i=0;i<w;++i){
            for(int j=0;j<d;++j){
                for(int k = 0;k < sub_win;++k){
                    a[i][j][k] = 0;
                }
            }
        }
        return;
    }

	void insert(uint8_t *key, uint64_t clock,bool scan = 0) {
		cnt++;
        int hashid[w];
		rep2 (i, 0, w) hashid[i] = hash[i]->run((char *)key, key_len) % d;
        for(int i = 0; i < w;++i){
			if(a[i][hashid[i]][time] < (1 << l -1)){
				a[i][hashid[i]][time] += 1;
			}
        }
        if(cnt == (win / sub_win)){
            time = (time + 1) % sub_win;
            cnt = 0;
        }
        return;
	}

	double query(uint8_t *key, uint64_t clock) {
		int hashid[w];
		rep2 (i, 0, w) hashid[i] = hash[i]->run((char *)key, key_len) % d;
        double res = 2e9;
        for(int i=0;i < w;++i){
            int tmp = 0;
            for(int k = 0;k < sub_win; ++k){
                if(a[i][hashid[i]][k] < (1 << l - 1)){
                    tmp += a[i][hashid[i]][k];
                }
            }
            if(tmp != 0) res = min(res,double(tmp));
        }
        return res;
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
