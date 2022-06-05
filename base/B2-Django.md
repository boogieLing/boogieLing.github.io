# Django

## 与flask的区别

flask：

**MVC框架**

轻量级web，默认依赖两个外部库，Jinja2引擎和WSGI工具

适用于做小型网站和web服务的api，开发大型网站的时候需要自己设计架构

![这里写图片描述](../B2-Django.assets/20180720202930363)

Django：

**MTV模式**

自带对象关系映射模型和**模板引擎**

自带ORM，所以与关系型数据库的耦合度更高

自带数据库管理app

有成熟的app，模板、表单、路由等

整体封闭性更好，重量



---

## 简述MVC和MTV

**MVC：**

model+view+controller

model：负责业务与数据库的映射

view：负责与用户的交互

control：接受用户的输入来调用模型和完成请求

**MTV：**

model+template+view

model：同上

template：负责如何把页面展示给用户

view：负责业务逻辑，并在适当的时候调用model和template

此外django还有一个urls分发器

---

## wsgi、uwsgi、uWSGI

1. wsgi：**web服务器网关**，是一套协议，用于接收用户请求并将请求进行封装，然后把请求交给web框架。
   1. wsgiref：本质是一个socket服务端，django使用
   2. wsgizeug：一样，flask使用
2. uwsgi：一样是通信协议，是uWSGI服务器的独占协议，**用于定义传输信息的类型**
3. uWSGI：是一个web服务器，实现了WSGI协议，http协议等协议

----

## Django请求的生命周期

1. wsgi：封装后再交给web框架
2. 中间件：**对请求进行校验或者在请求对象中添加相关数据**，例如：csrf、request.session
3. 路由匹配：url.py，根据浏览器发送的不同url去匹配不同的视图函数
4. 视图函数：在view.py中进行业务逻辑的处理，可能涉及到：orm（数据库）、templates=>渲染
5. 中间件：相应的数据context进行处理
6. wsgi：把相应的内容context返回给浏览器

---

## Django路由name的作用

**用于反向解析路由，相当于给url取别名，只要name不变，即使对应url改变也能通过改名字找到正确的url**

---

## 列举Django的内置组件

1. Admain
2. model组件
3. form组件
4. ModelFrom组件：能用于操作数据库，也能用于用户请求的验证

---

## Django重定向

**怎么使用？**

使用 HttpResponseRedirect 

from django.http import HttpResponseRedirect 

使用redirect和reverse

**状态码**

301：永久重定向

302：临时重定向，旧地址的资源还在

---

## xss攻击

xss攻击是向网页注入恶意脚本的攻击

后果：

1. 窃取cookie
2. 读取未公开的资料

防范措施：

1. 输入过滤，不允许有js代码
2. 输出编码：把脚本代码变成字符串形式输出出来
3. cookie加密
4. 服务器端再次检查输入是否有非法内容

---

## csrf的防范机制

1. django第一次相应来自某个客户端的请求时，后端随机产生一个token值，把这个token保存在session状态
2. 下次前端要发起请求，这时候**中间件**把token值加入到请求数据或者头部中，一起传给后端。Cookies:{csrftoken:xxxxx}
3. 后端验证前端带过来的token和session中保存的是否一致

```python
#1.后端将csrftoken传到前端，发送post请求时携带这个值发送
data: {
             csrfmiddlewaretoken: '{{ csrf_token }}'
        },
#2.获取form中隐藏标签的csrftoken值，加入到请求数据中传给后端
 data: {
         csrfmiddlewaretoken:$('[name="csrfmiddlewaretoken"]').val()
         },
#3.cookie中存在csrftoken,将csrftoken值放到请求头中
headers:{ "X-CSRFtoken":$.cookie("csrftoken")}，
```

---

## runserver与uWSGI的区别

runserver是自带的，一般调试的时候用，单进程

uWSGI是一个web服务器，搭配着Nginx就是一个生产环境了，**支持的并发量更高，方便管理多进程，低内存占用**

---

## orm常用方法

1. filter：筛选
2. get
3. exclude：返回和结果不匹配的对象
4. order_by：对查询结果排序
5. reverse：反向排序
6. count：返回查询到的数量
7. distinct：从返回结果中删除重复的记录

---

## orm中三种写sql语句的方法

1. 使用execute执行自定义sql
2. 使用extra方法
3. 使用raw方法
   1. 执行原始sql语句并**返回模型**
   2. 依赖model，多用于查询

---

## django如何配置redis

redis在项目的应用

1. 记录评论数、热度、浏览量
2. 记录我的文章
3. 记录分类排行榜

总的来说，与当前浏览的对象相关的对象一般都适合用redis做缓存

string：缓存用户基本信息

list：存储列表数据，内部是一个异步队列

hash：无序字典

set：防止重复事件的发生

```python
#1.安装 pip install django-redis
#2.在setting中配置CACHES,可以设置多个缓存,根据名字使用
        CACHES = {
            "default": {
                "BACKEND": "django_redis.cache.RedisCache",
                "LOCATION": "redis://127.0.0.1:6379",
                "OPTIONS": {
                    "CLIENT_CLASS": "django_redis.client.DefaultClient",
                    "CONNECTION_POOL_KWARGS": {"max_connections": 100}
                    # "PASSWORD": "密码",
                }
            }
        },
        #另添加缓存
        "JERD": { }
#3.根据名字去连接池中获取连接
        from django_redis import get_redis_connection
        conn = get_redis_connection("default")
```

---

## PV和UV

pv：页面访问量，每打开一次页面pv+1

uv：独立访问量，一台电脑终端为一个访客

---

## 跨域及解决方法

跨域：浏览器从一个域名的网页去请求另一个域名的资源，浏览器有可能处于安全考虑不允许不同源的请求。

同源策略

1. 协议相同
2. 域名相同
3. 端口相同

**处理方法：**

1. 通过JSONP协议跨域，jsonp是一种包含在函数调用中的json，**script标签不受同源策略的影响，手动创建一个script标签，传递url，同时传入一个回调函数的名字**，服务器得到名字之后，返回数据的时候会用这个函数包裹住数据，客户端得到数据之后立刻把script标签删掉就能得到数据了。
   缺点：只能GET
2. cors：跨域资源共享
   使用自定义的http头部允许浏览器和服务器互相通信
   1. 简单请求，直接设置允许访问的域名 **response['Access-Control-Allow-Origin'] = "*"**
   2. 复杂请求，首先发送options请求做预检，然后再发送真正的POST请求。

---

## 如何实现登陆验证

1. cookie、session第一步验证以及后续数据的保存
2. token，登陆成功后生成加密字符串
3. JWT：json web token，他把用户信息加密到token之中，服务器不保存任何用户信息
4. 服务器通过使用保存的密钥来验证token的正确性

---

## 如何实现单元测试

用过，但没有深入了解。

一般使用python的unittest模块，这个模块基于雷达方法来实现定义测试。