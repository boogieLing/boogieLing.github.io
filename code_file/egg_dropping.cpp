#include <bits/stdc++.h>
using namespace std ;

int dp(int K, int N)
{
	vector<vector<int>> dp(K+1, vector<int>(N+1, 0)) ;
	for (int i=0 ; i<=K ; ++i) {
		dp[i][0] = 0 ;
	}
	for (int i=0 ; i<=N ; ++i) {
		dp[0][i] = 0 ;
	}
	int F = 0 ;
	while (dp[K][F++]<N) {
		for (int i=1 ; i<=K ; ++i) {
			dp[i][F] = dp[i-1][F-1]+dp[i][F-1]+1 ;
		}
		if (dp[K][F]>=N) {
			break ;
		}
	}
	return F ;
}

long long combin(int K, int F, int N) 
{
	long long ans = 0 ;
	double tmp = 1.0 ;
	for (int i=1 ; i<=K ; ++i) {
		tmp *= (double)(F-i+1) ;
		tmp /= (double)(i) ;
		ans += tmp ;
		if (ans>=N) {
			break ;
		}
	}
	return ans ;
}

int binary(int K, int N) 
{
	int l = 0 ;
	int r = N ;
	while (l+5<r) {
		int mid = l+(r-l)/2 ;
		if (combin(K, mid, N)<N) {
			l = mid ;
		} else {
			r = mid ;
		}
	}
	int ans = 0 ;
	for (int i=l ; i<=r ; ++i) {
		if (combin(K, i, N)>=N) {
			ans = i ;
			break ;
		}
	}
	return ans ;
}



signed main(int argc, char const *argv[])
{
	int n=0 ;
	int k=0 ;
	while (~scanf("%d %d", &n, &k)) {
		printf("%d\n", dp(n, k));
		printf("%d\n", binary(n, k));
	}
	
	return 0;
}
/*
1 2
2 6
3 14

*/