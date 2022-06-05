# Flask

## 与Django的区别

flask：

**MVC框架**

轻量级web，默认依赖两个外部库，jinja2和WSGI工具

适用于做小型网站和web服务的api，开发大型网站的时候需要自己设计架构

![这里写图片描述](../B2-Flask.assets/20180720202930363)

Django：

**MTV模式**

自带对象关系映射模型和**模板引擎**

自带ORM，所以与关系型数据库的耦合度更高

自带数据库管理app

有成熟的app，模板、表单、路由等

整体封闭性更好，重量

---

## 列举Http请求中常见的请求头?

- User-Agent：浏览器类型，如果Servlet返回的内容与浏览器类型有关则该值非常有用。
- Cookie：这是最重要的请求头信息之一
- Content-Type:请求类型

---

## 依赖组件

1. route路由
2. template模板，但很少用
3. models，orm模型
4. blueprint蓝图
5. Jinja2模板渲染引擎

---

## 蓝图

蓝图Blueprint实现模块化的应用 
 \- `book_bp = Blueprint('book', __name__）`创建蓝图对象 
 \- 蓝图中使用路由`@book_bp.route('url')` 
 \- 在另一.py文件里导入和注册蓝图`from book import book_bp` `app.register_blueprint(book_bp)`

作用：

1. 把功能模块化
2. 优化项目结构
3. 增强可读性，对标django的view.py

---

