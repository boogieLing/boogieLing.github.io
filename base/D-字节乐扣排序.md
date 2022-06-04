# 字节乐扣排序

## DP

1. 接雨水

    1. 对于每个点，贪心地预处理左边最高和右边最高，遍历求和

2. 最大连续子数组和

    1. ```go
        for i:=1 ; i<len(nums); i++{
            nums[i] = max(nums[i-1]+nums[i], nums[i])
            ans = max(ans, nums[i])
        }
        ```

3. 跳跃游戏，带权的跳跃问能否到达终点

    1. ```go
        stepCnt := nums[0]
        for i:=1 ; i<n ; i++ {
            if stepCnt<=0 {
            	return false
            }
        	stepCnt = max(nums[i], stepCnt-1)
        }
        return true
        ```
        
    2. 维护剩余能跳跃步数即可

4. 不同路径，矩阵左上角到右下角的不同路径

    1. ```go
        for i:=1 ; i<n ; i++ {
            for j:=1 ; j<m ; j++ {
                dp[j] = dp[j] + dp[j-1]
            }
        }
        return dp[m-1]
        ```

5. 爬楼梯， 1、2阶

6. 编辑距离，删除、插入、替换

    1. ```go
        for i:=1 ; i<=n ; i++{
            for j:=1 ; j<=m ; j++ {
                tmp := dp[i-1][j-1]
                if word1[i-1]!=word2[j-1] {
                    tmp++
                }
                dp[i][j] = min(dp[i-1][j]+1, min(dp[i][j-1]+1, tmp))
            }
        }
        ```

7. 三角形最小路径和

    1. ```go
        for i:=1 ; i<len(triangle) ; i++ {
            for j:=0 ; j<len(triangle[i]) ; j++ {
                if j<len(triangle[i-1]) {
                    top := triangle[i-1][j]
                    if j>0 {
                        left := triangle[i-1][j-1]
                        triangle[i][j] += min(left, top)
                    } else {
                        triangle[i][j] += triangle[i-1][j]
                    }
                } else {
                    triangle[i][j] += triangle[i-1][j-1]
                }
            }
        }
        ```

8. 股票买卖，一只股票

    1. ```go
        curMin = min(prices[i], curMin)
        ans = max(prices[i]-curMin, ans)
        ```

9. 零钱兑换，最少零钱数量，完全背包

    1. ```go
        for i:=0 ; i<=amount ; i++ {
            dp[i] = 0x3f3f3f3f3f
        }
        dp[0] = 0
        for i:=1 ; i<=amount ; i++ {
            for _ , v := range(coins) {
                if i-v>=0 {
                    dp[i] = min(dp[i], dp[i-v]+1)
                }
            }
        }
        if dp[amount]>amount {
            return -1
        }
        return dp[amount]
        ```

10. 矩阵中的最长递增路径

    1. 对于每个点的值降序排序，从最高值开始dp找周围四个点

    2. ```cpp
        if (di>=0 && dj>=0 && di<n && dj< m && matrix[di][dj]>matrix[i][j]) {
            dp[i][j] = max(dp[i][j], dp[di][dj]+1) ;
        }
        ```

11. 整数拆分， 给定一个正整数 *n*，将其拆分为**至少**两个正整数的和，并使这些整数的乘积最大化。 

     1. ```go
         for i:=2 ; i<=n ; i++ {
             for j:=1 ; j<i ; j++ {
                 dp[i] = max(dp[i], max(j*(i-j), j*dp[i-j]))
             }
         }
         ```

12. 青蛙过河，hard

     1. 一只青蛙想要过河。 假定河流被等分为 x 个单元格，并且在每一个单元格内都有可能放有一石子（也有可能没有）。 青蛙可以跳上石头，但是不可以跳入水中。

         给定石子的位置列表（用单元格序号升序表示）， 请判定青蛙能否成功过河（即能否在最后一步跳至最后一个石子上）。 开始时， 青蛙默认已站在第一个石子上，并可以假定它第一步只能跳跃一个单位（即只能从单元格1跳至单元格2）。

         如果青蛙上一步跳跃了 k 个单位，那么它接下来的跳跃距离只能选择为 k - 1、k 或 k + 1个单位。 另请注意，青蛙只能向前方（终点的方向）跳跃。

      2. ```go
          for i, _ := range stones {
              if i==0 {
                  continue
              }
              if stones[i]-stones[i-1] > i 
                  return false
              }
          }
          for i:=0 ; i<n ; i++ {
              for j:=i-1 ; j>=0 ; j-- {
                  k := stones[i]-stones[j]
                  if k > j+1 {
                      // 由于是从后往前，所以可以直接break
                      break
                  }
                  dp[i][k] = dp[j][k-1] || dp[j][k] || dp[j][k+1]
                  if i==n-1 && dp[i][k] {
                      return true
                  }
              }
          }
          ```

13. 分割数组的最大值，给定一个非负整数数组 `nums` 和一个整数 `m` ，你需要将这个数组分成 `m` 个非空的连续子数组。设计一个算法使得这 `m` 个子数组各自和的最大值最小。

      1. ```go
          sub[0] = nums[0]
          for i:=0 ; i<=n ; i++ {
              dp[i] = make([]int, m+1)
              if i<n {
                  sub[i+1] = sub[i] + nums[i]
              }
              for j:=0 ; j<=m ; j++ {
                  dp[i][j] = math.MaxInt32
              }
          }
          dp[0][0] = 0
          for i:=1 ; i<=n ; i++ {
              for j:=1 ; j<=min(i, m) ; j++ {
                  for k:=0 ; k<i ; k++ {
                      dp[i][j] = min(dp[i][j], max(sub[i]-sub[k], dp[k][j-1]))
                  }
              }
          }
          ```

14. 最长上升子序列的方案数

      1. ```go
          for i, iV := range nums {
              dp[i] = 1
              cnt[i] = 1
              for j, jV := range nums[ :i] {
                  if iV > jV {
                      if dp[j]+1 > dp[i] {
                          dp[i] = dp[j] + 1
                          cnt[i] = cnt[j]
                      } else if dp[j]+1 == dp[i] {
                          cnt[i] += cnt[j]
                      }
                  }
              }
              if maxLen < dp[i] {
                  maxLen = dp[i]
                  ans = cnt[i]
              } else if maxLen == dp[i] {
                  ans += cnt[i]
              }
          }
          ```

15. 最长回文子串

     1. n2 dp，枚举right，枚举left

      2. ```go
          maxLen := 1
          maxLeft := 0
          maxRight := 0
          for right:=1 ; right<n ; right++ {
              for left:=0 ; left<right ; left++ {
                  if s[left]==s[right] && (right-left+1 <= 3 || dp[left+1][right-1]) {
                      dp[left][right] = true
                      if right-left+1 > maxLen {
                          maxLen = right-left+1
                          maxLeft = left
                          maxRight = right
                      }
                  }
              }
          }
          return s[maxLeft:maxRight+1]
          ```

      3. 马拉车

16. 正则表达式匹配，给你一个字符串 `s` 和一个字符规律 `p`，请你来实现一个支持 `'.'` 和 `'*'` 的正则表达式匹配。

      1. ```go
          dp[0][0] = true
          dp[1][1] = s[0]==p[0] || p[0]=='.'
          for i := 1 ; i <= m ; i++ {
          	// 这一步是额外补丁，可以先不写出来
              if p[i-1] == '*' {
                  dp[0][i] = dp[0][i-2]
              }
          }
          
          for i := 1 ; i <= n ; i++ {
              for j := 1 ; j <= m ; j++ {
                  if s[i-1] == p[j-1] || p[j-1] == '.' {
                      dp[i][j] = dp[i-1][j-1]
                  } else if p[j-1] == '*' {
                      if s[i-1] != p[j-2] && p[j-2] != '.' {
                          dp[i][j] = dp[i][j-2]
                      } else {
                          dp[i][j] = dp[i][j-1] || dp[i-1][j] || dp[i][j-2]
                      }
                  }
              }
          }
          ```

17. 最长有效括号

     1. dp，讨论 ....() 和 ((....)) 和 ()((...))

      2. ```go
          for i := 1 ; i < n  ; i++ {
              if s[i] == ')' {
                  if s[i-1] == '(' {
                      dp[i] = 2
                      if i-2 >= 0 {
                          dp[i] += dp[i-2]
                      }
                  }
                  if s[i-1] == ')' {
                      mirror := i-dp[i-1]-1
                      if mirror >= 0 && s[mirror] == '(' {
                          dp[i] = dp[i-1] + 2
                          if mirror - 1 >= 0 {
                              dp[i] += dp[mirror-1]
                          }
                      }
                  }
              }
              if ans < dp[i] {
                  ans = dp[i]
              }
          }
          ```

18. 跳跃游戏2，假设可达，求最小步数

     1. 维护最右可达和当前步数可达，只要在当前步数可达范围内ans就不变

      2. ```go
          for i:=0 ; i<n-1 ; i++ {
              maxRight = max(maxRight, i+nums[i])
              if i== curEnd {
                  ans++
                  curEnd = maxRight
              }
          }
          ```

      3. 维护最右边界和当前步数的end，只要不超过end，ans都不应该改变

19. 单词拆分，问字符串能否被拆分成一个或多个在字典中出现的单词

     1. dp，枚举right，枚举left

      2. ```GO
          checkMap := make(map[string]bool)
          for _, v := range wordDict {
              checkMap[v] = true
          }
          dp := make([]bool, n+1)
          dp[0] = true
          for i := 1 ; i <= n ; i++ {
              for j := 0 ; j < i ; j++ {
                  if dp[j] && checkMap[s[j:i]] {
                      dp[i] = true
                      break
                  } 
              }
          }
          ```

20. 买卖股票4，限制买卖次数k

      1. ```go
          buy[0][0] = -prices[0]
          sell[0][0] = 0
          for i := 1 ; i <= k ; i++ {
              buy[0][i] = math.MinInt32
              sell[0][i] = math.MinInt32
          }
          for i := 1 ; i < n ; i++ {
              buy[i][0] = max(buy[i-1][0], sell[i-1][0]-prices[i])
              for j := 1 ; j <= k ; j++ {
                  buy[i][j] = max(buy[i-1][j], sell[i-1][j]-prices[i])
                  sell[i][j] = max(sell[i-1][j], buy[i-1][j-1]+prices[i])
                  // 这个j-1要注意，代表操作数减少
              }
          }
          ans := 0
          for _, v := range sell[n-1] {
              if v > ans {
                  ans = v
              }
          }
          return ans
          ```

21. 打家劫舍3，树形dp

      1. ```go
          var _dfs func(node *TreeNode) []int
          _dfs = func(node *TreeNode) []int {
              if node == nil {
                  return []int{0, 0}
              }
              leftMax, rightMax := _dfs(node.Left), _dfs(node.Right)
              curSelect := node.Val + leftMax[1] + rightMax[1]
              curNoSelect := max(leftMax[0], leftMax[1]) + max(rightMax[0], rightMax[1])
              return []int{curSelect, curNoSelect}
          }
          ```

22. 买卖股票，一次交易，卖出时带手续费

      1. ```go
          dp[0][0] = 0
          dp[0][1] = -prices[0]
          for i := 1 ; i < n ; i++ {
              dp[i][0] = max(dp[i-1][0], dp[i-1][1]+prices[i]-fee)
              dp[i][1] = max(dp[i-1][1], dp[i-1][0]-prices[i])
          }
          ```

      2. 可以滚动为O1空间

23. 最小体力爬楼梯

      1. ```go
          dp0, dp1 := 0, 0
          for i := 2 ; i <= n ; i++ {
              tmpAns := min(dp0+cost[i-2], dp1+cost[i-1])
              dp0, dp1 = dp1, tmpAns
          }
          ```

24. 通配符匹配，\*表示匹配任意字符串或空串

      1. ```go
          for i := 1 ; i <= m ; i++ {
              if p[i-1] == '*' {
                  dp[0][i] = true
              } else {
                  break
              }
          } 
          dp[0][0] = true
          for i := 1 ; i <= n ; i++ {
              for j := 1 ; j<=m ; j++ {
                  if s[i-1]==p[j-1] || p[j-1]=='?' {
                      dp[i][j] = dp[i-1][j-1]
                  }
                  if p[j-1] == '*' {
                      dp[i][j] = dp[i-1][j] || dp[i][j-1]
                  }
              }
          }
          ```

25. 打家劫舍1，不相邻元素最大和

      1. ```go
          for i := 2 ; i <= n ; i++ {
          	dp[i] = max(dp[i-1], dp[i-2]+nums[i-1])
          }
          ```

26. 最长上升子序列

      1. ```go
          for i := 1 ; i < n ; i++ {
              dp[i+1] = 1 
              for j := 0 ; j < i ; j++ {
                  if nums[i] > nums[j] {
                      dp[i+1] = max(dp[i+1], dp[j+1]+1)
                  }
              }
              ans = max(dp[i+1], ans)
          }
          return ans
          ```

27. 最佳观光组合，一个正整数数组 `values`，其中 `values[i]` 表示第 `i` 个观光景点的评分，并且两个景点 `i` 和 `j` 之间的 **距离** 为 `j - i`。一对景点（`i < j`）组成的观光组合的得分为 `values[i] + values[j] + i - j` ，也就是景点的评分之和 **减去** 它们两者之间的距离。**返回一对观光景点能取得的最高分。**

      1. ```GO
          curMax := values[0]-0
          curPos := 0
          for i := 1 ; i < n ; i++ {
              tmpAns := values[i]+curMax-(i-curPos)
              ans = max(ans, tmpAns)
              if values[i] >= curMax-(i-curPos) {
                  curMax = values[i]
                  curPos = i
              }
          }
          ```

      2. 对于每个点，只需要知道之前的最大值就行了

28. 杨辉三角2， 输出第n行的序列

      1. dp模拟
      2. 结论

29. 杨辉三角，给n，输出生成的序列

      1. dp模拟

30. 01矩阵，给定一个由 0 和 1 组成的矩阵 mat ，请输出一个大小相同的矩阵，其中每一个格子是 mat 中对应位置元素到最近的 0 的距离。两个相邻元素间的距离为 1 

      1. 预处理，0和math.MaxInt32
      2. 左上角开始dp
      3. 右下角开始dp

31. 买卖股票，限制2次

      1. 跟限制k次一样的做法

32. 视频拼接，把区间内的视频合并为T时长，求使用到的区间数量的最小值

      1. 枚举1-T时长，然后枚举区间，如果覆盖，就可以转移

      2. ```go
          flag := 0
          dp[0] = 0
          for i := 1 ; i <= time ; i++ {
              for _, jV := range clips {
                  l, r := jV[0], jV[1]
                  if l < i && r >= i && dp[l] + 1 < dp[i] {
                      dp[i] = dp[l] + 1
                      if i == time {
                          flag = 1
                      }
                  }
              }
          }
          if flag == 0 {
              return -1
          }
          ```

33. 解码方法，把纯数字字符串解码为A-Z的字符串，求解码方案总数

      1. ```go
          if n >= 1 && s[0] == '0' {
              return 0
          }
          for i, v := range s {
              if v != '0' {
                  dp[i+1] = dp[i]
              }
              if i-1 >= 0 && ((s[i-1]-'0')*10 + s[i]-'0')<=26 && s[i-1] != '0' {
                  dp[i+1] += dp[i-1]
              }
          }
          ```

34. 不同的二叉树，给你一个整数 `n` ，求恰由 `n` 个节点组成且节点值从 `1` 到 `n` 互不相同的 **二叉搜索树** 有多少种？返回满足题意的二叉搜索树的种数。

       1. 真不会，是结论
       2. n个节点能产生多少种二叉树：**h(n)=h(n-1)\*(4\*n-2)/(n+1);**

35. 只有两个键的键盘，最初记事本上只有一个字符 'A' 。你每次可以对这个记事本进行两种操作：

          Copy All（复制全部）：复制这个记事本中的所有字符（不允许仅复制部分字符）。
          Paste（粘贴）：粘贴 上一次 复制的字符。

      给你一个数字 n ，你需要使用最少的操作次数

      1. ```go
          for i := 2; i <= n; i++ {
          f[i] = math.MaxInt32
              for j := 1; j*j <= i; j++ {
                  if i%j == 0 {
                      f[i] = min(f[i], f[j]+i/j)
                      f[i] = min(f[i], f[i/j]+j)
                  }
              }
          }
          ```

36. 最大全1正方形

       2. dp i j 表示以 i, j为右下角的正方形的最大周长

       2. https://leetcode-cn.com/problems/maximal-square/solution/zui-da-zheng-fang-xing-by-leetcode-solution/

       3. ```go
           if dp[i][j] == 1 {
           	dp[i][j] = min(min(dp[i-1][j], dp[i][j-1]), dp[i-1][j-1]) + 1
               if dp[i][j] > maxSide {
                   maxSide = dp[i][j]
               }
           }
           
           ```

37. 全一正方形的数目

       1. 需要看证明 https://leetcode-cn.com/problems/count-square-submatrices-with-all-ones/solution/tong-ji-quan-wei-1-de-zheng-fang-xing-zi-ju-zhen-2/

38. 最大全1矩形

     1. https://blog.csdn.net/whzyb1991/article/details/47008875

39. 全1矩形的数目

     1. https://leetcode-cn.com/problems/count-submatrices-with-all-ones/solution/tong-ji-quan-1-zi-ju-xing-by-leetcode-solution/

40. 矩阵的最小路径和，从左上角走到右下角的最小路径和

       1. ```go
           for i := 1 ; i < m ; i++ {
               dp[i] = dp[i-1] + grid[0][i]
           }
           for i := 1 ; i < n ; i++ {
               dp[0] += grid[i][0]
               for j := 1 ; j < m ; j++ {
                   dp[j] = min(dp[j], dp[j-1]) + grid[i][j]
               }
           }
           ```

41. 完全平方数，给定正整数 n，找到若干个完全平方数（比如 1, 4, 9, 16, ...）使得它们的和等于 n。你需要让组成和的完全平方数的个数最少。

       给你一个整数 n ，返回和为 n 的完全平方数的 最少数量 

       1. ```go
           for i := 1 ; i <= n ; i++ {
               minAns := math.MaxInt32
               for j := 1 ; j*j <= i ; j++ {
                   minAns = min(minAns, dp[i - j*j])
               }
               dp[i] = minAns + 1
           }
           ```

42. 打家劫舍2，环形dp

       1. 两个dp，一个从（不偷，偷第一个开始）一个从（不偷，偷第二个开始），然后从第三个开始转移，最后对比两种

       2. ```go
           dp0[0], dp0[1] = 0, nums[0]
           dp1[0], dp1[1] = 0, nums[1]
           for i := 2 ; i < n ; i++ {
               dp0[i] = max(dp0[i-1], dp0[i-2]+nums[i-1])
               dp1[i] = max(dp1[i-1], dp1[i-2]+nums[i])
           }
           return max(dp0[n-1], dp1[n-1])
           ```

           



---

## 单调栈

1. 最大矩形，找01矩阵中最大的只包含1的子矩阵



---

## DFS

1. 二叉树最大路径和

    1. ```go
        var _dfs func(node *TreeNode) int
        _dfs = func(node *TreeNode) int {
            if node == nil {
                return 0
            }
            leftMax := max(0, _dfs(node.Left))
            rightMax := max(0, _dfs(node.Right))
            tmpAns := node.Val + leftMax + rightMax
            ans = max(ans, tmpAns)
            return node.Val + max(leftMax, rightMax)
        }
        ```

2. 