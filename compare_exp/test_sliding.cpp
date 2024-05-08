#include "../Sliding_Sketch.h"
#include <map>

int M = 6e3, T = 16, H = 3, size_k = 6, log_base = 1;
int key_len = 4;
const int MAXINPUT = 1e7;
int WIN = 1e6;
int bf_num = 4;//布隆过滤器哈希个数
int bf_rate = 16;
int debug = 1;
int mem = 500;
uint32_t item[MAXINPUT];
uint64_t timestamp[MAXINPUT];

double cal_AAE(map<uint32_t,int> mp,Sliding_Sketch* sketch,uint64_t timestamp){
	double res = 0;
	int sum = 0;
	for(auto &it:mp){
		sum++;
		double q = sketch->query((uint8_t*)(&it.first),timestamp);
		res += abs(q - it.second);
	}
	res = res / sum;
	return res;
}

double cal_ARE(map<uint32_t,int> mp, Sliding_Sketch* sketch,uint64_t timestamp){
	double res = 0;
	int sum = 0;
	for(auto &it:mp){
		sum++;
		double q = sketch->query((uint8_t*)(&it.first),timestamp);
		res += abs(q - it.second) / (it.second);
	}
	res = res / sum;
	return res;
}

double cal_PR(map<uint32_t,int> mp, Sliding_Sketch* sketch,uint64_t timestamp,uint32_t threshold){
	double res = 0;
	int real = 0;
	int sum_s = 0;
	for(auto &it:mp){
		if(sketch->query((uint8_t*)(&it.first),timestamp) >= threshold){
			sum_s++;
			if(it.second >= threshold){
				real++;
			}
		}
	}
	res = real * 1.0 / sum_s;
	return res;
}

double cal_RR(map<uint32_t,int> mp, Sliding_Sketch* sketch,uint64_t timestamp,uint32_t threshold){
	double res = 0;
	int real = 0;
	int sum_r = 0;
	for(auto &it:mp){
        if(it.second >= threshold){
            sum_r++;
            if(sketch->query((uint8_t*)(&it.first),timestamp) >= threshold){
                real++;
            }
        }
	}
	res = real * 1.0 / sum_r;
	return res;
}


int main(int argc, char *argv[]) {
	freopen("output_sliding.txt", "a", stdout);
	//参数依次为mem, WIN, T, size_k, log_base,bf_num,bf_rate
	//转换命令行参数
	mem = std::stoi(argv[1]);
	WIN = std::stoi(argv[2]);
	T = std::stoi(argv[3]);
	size_k = std::stoi(argv[4]);
	log_base = std::stoi(argv[5]);
	bf_num = std::stoi(argv[6]);
	bf_rate = std::stoi(argv[7]);

	srand(time(NULL));
	ifstream fin("00.dat", ios::binary);
	if (!fin) {
		cerr << "Error opening file" << endl;
		return 0; // 或者根据需要处理错误
	}
	for(int i = 0;i < MAXINPUT;++i){
		uint8_t tmp[16];
		fin.read((char *)tmp, 16);
		memcpy(item + i,tmp + 8,4);
		timestamp[i] = i;
	}
	int M = mem * 1024 * 8 / H / (32 - size_k + (T + 2) * size_k + ceil(5 - log(log_base)/log(2)));
	int t_mem = mem + int(1 * T * bf_num * bf_rate * M / 8 / 1024);
	if(debug){
        cout<<"mem: "<<mem<<",";
		cout<<"WIN: "<<WIN<<",";
		cout<<"T: "<<T<<",";
		cout<<"size_k: "<<size_k<<",";
		cout<<"H: "<<H<<",";
		cout<<"log_base: "<<log_base<<",";
		cout<<"bf_num: "<<bf_num<<",";
		cout<<"bf_rate: "<<bf_rate<<endl;
	}
    int w = 3;
    int l = 16;
    int unit = 10;
    int d = t_mem * 1024 * 4 / w / l;
    int s = 5;
	Sliding_Sketch* sketch = new Sliding_Sketch(t_mem, w, d, l,s,key_len);
	double avrg_aae  = 0;
	double avrg_are  = 0;
	double avrg_pr  = 0;
	double avrg_rr  = 0;
    int scan = 1;
	for(int i = 0; i < MAXINPUT;++i){
		if(scan % (WIN / 5) == 0) sketch->insert((uint8_t*)(&item[i]),timestamp[i],1);
        else sketch->insert((uint8_t*)(&item[i]),timestamp[i]);
        scan += 1;
		if((i != 0) && (i % WIN == 0)){
			map<uint32_t,int> mp;
			for(int j = 0; j < WIN;++j){
				if(mp.find(item[i-j])!=mp.end()){
					mp[item[i-j]] += 1;
				}
				else{
					mp[item[i-j]] = 1;
				}
			}
			double aae = cal_AAE(mp,sketch,timestamp[i]);
			avrg_aae += aae;
			double are = cal_ARE(mp,sketch,timestamp[i]);
			avrg_are += are;
			double pr = cal_PR(mp,sketch,timestamp[i],WIN / 100);
			avrg_pr += pr;
			double rr = cal_RR(mp,sketch,timestamp[i],WIN / 100);
			avrg_rr += rr;
		}
		//cout<<1111<<endl;
	}
	avrg_aae = avrg_aae / ( MAXINPUT / WIN) ;
	avrg_are = avrg_are / ( MAXINPUT / WIN) ;
	avrg_pr = avrg_pr / ( MAXINPUT / WIN) ;
	avrg_rr = avrg_rr / ( MAXINPUT / WIN) ;
	cout<<"avrg_aae: " << avrg_aae << endl;
	cout<<"avrg_are: " << avrg_are << endl;
	cout<<"avrg_pr: " << avrg_pr << endl;
	cout<<"avrg_rr: " << avrg_rr << endl;
    return 0;
}