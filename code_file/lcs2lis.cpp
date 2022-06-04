#include <bits/stdc++.h>
using namespace std ;

class BIT
{
public:
	BIT(int n):_size(n),_tree(n+1, 0) {

	}
	int query(int x) {
		int ans = 0 ;
		while (0<x) {
			ans = max(ans, _tree[x]) ;
			x -= _lowbit(x) ;
		}
		return ans ;
	}
	void update(int x, int i) {
		while (x<=_size) {
			_tree[x] = max(_tree[x], i) ;
			x += _lowbit(x) ;
		}
	}
private :
	const static inline int _lowbit(int x) {
		return x&(-x) ; 
	}
	int _size ;
	vector<int> _tree ;
};


int solve1(string str1, string str2) 
{
	unsigned b_packet = 0 ;
	int len1 = 0 ;
	int len2 = 0 ;
	while (str1[len1] || str2[len2]) {
		str1[len1] && len1++ ;
		str2[len2] && len2++ ;
		b_packet = b_packet 
				| (1<<((int)(str1[len1-1]-'a'+1)))
				| (1<<((int)(str2[len2-1]-'a'+1))) ;
	}
	string ustr = "" ;
	int index = 0 ;
	while (0!=b_packet) {
		if (b_packet&1) {
			char tmp = (char)(index-1+'a') ;
			ustr.push_back(tmp) ;
		}
		++index ;
		b_packet >>= 1 ;
	}
	vector<vector<int>> cnt(len1+len2+1, vector<int>()) ;
	for (int i=0 ; i<len1 ; ++i) {
		str1[i] = lower_bound(ustr.begin(), ustr.end(), str1[i])-ustr.begin()+1 ;
		cnt[str1[i]].push_back(i+1) ;
	}
	vector<int> str3 ;
	for (int i=0 ; i<len2 ; ++i) {
		str2[i] = lower_bound(ustr.begin(), ustr.end(), str2[i])-ustr.begin()+1 ;
		for (int j=cnt[str2[i]].size()-1 ; ~j ; --j) {
			str3.push_back(cnt[str2[i]][j]) ;
		}
	}
	int ans = 0 ;
	BIT btt(len1+len2+1) ;
	for (const auto &iter:str3) {
		int tmp = btt.query(iter-1)+1 ;
		btt.update(iter, tmp) ;
		ans = max(ans, tmp) ;
	}
	return ans ;
}

int solve2(string s1, string s2) 
{
	const int len1 = s1.length() ;
	const int len2 = s2.length() ;
	vector<vector<int>> dp(len1+1, vector<int>(len2+1, 0)) ;
	for (int i=1 ; i<=len1 ; ++i) {
		for (int j=1 ; j<=len2 ; ++j) {
			if (s1[i-1]==s2[j-1]) {
				dp[i][j] = dp[i-1][j-1]+1 ;
			} else {
				dp[i][j] = max(dp[i-1][j], dp[i][j-1]) ;
			}
		}
	}
	return dp[len1][len2] ;
}

string solve3(string s1, string s2)
{
	const int len1 = s1.length() ;
	const int len2 = s2.length() ;
	vector<vector<int>> dp(len1+1, vector<int>(len2+1, 0)) ;
	int maxLen = -1 ;
	int indexEnd = -1 ;
	for (int i=0 ; i<len1 ; ++i) {
		for (int j=0 ; j<len2 ; ++j) {
			if (s1[i]==s2[j]) {
				if (0==i || 0==j) {
					dp[i][j] = 1 ;
				} else {
					dp[i][j] = dp[i-1][j-1]+1 ;
				}
			} else {
				dp[i][j] = 0 ;
			}
			if (dp[i][j]>maxLen) {
				maxLen = dp[i][j] ;
				indexEnd = i ;
			}
		}
	}
	return s1.substr(indexEnd-maxLen+1, maxLen) ;
}

signed main(int argc, char const *argv[])
{
	string s1, s2 ;
	while (cin>>s1>>s2) {
		cout<<solve1(s1, s2)<<'\n' ;
		cout<<solve2(s1, s2)<<'\n' ;
		cout<<solve3(s1, s2)<<'\n' ;
	}
	return 0;
}
/*
abcde
ace
aabcad
bcad
*/