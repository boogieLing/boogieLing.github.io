# Git

## 分支的概念

用来标记特定代码的提交，每个分支通过 **SHA1sum** 值来标识（与md5类似，是一种校验和算法），改变分支就是改变 SHA1sum的值而已。

例：

当前有2个分支，A,C,E属于master分支，而A,B，D,F属于dev分支。

```
A----C----E（master）
 \
  B---D---F(dev)
```

它们的HEAD指针分别指向E和F

```shell
git checkout master  //选择or切换到master分支
git merge dev        //将dev分支合并到当前分支(master)中
```

合并完成后：

```
A---C---E---G(master)
 \         /
  B---D---F（dev）
```

现在ABCDEFG属于master，G是一次合并后的结果，是将E和Ｆ的代码合并后的结果，可能会出现冲突。而ABDF依然属于dev分支。可以继续在dev的分支上进行开发:

```
A---C---E---G---H(master)
 \         /
  B---D---F---I（dev）
```

------

## 获取更新

获取更新有两种方法

1. ```shell
   git fetch origin master //拉取远程的master最近一次更新
   git log -p FETCH_HEAD //查看拉回来的内容
   git merge FETCH_HEAD //合并内容到本地，如果不想合并就不执行
   ```

2. ```shell
   git pull origin master//相当于上面的fetch+merge，但是没有办法查看拉回来了什么内容
   ```

------

## git checkout

checkout命令是最喜欢问的命令

大概有以下作用

- 切换、新建分支
- 将暂存区或指定commit内的内容覆盖到工作区

```shell
git checkout master
```

这就是最普通的切换分支的用法

```shell
git checkout -b newBr
```

-b 会检查分支是否存在，不存在就新建并且切换，很方便

在错误的分支工作后，想切换到正确的分支，则使用：

```shell
git checkout master
```

“错误”分支和正确的“`mytopic`”分支可能会在在本地修改的文件中有所不同，在这种情况下，上述检出将会失败：

```shell
git checkout master
error: You have local changes to 'frotz'; not switching branches.
```

可以将`-m`标志赋给命令，这将尝试三路合并：

```shell
git checkout -m master
Auto-merging frotz
```

在这种三路合并之后，本地的修改没有在索引文件中注册，所以`git diff`会显示从新分支的提示之后所做的更改。

-

**撤销工作区的修改**

```shell
git checkout -- <filename>
```

**就是让这个文件回到最近一次`git add`或`git commit`时的状态。**

<filename>支持简单的正则表达式

比如

```shell
git checkout -- '*.c'
```

能检出所有C源文件并返回

1. 如果文件修改后之后还没有被放到暂存区（还没被add追踪），只需要直接 <code>git checkout -- <filename></code>就可以丢弃修改了，但是会直接恢复到远程版本一样的状态（最近一次merge），**慎用！**
2. 如果文件已经被add，此命令只能恢复到和暂存区一样的状态，没啥用。
3. 如果文件已经被commit，此命令就可以恢复到跟本地一样的状态（commit之前）

**注意：git checkout – . 命令会将工作区的所有文件的修改都撤消。**

这条命令把 当前目录所有修改的文件 从HEAD中签出并且把它恢复成未修改时的样子.

------

## 更靠谱的取消缓存方法

### 已经git add

可以使用  **git reset HEAD filepathname** （比如： **git reset HEAD readme.md**）来放弃指定文件的缓存，放弃所有的缓存可以使用 **git reset HEAD .** 命令。

这相当于撤销git add命令，但是本地修改不会消失。

### 已经git commit

可以使用 **git reset --hard HEAD^ **来回退到上一次commit的状态。
 此命令可以用来回退到任意版本：**git reset --hard  commitid **

你可以使用 **git log **命令来查看git的提交历史。

-----

## 如何还原已经 push 并公开的提交

1. 删除或修复新提交中的错误文件，再手动将其推送到远程存储库。

   ```shell
   git commit -m "commit message"
   ```

2. 创建一个新的提交，撤消在错误提交中所做的所有更改。

   ```shell
   git revert <name of bad commit>
   ```

-----

## git stash

通常情况下，当一直在处理项目的某一部分时，如果想要在某个时候切换分支去处理其他事情，事情会处于混乱的状态。

问题是，不想把完成了一半的工作的提交，以便以后可以快速地回到当前的工作。

解决这个问题的答案是 git stash。

**注：在未`add`之前才能执行 stash！！！！**

1. `git stash [save message]`
    保存，`save`为可选项，`message`为本次保存的注释
2. `git stash list`
    所有保存的记录列表
3. `git stash pop stash@{num}`
    恢复，`num`是可选项，通过`git stash list`可查看具体值。
   **只能恢复一次**
4. `git stash apply stash@{num}`
    恢复，`num`是可选项，通过`git stash list`可查看具体值。**可回复多次**
5. `git stash drop stash@{num}`
    删除某个保存，`num`是可选项，通过`git stash list`可查看具体值
6. `git stash clear`
    删除所有保存

------

## 提交的对象包括什么

commit是一个commit对象

- 一组文件，表示给定时间段的项目状态
- 引用父提交对象
- SHA1sum名称，提交对象的唯一标识

-----

## 创建一个存储库的步骤

全局配置

```shell
git config --global user.name "Ling_Boogie"
git config --global user.email "boogieling_o@163.com"
```

创建仓库

```shell
mkdir qwq
cd qwq
git init //重要
touch README.md
git add README.md
git commit -m "first commit"
git remote add origin https://gitee.com/ling_boogie/qwq.git //重要
git push -u origin master
```

------

## 将N次提交压缩成一次提交

- 从头开始编写新的提交

  ```shell
  git reset –soft HEAD~N &&
  git commit
  ```

- 新的提交消息中串联现有的提交消息，那么需要提取这些消息并将它们传给 git commit

  ```shell
  git reset –soft HEAD~N &&
  git commit –edit -m"$(git log –format=%B –reverse .HEAD@{N})"
  ```

-----

## Merge和rebase的区别

Merge会自动根据两个分支的共同祖先和两个分支的最新提交 
进行一个三方合并

然后将合并中修改的内容生成一个新的 commit

即merge合并两个分支并生成一个新的提交
**并且仍然后保存原来分支的commit记录**  

Rebase会从两个分支的共同祖先开始提取当前分支上的修改

然后将当前分支上的所有修改合并到目标分支的最新提交后面

如果提取的修改有多个，那git将依次应用到最新的提交后面。
**Rebase后只剩下一个分支的commit记录** 

------

