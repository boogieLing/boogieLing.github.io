#include <bits/stdc++.h>
using namespace std ;

int get_random(int low, int high) {
    std::random_device rd ;
    std::mt19937 gen(rd()) ;
    std::uniform_int_distribution<int> distrib(low, high) ;
    return distrib(gen) ;
}

vector<int> create_arr(int num, int l, int r){
    vector<int> ans ;
    srand(time(NULL)) ;
    for (int i=0 ; i<num ; ++i) {
        ans.push_back(l+(rand()%(r-l+1))) ;
    }
    return ans ;
}

class QSort
{
public:
    explicit QSort(const vector<int> &arr, int choice=1):_cache(arr), _sz(arr.size()) {
        auto begin=std::chrono::steady_clock::now();

        if (1==choice) {
            solve1(this->_cache, 0, (this->_sz)-1) ;
        } else if (2==choice) {
            solve2(this->_cache) ;
        }
        
        auto end = std::chrono::steady_clock::now() ;
        auto ans = std::chrono::duration<double, std::milli>(end-begin).count() ;
        printf("--cost: %lfms--\n", ans);
        printf("--judge:%s--\n", this->_judge()? "true": "false");
    }
    friend ostream &operator<<(ostream &os, const QSort &_a) {
        for (const int &iter:_a._cache) {
            os<<iter<<" ";
        }
        os<<'\n' ;
        return os ;
    }
    void solve1(vector<int> &arr, int l, int r, int k=0) {
        if (l>=r || 0>k) {
            return ;
        } else {
            int pos = _rand_pos(arr, l, r) ;
            int cnt = pos-l+1 ;
            if (0==k) {
                solve1(arr, l, pos-1, k) ;
                solve1(arr, pos+1, r, k) ;
                return ;
            } else {
               if (cnt>k) {
                   solve1(arr, l, pos-1, k) ;
               } else if (cnt<k) {
                   solve1(arr, pos+1, r, k-cnt) ;
               } else {
                   return ;
               } 
            }
        }
    }
    void solve2(vector<int> &arr) {
        const int begin = 0 ;
        const int end = arr.size()-1 ;
        stack<pair<int, int>> stk ;
        stk.push(make_pair(begin, end)) ;
        while (!stk.empty()) {
            int l = stk.top().first ;
            int r = stk.top().second ;
            stk.pop() ;
            if (l>=r && stk.empty()) {
                break ;
            }

            int pivot = arr[r] ;
            int lower_r = l-1 ;
            int upper_l = r-1+1 ;
            int equal_r = l ;
            while (equal_r<=upper_l-1) {
                if (arr[equal_r]<pivot) {
                    ++lower_r ;
                    swap(arr[equal_r], arr[lower_r]) ;
                    ++equal_r ;
                } else if (arr[equal_r]>pivot) {
                    --upper_l ;
                    swap(arr[equal_r], arr[upper_l]) ;
                } else {
                    ++equal_r ;
                }

            }
            assert(equal_r==upper_l) ;
            //++equal_r ;
            swap(arr[equal_r], arr[r]) ;
            if (lower_r>l) {
                stk.push(make_pair(l, lower_r)) ;
            }
            if (r>upper_l) {
                stk.push(make_pair(upper_l, r)) ;
            }
        }
        return ;
    }
private:
    vector<int> _cache ;
    int _sz ; 
    
    int _rand_pos(vector<int> &arr, int l, int r) {
        srand(time(NULL)) ;
        int temp = l+(rand()%(r-l+1)) ;
        swap(arr[temp], arr[r]) ;
        return _get_cnt(arr, l, r) ;
    }
    int _get_cnt(vector<int> &arr, int l, int r) {
        int pivot = arr[r] ;
        int cnt = l-1 ;
        for (int i=l ; i<r ; ++i) {
            if (arr[i]<pivot) {
                ++cnt ;
                swap(arr[cnt], arr[i]) ;
            }
        }
        ++cnt ;
        swap(arr[cnt], arr[r]) ;
        return cnt ;
    }
    bool inline _judge() {
        for (int i=0 ; i<_sz-1 ; ++i) {
            if (_cache[i+1]<_cache[i]) {
                return false ;
            }
        }
        return true ;
    }
};

class MSort
{
public:
    MSort(const vector<int> &arr):_cache(arr), _sz(arr.size()) {
        auto begin=std::chrono::steady_clock::now();

        solve(this->_cache) ;

        auto end = std::chrono::steady_clock::now() ;
        auto ans = std::chrono::duration<double, std::milli>(end-begin).count() ;
        printf("--cost: %lfms--\n", ans);
        printf("--judge:%s--\n", this->_judge()? "true": "false");
    }
    friend ostream &operator<<(ostream &os, const MSort &_a) {
        for (const int &iter:_a._cache) {
            os<<iter<<" ";
        }
        os<<'\n' ;
        return os ;
    }
    void solve(vector<int> &arr) {
        if (1>=arr.size()) {
            return ;
        } else {
            for (int st=1 ; st<=_sz ; st<<=1) {
                for (int i=0 ; i<_sz-st ; i+=(st<<1)) {
                    _merge(arr, i, i+st-1, min(i+(st<<1)-1, _sz-1)) ;
                }
            }
        }
        return ;
    }
private:
    vector<int> _cache ;
    int _sz ;
    void _merge(vector<int> &arr, int low, int mid, int high) {
        vector<int> ans   ;
        ans.reserve(high-low+1) ;
        int l = low, r = mid+1 ;
        while (l<=mid && r<=high) {
            if (arr[l]<arr[r]) {
                ans.push_back(arr[l++]) ;
            } else if (arr[l]>arr[r]) {
                ans.push_back(arr[r++]) ;
            } else {
                ans.push_back(arr[l++]) ;
                ans.push_back(arr[r++]) ;
            }
        }
        while (l<=mid) {
            ans.push_back(arr[l++]) ;
        }
        while (r<=high) {
            ans.push_back(arr[r++]) ;
        }
        /*for (const auto &iter:ans) {
            printf("%d ", iter);
        }
        printf("\n\n");*/
        assert((int)ans.size()==(high-low+1)) ;
        int index = 0 ;
        while (low<=high) {
            arr[low++] = ans[index++] ;
        }
        return ;
    }
    bool inline _judge() {
        for (int i=0 ; i<_sz-1 ; ++i) {
            if (_cache[i+1]<_cache[i]) {
                return false ;
            }
        }
        return true ;
    }
};

signed main(int argc, char const *argv[])
{
    vector<int> arr1 = create_arr(10000, 0, 10000) ;
    QSort t1(arr1, 1) ;
    QSort t2(arr1, 2) ;
    QSort t3(arr1, 3) ;
    //cout<<t3 ;
    MSort t4(arr1) ;
    //cout<<t4 ;
    return 0;
}