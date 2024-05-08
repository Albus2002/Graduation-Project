#include "inc.h"
struct CM_Sketch {
	int m,w,d,key_len,l;//w为行，d为列

	uint32_t **a;
	BOBHash32 **hash;

	CM_Sketch(int _m, int _w, int _d,int _l, int _k) : m(_m), w(_w), d(_d),l(_l), key_len(_k) {
		a = new uint32_t*[w];
		rep2 (i, 0, w) {
			a[i] = new uint32_t[d];
			rep2 (j, 0, d) {
                a[i][j] = 0;
            }
		}
		hash = new BOBHash32*[w];
		rep2 (i, 0, w) hash[i] = new BOBHash32(uint8_t(rd() % MAX_PRIME32));
	}

	void insert(uint8_t *key, uint64_t clock) {
		int hashid[w];
		rep2 (i, 0, w) hashid[i] = hash[i]->run((char *)key, key_len) % d;
        for(int i = 0; i < w;++i){
			if(a[i][hashid[i]] < (1 << l -1)){
				a[i][hashid[i]] += 1;
			}
        }
	}

	double query(uint8_t *key, uint64_t clock) {
		int hashid[w];
		rep2 (i, 0, w) hashid[i] = hash[i]->run((char *)key, key_len) % d;
        double res = 2e9;
        for(int i=0;i < w;++i){
			if(a[i][hashid[i]] < (1 << l - 1))
            res = min(res,double(a[i][hashid[i]]));
        }
        return res;
	}
    void clear(){
        for(int i = 0;i < w;++i){
            for(int j = 0;j < d;++j){
                a[i][j] = 0;
            }
        }
        return;
    }
	void get_situation(){
		int  sum = 0;
		for(int i = 0;i < w;++i){
            for(int j = 0;j < d;++j){
                if(a[i][j] == 0) sum++;
            }
        }
		cout<<"all cnt:"<< w*d <<endl;
		//cout<<"zero rate:"<<sum*1.0/w/d<<endl;
        return;
	}
};
