<?php
	session_start();
	if (!file_exists("/tmp/".session_id())) {
		mkdir ("/tmp/".session_id());
	}
	date_default_timezone_set ("Asia/Taipei");
	include_once ("inc/user.inc");
	include_once ("inc/daiSqlite.inc");
?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/html">
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="target-densitydpi=device-dpi, width=device-width, initial-scale=1.0, maximum-scale=1">
	<!--<link rel="shortcut icon" href="favicon.png" type="image/vnd.microsoft.icon">-->
	<script type="text/javascript" src="js/jquery-1.8.2.min.js"></script>
	<title>Better Labs</title>
	<script type="text/javascript">
	$(function() {
		$(window).resize(function(){
		});
		$(".tile").click(function() {
			window.location = $(this).data("options").url;
		});
	});
	</script>
	<style>
	body {
		margin:0;
		background: #1d1d1d;
		font-family: "Segoe UI Semilight", "Open Sans", "Verdana", "Arial", "Helvetica", sans-serif;
	}
	.tiles {
		margin-top: 25px;
		margin-left: 25px;
	}
	.tile {
		box-shadow: inset 0px 0px 1px #FFFFCC;
		background-color: #525252;
		border: 1px solid transparent;
		color: #fff;
		cursor: pointer;
		display: inline-block;
		vertical-align: middle !important;
		margin-right: 5px;
		margin-bottom: 10px;
	}
	.tile span{
		font-size: 22pt;
		font-weight: 300;
		height: 150px;
		width: 150px;
		display: inline-block;
		display: table-cell; 
		vertical-align: middle; 
		text-align: center; 
	}
	.tile:hover { 
		outline: 3px solid #3A3A3A;
	}
	.double span {
		width: 300px;
	}
	.bg-color-blue { 
		background-color: #2D89EF !important;
	}
	</style>

</head>
<body>
<div class="tiles">
<div class="tile" data-options='{"url":"99770.php"}'><span>Better 99770</span></div>
<?php
	if ($user) {
		echo "<div class=\"tile double bg-color-blue\"}'><span>$g_first_name $g_last_name <img src=\"$g_pic\"><a href=\"logouti.php\"><img src=\"image/logout.png\"></a></span></div>";

	} else {
		echo "<div class=\"tile double bg-color-blue\" data-options='{\"url\":\"$facebookLoginUrl\"}'><span>Login with Facebook</span></div>";
	}
?>
</div>
</body>
</html>
