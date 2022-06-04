# 手撕代码

## 鸡蛋掉落

假设有N个鸡蛋，N层楼。请求出能够找到最小的鸡蛋可摔破楼层的最少尝试次数。

**DP**

转化问题，以dp(k, f)表示k个鸡蛋，f次尝试能确定的最大层数

1. 碎了，步数和蛋数都减一
2. 没碎，步数减一

dp(k, f) = dp(k-1, f-1)+dp(k-1, f)+1

+1表示当前楼层能立刻确定，当dp(k, f)>=N的时候立刻取出。

**组合式求解**

状态转移是：dp(k, f) = dp(k-1, f-1)+dp(k-1, f)+1

我们令g(k, f) = dp(k,f+1) - dp(k, f)

就有

```pascal
g(k, f) 
= dp(k, f+1) - dp(k, f)
= dp(k-1, f)+dp(k-1, f+1) - dp(k-1, f-1)-dp(k-1, f)
= (dp(k-1, f+1)-dp(k-1. f)) + (dp(k-1, f)-dp(k-1, f-1))
= g(k-1, f) + g(k-1, f-1)
```

显然，$g(k, f) = \binom{k}{f}$

dp(0, f)应该恒为0，但是g(0, f)当f为0的时候为1

所以修正为$g(k, f) = \binom{k}{f+1}$

此时有

```pascal
dp(k, f) = (dp(k,f)-dp(k, f-1))
		 + (dp(k,f-1)-dp(k, f-2))
		 ...
		 +(dp(k, 1)-dp(k, 0))
		 +dp(k, 0)
=g(k, f-1)+...+g(k, 0)
```

得$dp(k, f) = \sum_{i=1}^{f}\binom{k}{i}$

所以可以二分上式的f，当值大于等于k的时候取出f即可

---

## LCS

LCS可以转化为LIS，假设匹配串长度为n，模式串长度为m，第i位字母在匹配串中重复次数为r(i)；

那么时间复杂度为$ O(log(\sum_{i=0}^{m-1}(r(i)))*\sum_{i=0}^{m-1}(r(i))) $

---

## 字节毒蘑菇

从起点开始接下来有 100 个方块，相邻方块间的距离都为 1，每个方块上有增加体力的食用蘑菇或减少体力的毒蘑菇，蘑菇带来的体力改变是已知的。一个人初始体力为 m，每次可以往前跳任意个方块，体力耗尽就会死掉。

1. 每跳一次消耗的体力与跳的距离成正比，比例为1。问这个人能否跳到终点，如果能，求可能剩余的最大体力。
2. 每跳一次消耗的体力是跳的距离的平方。问这个人能否跳到终点，如果能，求可能剩余的最大体力。
3. 每跳一次消耗的体力是跳的距离的平方，每一次跳跃得一分，求到达终点的最大分数。

==

1. 贪心取所有正数蘑菇

2. ```cpp
    int solve(vector<int>& mushrooms, int m){
        int n = mushrooms;
        vector<int> maxEnergy(n, 0);
        maxEnergy[0] = m;
        for (int i = 0; i < n; i++){
            if (maxEnergy[i] == 0)  continue;
            for (int j = i + 1; j <= i + sqrt(maxEnergy[i]) && j < n; j++){
                maxEnergy[j] = max(maxEnergy[j], maxEnergy[i] - (j - i) * (j - i) + mushrooms[j]);
            }
        }
        return maxEnergy.back();
    }
    ```

3. 不会

## 字节简化路径

链接：https://leetcode-cn.com/problems/simplify-path/

给你一个字符串 `path` ，表示指向某一文件或目录的 Unix 风格 **绝对路径** （以 `'/'` 开头），请你将其转化为更加简洁的规范路径。

## 字节矩阵DFS

给一个N*M的数组,0表示可以访问,1表示不可以访问,可不可以从左上角(起始)到右下角(结束)并且把每个0结点都访问到,且每个结点只能访问一次,输出路径,如果不能访问返回空

A：先预处理数组,把所有能访问结点的个数count出来,然后维护一个是否访问的数组isVisit,和结果集List<Point(x,y)>res
每次dfs先判断当前结点是否是最后一个结点,如果是,那count是否为0了,为0返回res;不为0,res.remove(),设置未访问,count++;

## 字节完全二叉树的最后一个节点

链接：https://blog.csdn.net/qq_39117858/article/details/104832104

## 网易互娱随机小球

https://blog.csdn.net/fanyun_01/article/details/81272249