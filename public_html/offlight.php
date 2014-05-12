<?php
$keyword = $_POST['url-keyword'];
if($keyword){
	if(strtolower(substr($keyword,0,4)) != 'http'){
		$keyword = "http://".$keyword;
	}
}
?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" >
<title>Turn Off the Light</title>
<link rel="stylesheet" type="text/css" href="lib/jquery-ui-1.8.24.custom/css/smoothness/jquery-ui-1.8.24.custom.css"/>
<style>
body{
	overflow-y:hidden;
}
#corrent *{
	background:#999;
}
</style>
<!--[if lt IE 9]>
<script>
var IE = true;
</script>
<![endif]-->
<script type="text/javascript" src="lib/jquery-ui-1.8.24.custom/js/jquery-1.8.0.min.js"></script>
<script type="text/javascript" src="lib/jquery-ui-1.8.24.custom/js/jquery-ui-1.8.24.custom.min.js"></script>
<script>
$(function(){
	if(typeof(IE) != 'undefined'){
		$("#corrent").css("height",$(window).height()-30+"px");
	}else{
		$("#corrent").css("height",$(document).height()-50+"px");
	}
});
$(window).resize(function(){
	if(typeof(IE) != 'undefined'){
		$("#corrent").css("height",$(window).height()-30+"px");
	}else{
		$("#corrent").css("height",$(document).height()-50+"px");
	}
});
</script>
</head>
<body>
<form name="frm" action="offlight.php" method="POST">
<input type="text" name="url-keyword" value="<?=$keyword;?>"/>
<input type="submit" value="submit"/ style="display:none;">
</form>
<iframe id="corrent" width="100%" height="200px" src="<?=$keyword;?>"></iframe>
</body>
</html>
