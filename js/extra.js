marked.setOptions({
    renderer: new marked.Renderer(),
    highlight: (code) => hljs.highlightAuto(code).value,
});
// window 失去焦点
window.onblur = function () {
    let ans = $("h1").text();
    document.title = ans.slice(1);
};

// window 获得焦点
window.onfocus = function () {
    document.title = "R0 烤面筋百分百路边摊制霸";
};

$(document).ready(function () {
    // console.log("qwq") ;
    $(function () {
        $("html").niceScroll({
            cursorcolor: "#f09199",
            cursorwidth: "8px",
            autohidemode: false,
            cursorborder: "0px solid #fff",
            nativeparentscrolling: false
        });
    }); // FROM: https://github.com/inuyaksa/jquery.nicescroll
    get_markdown("./extra_md/多多.md");
    document.title = "R0 烤面筋百分百路边摊制霸";
    $(".ali_icon").click(function () {
        $("div#toc-box").toggleClass("toc_hidden");
    });
    $("#toc-box .file_box").on("click", ".file_item", function () {
        const ans = $(this).find(".file_a").attr("href");
        get_markdown(ans);
        // $(".file_box").removeClass("file_item_box_hidden") ;
    });
    $("#toc-box .file_box .file_box_btn").click(function () {
        $(".file_box").toggleClass("file_item_box_hidden");
    });
    get_markdown_file("./py_extra.xml", "").catch(err => {

    });


});

function get_markdown(path) {
    $.ajax({
        type: "GET",
        url: path,
        dataType: "text",
        data: {},
        success: function (data) {
            let ans = marked.parse(data);
            // console.log(ans.indexOf("<pre><code"));
            // $("main.content").html(ans) ;
            $("main.content").html("");
            change_head_img_info(`word-${wordCount(ans)}-%2381ecec`);
            $("main.content").append(html2dom(ans));
            clear_ul_point();
            clear_task_point();
            code_block_add_hljs();
            code_block_add_line_1();
            code_block_link_clear_indent();
            code_block_add_lang();
            $("#toc").html("");
            toc(1, 4, "toc", "content");
            // change_title() ;
            // setTimeout(function(){$("html").getNiceScroll().resize()}, 500) ;
            // $("html").getNiceScroll().resize() ;
            setTimeout(function () {
                $("html").getNiceScroll().resize();
            }, 1000);
        },
        error: function (jqXHR) {
            console.log("Error: " + jqXHR.status);
        }
    });
}

function html2dom(html_text) {
    return $(html_text);
}

function code_block_add_hljs() {
    $("main.content pre").each(function () {
        const code_len = $(this).find("code").length;
        if (1 <= code_len) {
            $(this).addClass("hljs");
        }
    });
}

function code_block_add_lang() {
    $("main.content pre.hljs").each(function () {
        let code_lang = $(this).find("code").attr("class");
        code_lang = code_lang.replace("language-", "");
        $(this).prepend(`<div class="code_lang">${code_lang}</div>`);
    });
}

function code_block_add_line() {
    let index = 0;
    $("main.content pre.hljs code").each(function () {
        let lines = $(this).text().split(/\n/).slice(0, -1);
        let $numbering = $("<ul/>").addClass("pre-numbering");
        lines.map(function (item, index) {
            $numbering.append($("<li/>").text(index));
            // return '<li><span class="line-num" data-line="' + (index + 1) + '"></span>' + item + '</li>';
        });
        $(this).addClass("has-numbering").parent().append($numbering);
    });
}

function code_block_add_line_1() {
    $("main.content pre.hljs code").each(function () {
        let lines = $(this).html().split(/\n/).slice(0, -1);
        var html_text = lines.map(function (item, index) {
            return `<li><span class="line_num" data-line=${index + 1}>${index + 1}</span>${item}</li>`;
        }).join("");
        html_text = `<ul class="code_line">${html_text}</ul>`;
        $(this).html(html_text);
    });
}

function code_block_link_clear_indent() {
    $("ul.code_line").each(function () {
        $(this).css("list-style-type", "none");
        $(this).css("padding-inline-start", "0");
    });
}

function clear_ul_point() {
    $("ul").each(function () {
        $(this).css("list-style-type", "none");
    });
}

function clear_task_point() {
    $("ul").each(function () {
        const input_len = $(this).find("input").length;
        if (1 <= input_len) {
            $(this).addClass("task_ul");
        }
    });
    $("ul.task_ul").each(function () {
        $(this).css("list-style-type", "none");
    });
}

function change_title() {
    let ans = $("h1").text();
    document.title = ans.slice(1);
}

function wordCount(data) {
    var pattern = /[a-zA-Z0-9_\u0392-\u03c9]+|[\u4E00-\u9FFF\u3400-\u4dbf\uf900-\ufaff\u3040-\u309f\uac00-\ud7af]+/g;
    var m = data.match(pattern);
    var count = 0;
    if (m == null) {
        return count;
    }
    for (var i = 0; i < m.length; i++) {
        if (m[i].charCodeAt(0) >= 0x4E00) {
            count += m[i].length;
        } else {
            count += 1;
        }
    }
    return count;
}

function change_head_img_info(data) {
    $("#word-cnt").remove();
    $("div.head").append(`<div class="img_box"><img id="word-cnt" src="https://img.shields.io/badge/${data}"></div>`);
}

//读取summary文件中指定的xml文件以获取md名
//传入的参数xmlDile为xml文件url即文件路径，album是md文件夹的名字
//例如对于9_HongKong.xml文件，其对应的相册名是9_HongKong
async function get_markdown_file(xmlFile, albumName) {

    //生成xml对象。
    var xmlDoc = createXMLDoc(xmlFile);
    // console.log(xmlDoc)
    //检验xml对象
    if (xmlDoc == null) {
        alert("您的浏览器不支持该功能, 请使用Firefox或Chrome!");
    }

    //获取照片节点
    var photos = xmlDoc.getElementsByTagName("md");
    //图片数量
    var len = photos.length;

    //src图片src, alt 图片alt
    //设定地址

    var destination = "" + albumName;
    if (albumName !== "") {
        destination += "\\";
    }
    var src;
    var alt;
    // for (var i = len-1; ~i ; --i) {
    for (var i = 0; i < len; ++i) {
        photoName = photos[i].childNodes[0].nodeValue;
        //photos[i].childNodes[0].nodeValue
        src = destination + photoName;
        alt = photoName;
        const alt_name = ((photoName.split("/").pop()).split("\\").pop()).split(".")[0];
        // alert(photoName);

        const new_item = `<span class="file_item">
                <i class="iconfont icon-icon-"></i>
                <span href="${src}" class="file_a">${alt_name}</span>
        </span>`;
        const $new_item = $(new_item);
        // $(".grid").append($new_item) ;
        $("div.file_box .file_item_box").append($new_item);
    }

    return len;//将图片数量返回，方便处理计数，分页等问题
}


//读取xml文件并生成XMLDocument对象
//针对不同浏览器，读取xml文件有不同操作。
function createXMLDoc(xmlFile) {
    var xmlDoc;
    if (window.ActiveXObject) {
        xmlDoc = new ActiveXObject("Microsoft.XMLDOM");//IE浏览器
        xmlDoc.async = false;
        xmlDoc.load(xmlFile);
    }
        //  else if (isFirefox=navigator.userAgent.indexOf("Firefox")>0) { //火狐浏览器
        // //  else if (document.implementation && document.implementation.createDocument) {//这里主要是对谷歌浏览器进行处理
        //     xmlDoc = document.implementation.createDocument('', '', null);
        //     xmlDoc.async=false;
        //     xmlDoc.load(xmlFile);
    // }
    else { //谷歌浏览器
        var xmlhttp = new window.XMLHttpRequest();
        xmlhttp.open("GET", xmlFile, false);
        xmlhttp.send(null);
        if (xmlhttp.readyState == 4) {
            xmlDoc = xmlhttp.responseXML.documentElement;
        }
    }

    return xmlDoc;
}

function unableScroll() {
    var top = $(document).scrollTop();
    $(document).on("scroll.unable", function (e) {
        $(document).scrollTop(top);
    });
}