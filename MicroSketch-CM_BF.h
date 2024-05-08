#include "inc.h"

struct MicroSketch_CM {
	int win, m, n, t, size_k, hnum, log_base,bf_num,bf_length,bf_rate;
	int lst = -1;
    int cnt = 0;
    uint32_t key_len = 4;


	uint32_t ***a = nullptr;
    uint32_t **b = nullptr;
    uint32_t **s = nullptr;
    uint8_t ***bf = nullptr;
	BOBHash32 **hash = nullptr;
    BOBHash32 **hash_bf = nullptr;

	int Space() {
		return (t*size_k + 32 - size_k + ceil(5 - log(log_base)/log(2))) * n * hnum / 8;
	}

    void counter_add(int h, int idx, int cur) {
        ++s[h][idx];
        while (s[h][idx] >> (log_base * b[h][idx])) {
            s[h][idx] -= 1 << (log_base * b[h][idx]);
            if (a[h][idx][cur] == (1 << size_k) - 1) {
                ++b[h][idx];
                a[h][idx][cur] = 1 << (size_k - log_base);
                rep2 (i, 0, t) if (i != cur) {
                    /*if (rd() & ((1 << log_base) - 1) < a[h][idx][i] & ((1 << log_base) - 1))*/ a[h][idx][i] += (1 << log_base) - 1;
					a[h][idx][i] >>= log_base;
                }
            } else ++a[h][idx][cur];
        }
    }

    void counter_convert(int h, int idx, int cur) {
        /*
        if (rd() & ((1 << log_base * b[h][idx]) - 1) >= s[h][idx]) {
            s[h][idx] = 0;
        } else {
            */
            s[h][idx] = 0;
            if (a[h][idx][cur] == (1 << size_k) - 1) {
                ++b[h][idx];
                a[h][idx][cur] = 1 << (size_k - log_base);
                rep2 (i, 0, t) {
                    if (i != cur) {
                    /*if (rd() & ((1 << log_base) - 1) < a[h][idx][i] & ((1 << log_base) - 1))*/ a[h][idx][i] += (1 << log_base) - 1;
					a[h][idx][i] >>= log_base;
                    }
                }
            } else ++a[h][idx][cur];
    //    }
    }

    void counter_maintain(int h, int idx) {
        while (b[h][idx]) {
            bool mle = 0;
            rep2 (i, 0, t) if (a[h][idx][i] >> (size_k - log_base)) mle = 1;
            if (mle) return;
            --b[h][idx];
            rep2 (i, 0, t) a[h][idx][i] <<= log_base;
        }
    }

	void counter_clear(int h, int idx, int cur) {
		++cur;
		if (cur >= t) cur -= t;
        if (!a[h][idx][cur]) return;
        a[h][idx][cur] = 0;
        counter_maintain(h, idx);
	}

    void bf_clear(int h,int idx, int cur){
        ++cur;
		if (cur >= t) cur -= t;
        //cout<<h<<" "<<idx<<" "<<cur<<endl;
        bf[h][idx][cur] = 0;
        return;
    }

    void bf_set(int h, int idx, int cur){
        bf[h][idx][cur] = 1;
        return;
    }

    bool bf_query(int *hashid_bf, int cur){
        bool res = 1;
        rep2(i, 0, bf_num){
            //cout<<hashid_bf[i]<<endl;
            //cout<<bf[i][hashid_bf[i]][cur]<<endl;
            res &= bf[i][hashid_bf[i]][cur];
        }
        return res;
    }
    
	double counter_query(int h, int idx, int cur, double rate) {
        rate = 1;
        double res = s[h][idx];
        rep2 (i, 0, t - 2) {
			res += a[h][idx][cur] << (log_base * b[h][idx]);
			--cur;
			if (cur < 0) cur += t;
		}
        res += (a[h][idx][cur] << (log_base * b[h][idx])) * rate;
        return res;
	}

	double counter_query_new(int *hashid, int *hashid_bf, int cur, double rate) {
		double res = 0;
		uint32_t tmp = 2e9;

		rep2 (k, 0, hnum) {
            tmp = min(tmp, s[k][hashid[k]]);
        }
        res += tmp;

        if(bf_query(hashid_bf, cur)){
            tmp = 2e9;
            rep2(k,0,hnum){
                tmp = min(tmp, (a[k][hashid[k]][cur] << (log_base * b[k][hashid[k]])));
            }
            res += tmp;
        }

		--cur;
		if (cur < 0) cur += t;
        
		rep2 (i, 0, t - 3) {
            if(bf_query(hashid_bf, i)){
                tmp = 2e9;
                rep2 (k, 0, hnum) {
                        tmp = min(tmp, a[k][hashid[k]][cur] << (log_base * b[k][hashid[k]]));
                }
                res += tmp;
            }
            --cur;
            if (cur < 0) cur += t;
		}

        if(bf_query(hashid_bf, cur)){
            cnt++;
            tmp = 2e9;
            rep2(k,0,hnum){
                tmp = min(tmp, (a[k][hashid[k]][cur] << (log_base * b[k][hashid[k]])));
            }
            res += tmp * rate;
        }
        
		return res;
	}

	MicroSketch_CM(int _m, int _win, int _t, int _size_k, int _hnum, int _log_base,int _bf_num,int _r) : m(_m), win(_win), t(_t), size_k(_size_k), hnum(_hnum), log_base(_log_base), bf_num(_bf_num),bf_rate(_r) {
		t += 2;
		n = m;
        bf_length = n * bf_rate;
        
		a = new uint32_t**[hnum];
        b = new uint32_t*[hnum];
        s = new uint32_t*[hnum];

        bf = new uint8_t**[bf_num];

		rep2 (i, 0, hnum) {
			a[i] = new uint32_t*[n];
            b[i] = new uint32_t[n];
            s[i] = new uint32_t[n];
			rep2 (j, 0, n) {
                a[i][j] = new uint32_t[t];
                s[i][j] = b[i][j] = 0;
                rep2 (k, 0, t){
                    a[i][j][k] = 0;
                }
            }
		}
        rep2 (i, 0, bf_num) {
			bf[i] = new uint8_t*[bf_length];
			rep2 (j, 0, bf_length) {
                bf[i][j] = new uint8_t[t];
                rep2 (k, 0, t){
                    bf[i][j][k] = 0;
                }
            }
		}

		hash = new BOBHash32*[hnum];
        hash_bf = new BOBHash32*[bf_num];
		rep2 (i, 0, hnum) {
            hash[i] = new BOBHash32(uint8_t(rd() % MAX_PRIME32));
        }
        rep2(i, 0, bf_num){
            hash_bf[i] = new BOBHash32(uint8_t(rd() % MAX_PRIME32));
            //cout<<rd()<<endl;
        }
	}

	void insert(uint8_t *key, uint64_t clock) {
        //cout<<(*(key+1))<<endl;
        int cur = (clock / (win / (t-2))) % t;
        
        int hashid[hnum];
        int hashid_bf[bf_num];
		rep2 (i, 0, hnum) {
            hashid[i] = hash[i]->run((char *)key, key_len) % n;
            //if(hashid[i] >= n) cout<<111<<endl;
            //cout<<hashid[i]<<" "<<n<<endl;
        }

        rep2 (i, 0, bf_num){ 
            hashid_bf[i] = hash_bf[i]->run((char *)key, key_len) % bf_length;
            //if(hashid_bf[i] >= bf_length) cout<<111<<endl;
            //cout<<hashid_bf[i]<<endl;
        }

        double rate = 1 - 1.0 * (clock % (win / (t-2))) / (win / (t-2));
        
		if (cur != lst){
            rep2 (k, 0, hnum) {
                rep2 (idx, 0, n) {
                    counter_convert(k, idx, lst);
                    counter_clear(k, idx, cur);
                }
            }
            
            rep2(k, 0, bf_num){
                rep2(idx, 0, bf_length){
                    bf_clear(k, idx, cur);
                }
            }
        } 
        
		lst = cur;

		rep2 (k, 0, hnum) {
            counter_add(k, hashid[k], cur);
        }
        rep2 (k,0,bf_num) {
            //cout<<1111<<endl;
            bf_set(k, hashid_bf[k],cur);
            //cout<<1111<<endl;
        }
        return;
	}

	double query(uint8_t *key, uint64_t clock) {
        int cur = (clock / (win / (t-2))) % t;
		int hashid[hnum];
        int hashid_bf[bf_num];

		rep2 (i, 0, hnum) hashid[i] = hash[i]->run((char *)key, key_len) % n;
        
        rep2 (i, 0, bf_num) hashid_bf[i] = hash_bf[i]->run((char *)key, key_len) % bf_length;

        double rate = 1 - 1.0 * (clock % (win / (t-2))) / (win / (t-2));

		return counter_query_new(hashid, hashid_bf, cur, rate);
	}

    void get_bf_situation(){
        int cnt = 0;
        rep2(i,0,bf_num){
            rep2(j,0,bf_length){
                rep2(k,0,t){
                    if(!bf[i][j][k]) cnt++;
                }
            }
        }
        double res = 0;
        res = cnt*1.0 /(bf_num*bf_length*t);
        cout<<"bf zero rate: "<<res<<endl;
    }

    void get_sketch_situation(){
        int cnt = 0;
        rep2(i,0,hnum){
            rep2(j,0,n){
                rep2(k,0,t){
                    if(!a[i][j][k]) cnt++;
                }
            }
        }
        double res = 0;
        res = cnt*1.0 /(hnum*n*t);
        cout<<"sketch zero rate: "<<res<<endl;
    }
    ~MicroSketch_CM() {
        // 释放a数组
        cout<<"delete start."<<endl;
        for (int i = 0; i < hnum; ++i) {
            for (int j = 0; j < n; ++j) {
                delete[] a[i][j]; // 释放内部数组
                //cout<<"1111"<<endl;
            }
            delete[] a[i]; // 释放外层数组
            delete[] b[i]; // 释放b数组
            delete[] s[i]; // 释放s数组
        }
        delete[] a; // 释放a的最外层
        delete[] b; // 释放b的最外层
        delete[] s; // 释放s的最外层
        //cout<<"1111"<<endl;

        // 释放bf数组
        for (int i = 0; i < bf_num; ++i) {
            for (int j = 0; j < bf_length; ++j) {
                delete[] bf[i][j]; // 释放内部数组
            }
            delete[] bf[i]; // 释放外层数组
        }
        delete[] bf; // 释放bf的最外层
        //cout<<"2222"<<endl;

        // 释放hash数组
        for (int i = 0; i < hnum; ++i) {
            delete hash[i]; // 释放BOBHash32对象
        }
        delete[] hash; // 释放hash数组
        //cout<<"3333"<<endl;

        // 释放hash_bf数组
        for (int i = 0; i < bf_num; ++i) {
            delete hash_bf[i]; // 释放BOBHash32对象
        }
        delete[] hash_bf; // 释放hash_bf数组
        cout<<"delete complete."<<endl;
    }
    
};
