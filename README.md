# R0_note

本质上是将 md 文件 ajax 到前端进行渲染，使用 js 控制文件之间的跳转。

## 如何收集文件

在这里该收集的文件就是 base 文件夹下的各个 md 文件，纯浏览器端的 js 本身是不能做到检查所有文件的。

所以手动写了个名为 updateXML 的 py 脚本，将指定文件收集到某个 xml 中，js 再解析 xml 中的内容，调整 xml 节点中文件的路径并生成标签。

之所以使用 xml ，只是因为利用 ActiveXObject 生成的 XMLDOM 处理节点很方便。

缺点是每次新建文件之后都要手动跑一遍，未来计划是结合我的另一个项目——docwatcher来自动完成这个工作。

## 如何解析md

使用已有的轮子：https://github.com/markedjs/marked

在上一步解析 xml 得到对应文件的 url 之后，ajax 得到文件内容，清空 main.content ，使用上述轮子转换为 html 并做好对应的补丁。

1. 清除掉 ul 的点
2. 清除掉 task 节点的点
3. 使用额外的代码块插件渲染
4. 增加渲染失败的代码块语言解析

## 其他插件

1. 基于 JQ 的滚动条 jquery.nicescroll.js
2. 高亮代码块 highlight.pack.js
3. 高亮代码块配套的序号补丁 highlightjs-line-numbers.js

## 其他问题

### 图片跨域

有的图片来源的网站不允许外链，所以后面都利用 typora 在复制网络图片的时候将图片copy到本地，js 那边再做调整。