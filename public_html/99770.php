<?php
require_once ("inc/head.inc");
require_once ("inc/daiSqlite.inc");

$tag = isset($_GET["tag"])? $_GET["tag"]: "newest";
$tags = array (
	"newest" => array("NEW", ""),
	"top" => array("TOP 20", ""),
	//"newestbook" => array("NewBook",""),
	"1" => array("萌系", ""),
	"2" => array("搞笑", ""),
	"3" => array("格斗", ""),
	"4" => array("科幻", ""),
	"5" => array("劇情", ""),
	"6" => array("偵探", ""),
	"7" => array("競技", ""),
	"8" => array("魔法", ""),
	"9" => array("神鬼", ""),
	"10" => array("校園", ""),
	"11" => array("驚栗", ""),
	"12" => array("廚藝", ""),
	"13" => array("偽娘", ""),
	"14" => array("賣肉", ""),
	"15" => array("冒險", ""),
	"19" => array("小說", ""),
	"20" => array("港漫", ""),
	"21" => array("耽美", ""),
	"22" => array("經典", ""),
	"23" => array("歐美", ""),
	"24" => array("日文", ""),
	"25" => array("東方", ""),
	//"eyes" => array("<img src=\"image/eye.png\" border=\"0\">", ""),
	"hearts" => array("<img src=\"image/heart.png\" border=\"0\">", "")
);
if (array_key_exists($tag, $tags)) {
	$tags[$tag][1] = "background-color: #fff; color: #000;";
}
$top_button = "";
foreach ($tags as $key => $value) {
	$top_button .= "<li><a href=\"?tag=$key\" style=\"".$value[1]."\">".$value[0]."</a></li>";
}
$top_button .= " <input id=\"search\" size=\"12\"/ style=\"padding: 0px;\">";

$idx=0;
$comic_button = "";
$comic_ul = "<ul>";
$comicEyes = daiSqliteComicHaveEyeGet();
$comicHearts = daiSqliteComicHaveHeartGet();
$removeable = false;
$unreadonly = false;

$myComicHearts = array();
if ($user) {
	daiSqliteUserHeartComicsGet($g_type, $user, $heartComics);
	if (strlen($heartComics) > 0) {
		$myComicHearts = explode (",", $heartComics);
	}
}

switch ($tag) {
case "eyes":
	$lines = array();
	array_push ($lines, "Most Viewed");
	$keys = array_keys($comicEyes);
	foreach ($keys as $key) {
		$name = file_get_contents ("data/$key/name");
		$url = file_get_contents ("data/$key/url");
		array_push ($lines, "$name\t$url");
	}
	break;
case "hearts":
	$lines = array();
	array_push ($lines, "Most Liked");
	$keys = array_keys($comicHearts);
	foreach ($keys as $key) {
		$name = file_get_contents ("data/$key/name");
		$url = file_get_contents ("data/$key/url");
		array_push ($lines, "$name\t$url");
	}
	break;
case "myeyes":
	$lines = array();
	array_push ($lines, "My Viewed");
	if ($user) {
		daiSqliteUserEyeComicsGet($g_type, $user, $eyeComics);
		if (strlen($eyeComics) > 0) {
			$keys = array_reverse (explode (",", $eyeComics));

			foreach ($keys as $key) {
				$name = file_get_contents ("data/$key/name");
				$url = file_get_contents ("data/$key/url");
				array_push ($lines, "$name\t$url");
			}
			$removeable = true;
		}
	}
	break;
case "mylikes":
	$lines = array();
	array_push ($lines, "My Liked");
	if ($user) {
		foreach ($myComicHearts as $key) {
			$name = file_get_contents ("data/$key/name");
			$url = file_get_contents ("data/$key/url");
			array_push ($lines, "$name\t$url");
		}
	}
	break;
case "myunreadeyes":
	$lines = array();
	array_push ($lines, "My Unread Viewed");
	if ($user) {
		daiSqliteUserEyeComicsGet($g_type, $user, $eyeComics);
		if (strlen($eyeComics) > 0) {
			$keys = array_reverse (explode (",", $eyeComics));

			foreach ($keys as $key) {
				$name = file_get_contents ("data/$key/name");
				$url = file_get_contents ("data/$key/url");
				array_push ($lines, "$name\t$url");
			}
			$removeable = true;
		}
	}
	$unreadonly = true;
	break;
case "search":
	$lines = file ("/tmp/".session_id()."/search");
	break;
default:
	$lines = file ("data/$tag");
	break;
}

foreach ($lines as $line) {
	$line = trim($line);
	if (strlen($line) <= 0)
		continue;
	$arr = explode ("\t", $line);
	$name = $arr[0];
	if (count($arr) < 2) {
		$comic_button .= "<input type=\"button\" name=\"comicType\" value=\"$name\" data-options='{\"idx\":$idx}'></input>";
		$idx++;
		$comic_ul .= "<li style=\"display:none; width:100px; text-align:center; top: -12px; font-weight:bold;}\" class=\"title\">$name</li>";
	} else {
		$url = $arr[1];
		list($a, $b, $c, $d, $num) = explode ("/", $url);
		$eyes = isset($comicEyes[$num])? $comicEyes[$num]: 0;
		$hearts = isset($comicHearts[$num])? $comicHearts[$num]: 0;
		$myheart = in_array($num, $myComicHearts)? "true": "false";

		$authors = "";
		for ($i=2; $i<count($arr); $i++) {
			if (strlen($authors) <= 0) {
				$authors = $arr[$i];
			} else {
				$authors .= " ".$arr[$i];
			}
		}
		$comicdir  = "data/$num";
		if (!file_exists($comicdir)) {
			mkdir ($comicdir);
			chmod ($comicdir, 0777);
		}
		if (!file_exists("$comicdir/name")) {
			file_put_contents ("$comicdir/name", $name);
		}
		if (!file_exists("$comicdir/url")) {
			file_put_contents ("$comicdir/url", $url);
		}
		if (strlen($authors) > 0 && !file_exists("$comicdir/authors")) {
			file_put_contents ("$comicdir/authors", $authors);
		}
		if (strlen($authors) <= 0 && file_exists("$comicdir/authors")) {
			$authors = file_get_contents ("$comicdir/authors");
		}
		
		$unread = 0;
		if ($user) {
			$newviewedstr = $user? "newviewed.$g_type.$user": "newviewed";

			if (file_exists("$comicdir/$newviewedstr") && file_exists("$comicdir/comic")) {
				$newviewed = file_get_contents ("$comicdir/$newviewedstr");
				$i = strlen($newviewed);
				$fp = fopen ("$comicdir/comic", "r");
				while ($line = fgets($fp)) {
					if (substr(trim($line), ($i+1)*-1, $i) == $newviewed) {
						break;
					}
					$unread ++;
				}
				fclose ($fp);
			}
		}
		if ($unreadonly && $unread <= 0)
			continue;
		$title = strlen($authors)>0? "$name <$authors>": $name;
		$comic_ul .= "<li style=\"display:none; text-align:center; width:100px;\" data-img='{\"num\":\"$num\",\"eye\":\"$eyes\",\"heart\":\"$hearts\",\"myheart\":$myheart,\"unread\":\"$unread\", \"removeable\":\"$removeable\"}'><a href=\"#content\" data-options='{\"url\":\"$url/\",\"type\":\"comic\"}' title='$title'>$name</a>";
		$comic_ul .= "<span></span></li>";
	}
}
$comic_ul .= "</ul>";
?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" >
<title>Better 99770</title>
<link rel="stylesheet" type="text/css" href="/css/head.css"/>
<link rel="stylesheet" type="text/css" href="colorbox/example1/colorbox.css"/>
<link rel="stylesheet" type="text/css" href="lib/jquery-ui-1.8.24.custom/css/smoothness/jquery-ui-1.8.24.custom.css"/>
<link rel="stylesheet" type="text/css" href="/css/notification.css"/>
<style>
#toplist {
	padding-top: 35px;
}
#toplist ul {
}

#commicbuttons {
}

div.screenMask {
	position: absolute;
	left: 0px;
	top: 0px;
	width: 100%;
	height: 100%;
	z-index: 1000;
	background-color: #000000;
	opacity: 0.7;
	filter: alpha(opacity=70);
	display: none;
}

.leftbutton {
	position:absolute;
	height:125px;
	margin-top:30px;
	z-index:20;
}

.rightbutton {
	position:absolute;
	height:125px;
	right:10px;
	margin-top:30px;
	z-index:100;
}

#slider{
	position: absolute;
	top: 310px;
	width: 80%;
	left: 10%;
	right: 10%;
	opacity: 0.5;
}

#comiclist ul{
	position:absolute;
	display:inline;
	margin-left:50px;
}
#comiclist li{
	position:absolute;
	display:inline;
	font-size: 13px;
}
#comiclist li a {
	text-decoration: none;
	color:  #696969;
}
#comiclist li img {
	position:absolute;
	top: 15px;
	right: 0px; 
	
}
#comiclist li span {
	position:absolute;
	top: 143px;
	right: 0px;
	color: #AAAAAA;
}
#comiclist li span span {
	position:static;
}
#comiclist li span img {
	position:static;
}
#comiclist li span img.heart {
	cursor: pointer;
}
#comiclist li span div.step {
	position:absolute;
	top: -135px;
	left: 7px;
  	background: #000;
	border-radius: 0.8em;
	-moz-border-radius: 0.8em;
	-webkit-border-radius: 0.8em;
	color: #fff;
	display: inline-block;
	font-weight: bold;
	line-height: 1.6em;
	margin-right: 5px;
	text-align: center;
	width: 1.6em; 
	cursor: pointer;
}
#comiclist li span img.removeable {
	position:absolute;
	cursor: pointer;
	top: 0px;
	left: -72px;
}
#content{
	position:absolute;
	top:325px;
	font-size: 13px;
	width: 90%;
/*
	left:2.5%;
	word-wrap: break-word;
*/
}
#content li {
	float: left;
	width: auto;
	list-style-type: none;
	text-align: center;
}
#content li a {
	padding: 0.5em;
	color: #333;
	text-decoration: none;
}
#content li a:hover {
	background-color: #666;
	color: #fff;
}
#content li .page {
}

#copyright{
	position: fixed;
	left:0px;
	bottom:0px;
	font-size: 13px;
}

#feedbackchat {
	position: fixed;
	right:5px;
	bottom:0px;
	width: 300px;
}

#feedbackchat div {
	height:300px;
	overflow:auto;
	overflow-x: hidden;
}

#feedbackchat textarea {
	margin-bottom: 10px;
	margin-left: -10px;
	font-size: 11px;
}

#feedbackchat h3 {
	font-size: 12px;
	padding-left: 30px;
	padding-bottom: 5px;
	padding-top: 5px;
}
#feedbackchat ul {
	list-style-type: none;
	padding-left: 0px;
	margin-left: -10px;
}
#feedbackchat ul li {
	font-size: 11px;
	padding-bottom: 5px;
}
#feedbackchat ul li img {
	height: 28px;
	width: 28px;
}
.ui-progressbar .ui-progressbar-value {
	background-image: url(image/pbar-ani.gif);
}
</style>
<script language="javascript">
	//if ((navigator.userAgent.match(/iPhone/i)) || (navigator.userAgent.match(/iPod/i)) || (navigator.userAgent.match(/iPad/i))) {location.replace("m.php");}
</script>
<script type="text/javascript" src="lib/jquery-ui-1.8.24.custom/js/jquery-1.8.0.min.js"></script>
<script type="text/javascript" src="lib/jquery-ui-1.8.24.custom/js/jquery-ui-1.8.24.custom.min.js"></script>
<script type="text/javascript" src="colorbox/colorbox/jquery.colorbox.js"></script>
<script type="text/javascript" src="/js/imageCache.js"></script>
<script type="text/javascript" src="/js/jquery.imgMoveScroll.js"></script>
</head>
<body>

<?=$head;?>
	<div id="toplist"><nav role="navigation"><ul><?=$top_button;?></ul></nav></div>
	<div id="comicbuttons"><?=$comic_button;?></div>
	<div id="comiclist"><?=$comic_ul;?></div>
	<input class="leftbutton" type="button" name="prev" value="<"></input>
	<input class="rightbutton" type="button" name="next" value=">"></input>
	<div id="content"></div>
	<div class="screenMask"></div>
	<img src="image/loading.gif" id="loadingImage" style="display: none; position:absolute; left: 50%; margin-left:-50px; top:50%; margin-top:-50px;" />
	<div style="display:none;">
		<div id='inline_content' style='padding:10px; background:#fff;'></div>
	</div>
	<div id="slider"></div>
	<div id="copyright">
	本網站中所使用之圖片、文字，轉載或引用 <a href="http://mh.99770.cc">99770</a>
	</div>
	<div id="feedbackchat">
		<h3>Feedback</h3>
		<div>
<ul id="fwall">
<?php
	daiSqliteFeedbackDump ();
?>
</ul>
		<textarea rows="2" cols="32" name="feedback"></textarea>
		</div>
	</div>
<script type="text/javascript">
//Google Analytics
var _gaq = _gaq || [];
var base = 0;
var count = 0;
var baseLen = 100;
var base_count = Math.floor(($(document).width()-40)/(baseLen+5));
var actionStart = false;
_gaq.push(['_setAccount', 'UA-35109963-1']);
_gaq.push(['_trackPageview']);
(function() {
	var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
	ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
	var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
})();

function comicAppend (value)
{
	$(value).find('a').append("<img width=\"100px\" height=\"125px\" border=\"0\" src=\"http://img.99mh.com/comicui/"+$(value).data("img").num+".jpg\"/></img>");
	//$(value).find('span').append($(value).data("img").eye+"<img src=\"image/eye.png\">");
	if ($(value).data("img").unread > 0) {
		$(value).find('span').append("<div class=\"step\" data-img='{\"num\":\""+$(value).data("img").num+"\"}'>"+$(value).data("img").unread+"</div>");
	}
	if ($(value).data("img").myheart) {
		$(value).find('span').append("<span>"+$(value).data("img").heart+"</span> <img class=\"<?php echo ($user)? "heart":"";?>\" data-img='{\"num\":\""+$(value).data("img").num+"\"}' src=\"image/myheart.png\">");
	} else {
		$(value).find('span').append("<span>"+$(value).data("img").heart+"</span> <img class=\"<?php echo ($user)? "heart":"";?>\" data-img='{\"num\":\""+$(value).data("img").num+"\"}' src=\"image/heart.png\">");
	}
	if ($(value).data("img").removeable) {
		$(value).find('span').append("<img class=\"removeable\" data-img='{\"num\":\""+$(value).data("img").num+"\"}' src=\"image/remove.png\">");
	}
}

function jumpLocalMove(jumpLocal, duration){
	var moveLength = 0-(jumpLocal * baseLen) + "px";
	$("#comiclist li").each(function(key,value){
		if(key >= jumpLocal && key < (jumpLocal + base_count)){
			if(typeof($(value).data("img")) != "undefined" && $(value).find("img").length == 0){
				comicAppend (value);
			}
			$(value).show();
		}else{
			$(value).hide();
		}
	});
	$("#comiclist ul").animate({left:moveLength},{ "duration": duration, "easing": "linear"});
}

$(function(){
	$( "#feedbackchat" ).accordion({
		collapsible: true,
		active: false,
		create: function( event, ui ) {
			$('#feedbackchat div').scrollTop($('#feedbackchat div')[0].scrollHeight);
		},
        });
	$(".ui-accordion-header").click(function(){
		$('#feedbackchat div').scrollTop($('#feedbackchat div')[0].scrollHeight);
	});
	$("textarea[name='feedback']").keypress(function(e) {
		if (e.keyCode == 13) {
 	               var fbtext = $("textarea[name='feedback']").val();
		       if (fbtext.length <= 0)
			       return;
		       $.ajax({
				type: "POST",
				url: "post.php",
				data:{action:"feedback", text:fbtext},
				async: false,
				dataType: "json",
				success: function(data){
					$("textarea").val('');
					var who;
					if (data[1] == 0) {
						who = "<img src=\"image/guest.jpg\" title=\"Guest\">";
					} else {
						who = "<img src=\"<? echo getUserImgSrc($g_type, $user); ?>\" title=\""+data[1]+"\">";
					}
					var li = "<li id=\"tmpid\">"+who+"<span style=\"\">&nbsp;&nbsp;"+data[2]+"</span><span style=\"font-size: 10px; position: absolute; right: 5px; opacity: 0.5;\">"+data[3]+"</span></li>";
					$("#fwall").append(li);
					$("#tmpid").hide().show('slow');
					$("#tmpid").removeAttr('id');
					$('#feedbackchat div').scrollTop($('#feedbackchat div')[0].scrollHeight);
				}
		       });
			return false;
		}
        });

	$('.message').click(function(){			  
		$(this).animate({top: -$(this).outerHeight()}, 500);
	});

	$("#slider").slider({
		animate: true,
		max: ($("#comiclist li").length-1),
		slide:function(event, ui) {
			jumpLocalMove(ui.value, 0);
		}
	});
	//caching images
	var $loader;
	function loadImageEvent() {
		var val = parseInt($loader.attr('value'));
		val++;
		$loader.progressbar( "option", "value", (val/$loader.attr("max"))*100);
		$loader.attr('value',val);
	}

	function loadAllEvent() {
		$loader.progressbar( "option", "value", 100);
		$loader.attr('value',100);
		$("input[name='preload']").attr('disabled', false);
	}
	$("#search").keypress(function(e) {
		if(e.which == 13) {
			$.ajax({
				type: "POST",
				url: "post.php",
				data: {search:$(this).val()},
				//async: false,
				dataType: "json",
				success: function(data){
					location.replace ("?tag=search");
				}
			});
		}
	});
	$("#comiclist li").each(function(key,value){
		if(key == base){
			actionStart = true;
		}
		if(actionStart){
			if(key != 0){
				var mValue = $("#comiclist li").eq((key-1)).css("left").split("px");
				var beforeMarginLeft = parseInt(mValue[0])+baseLen;
			}else{
				var beforeMarginLeft = 0;
			}
			$(value).css('left',(beforeMarginLeft));
			count++;
			if(count == base_count){
				actionStart = false;
			}
			if(typeof($(value).data("img")) != "undefined"){
				comicAppend (value);
			}
			$(value).show();
		}else{
			var mValue = $("#comiclist li").eq((key-1)).css("left").split("px");
			var beforeMarginLeft = parseInt(mValue[0])+baseLen;
			$(value).css('left',(beforeMarginLeft));
			$(value).hide();
		}
	});

	$("input[name='prev']").click(function(){
		var local = $("#comiclist li:visible").index();
		if((local - base_count +1) == 0){
			return false;
		}
		if(local < (base_count*2) && (local % base_count) != 0){
			local = base_count * 2 -1;
		}
		if(((local +1) % base_count) != 0 && local < (base_count*2) && local > 0){
			local = base_count*2;
		}
		var moveLength = (0-(local - (base_count*2)+1))*baseLen + "px";
		var firstLocal = '';
			$("#comiclist li").each(function(key,value){
				if(key > (local - (base_count*2)) && key <= (local - base_count)){
					if(firstLocal == ''){
						firstLocal = key;
					}
					if(typeof($(value).data("img")) != "undefined" && $(value).find("img").length == 0){
						comicAppend (value);
					}
					$(value).show();
				}else{
					$(value).hide();
				}
			});
		$("#comiclist ul").animate({left:moveLength},200);
		$("#slider").slider("value" , firstLocal);
	});

	 $("input[name='next']").click(function(){
		var local = $("#comiclist li:visible").index();
		if(local == ($("#comiclist li").length-1)){
			return false;
		}
		var moveLength = 0-((local +1)*baseLen) + "px";
		$("#comiclist li").each(function(key,value){
			if(key > local && key <= (local + base_count)){
				$(value).show();
				if(typeof($(value).data("img")) != "undefined" && $(value).find("img").length == 0){
					comicAppend (value);
				}
			}else{
				$(value).hide();
			}
		});
		$("#comiclist ul").animate({left:moveLength},200);
		$("#slider").slider("value" , local);
	});
	$("input[name='comicType']").click(function(){
		var jumpLocal = $("#comiclist li").index($(".title").eq($(this).data("options").idx));
		jumpLocalMove(jumpLocal, 200);
		$("#slider").slider("value" , jumpLocal);
	});
	$("#comiclist li > a").click(function(){
		$.ajax({
			type: "POST",
			url: "post.php",
			data:{ action:"getPage", url:$(this).data("options").url, name:$(this).attr('title')},
			dataType: "json",
			beforeSend: function(data){
				$(".screenMask").show();
				$("#loadingImage").show();
			},
			complete: function(data){
				$(".screenMask").hide();
				$("#loadingImage").hide();
			},
			success: function(data){
				if(data[0]){
					$("#content").html(data[1]);
					$(".inline").click(function(){
						//$(".page").hide();
						$(".page").html('');
						next = $(this).next();
						$.ajax({
							type: "POST",
							url: "post.php",
							data: {type:$(this).data("options").type, url:$(this).data("options").url},
							dataType: "json",
							beforeSend: function(data){
								$(".screenMask").show();
								$("#loadingImage").show();
							},
							complete: function(data){
								$(".group1").colorbox({
									rel:'group1',maxWidth:'100%', loop:false, title:$(this).html(),
									onComplete: function(){
										$(".cboxPhoto").imgMoveScroll();
									}
								}); 
								$(".screenMask").hide();
								$("#loadingImage").hide();
							},
							success: function(data){
								var content = '';
								getcontent=true;
								$.each(data, function(idx, value) {
									content += '<a class="group1" id="p'+(idx+1)+'" href="'+value+'">'+(idx+1)+'</a> ';
								});
								content += '<input type="button" name="preload" value="preload images"></input>';
								content += '<div id="loader" max="100" value="0" style="display:block; float:right; height:20px; margin-top:3px;">';
								content += '</div>';
								next.html(content);
								$("input[name='preload']").unbind("click");
								$("input[name='preload']").click(function(){
									var img = new Array();
									$(".group1").each(function(idx){
										img.push ($(this).attr("href"));
									});
									$loader = $('#loader');
									$loader.css("width", "100px");
									$loader.attr("max", img.length);
									$loader.attr("value",0);
									$loader.css("display","inline");
									$loader.progressbar({value:0});
									imageCache.pushArray(img, loadImageEvent, loadAllEvent);
									$(this).attr('disabled',true);
								});
								$(".group1").css("color","blue");
								next.show();
							}
						});
						//$(this).css('color','red');
					});
				}else{
					alert("cannot get page");
				}
			}
		});
	});

	$('#comiclist li span img.heart').live('click', function(){
		var $t = $(this);
		$.ajax({
			type: "POST",
			url: "post.php",
			data:{action:"heartAdd", num:$(this).data("img").num},
			dataType: "json",
			success: function(data){
				if (data[1]) {
					$t.attr("src", "image/myheart.png");
				} else {
					$t.attr("src", "image/heart.png");
				}
				$t.siblings('span').text(data[0]);
			}
		});
	});
	$('#comiclist li span img.removeable').live('click', function(){
		$.ajax({
			type: "POST",
			url: "post.php",
			data:{action:"eyeRemove", num:$(this).data("img").num},
			dataType: "json",
			success: function(data){
				location.reload();
			}
		});
	});
	$('#comiclist li span div.step').live('click', function(){
		var $t = $(this);
		$.ajax({
			type: "POST",
			url: "post.php",
			data:{action:"stepRemove", num:$(this).data("img").num},
			dataType: "json",
			success: function(data){
				$t.remove();
			}
		});
	});
	$('#comiclist').on({
		touchstart:function(e){
			var ex = e.originalEvent.touches[0].pageX;
			$(this).css("cursor","move");
			var tmp = (($(this).find("ul").css("left") == 'auto')?"0px":$(this).find("ul").css("left")).split("px");
			$ul_w = parseInt(tmp[0]);
			$(this).on("touchmove",function(e){
				var tmp_ex = e.originalEvent.touches[0].pageX;;
				$('#comiclist ul').css("left",$ul_w + (tmp_ex - ex) + "px");
			});
		},
		touchend:function(){
			$(this).css("cursor","pointer");
			$(this).on("touchcancel");
		}
	});
});
</script>
</body>
</html>
