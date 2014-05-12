<?php

function url_exists($url)
{
	$handle = curl_init($url);
	curl_setopt($handle,  CURLOPT_RETURNTRANSFER, TRUE);
	/* Get the HTML or whatever is linked in $url. */
	$response = curl_exec($handle);
	/* Check for 404 (file not found). */
	$httpCode = curl_getinfo($handle, CURLINFO_HTTP_CODE);
	curl_close($handle);
	return ($httpCode == 404 || $httpCode == 0) ? false: true;
}

session_start();
if (!file_exists("/tmp/".session_id())) {
	mkdir ("/tmp/".session_id());
}
date_default_timezone_set ("Asia/Taipei");
include_once ("inc/user.inc");
include_once ("inc/daiSqlite.inc");
$head = "";
/*
// check 99770 dead or not
$url99770 = "http://mh.99770.cc/";
if (!url_exists($url99770)) {
	$head .= "<div class=\"error message\"><a href=\"$url99770\">99770</a> is unreachable. At same time, we have no service, too. Sorry for that.</div>";
}
 */

$head .= "<div class=\"head\" style=\"text-align: right;\">";


$head .= '<nav role="navigation"><ul>';
/*
if  ($_SERVER["PHP_SELF"] == "/todo.php") {
	$head .= '<li><a href="99770.php">99770</a></li>';
	$head .= '<li class="active"><a href="todo.php">TODO</a></li>';
} else {
	$head .= '<li class="active"><a href="99770.php">99770</a></li>';
	$head .= '<li><a href="todo.php">TODO</a></li>';
}
*/
if ($user) {
	/*
	try {
		// Proceed knowing you have a logged in user who's authenticated.
		//$user_profile = $facebook->api('/me');
	} catch (FacebookApiException $e) {
		error_log($e);
		$user = null;
	}
	 */
	//$logoutUrl = $facebook->getLogoutUrl();
	//$head .= "<a href=\"".$user_profile['link']."\"><img src=\"https://graph.facebook.com/$user/picture\" style=\"height: 28px; width: 28px;\">".$user_profile['name']."</a>";
	$head .= '<li><a href="/"><img src="image/home.png"></a></li>';
	$head .= '<li><a href="logout.php"><img src="image/logout.png"></a></li>';
	$head .= "<li style=\"margin-top: -12px;\"><a id=\"profileUp\" href=\"#\"><img src=\"$g_pic\" style=\"height: 28px; width: 28px; \"></a>";
	//$head .= "<a href=\"".$user_profile['link']."\">".$user_profile['name']."</a>";
	daiSqliteUserEyesHeartsGet($g_type, $user, $eye, $heart);
	$head .= "<div style=\"position:absolute; top: 35px; right: 5px; font-size: 13px;\"><a href=\"99770.php?tag=myunreadeyes\" style=\"padding: 0px;\"><img src=\"image/eye.png\"></a> <a href=\"99770.php?tag=myeyes\" style=\"padding: 0px;\">$eye<img src=\"image/eye.png\"></a> <a href=\"99770.php?tag=mylikes\" style=\"padding: 0 3px;\">$heart<img src=\"image/myheart.png\"></a></div>";
	$head .= "</div></li>";
	/*
	$head .= "<script type=\"text/javascript\">";
	$head .= "$(function(){";
	$head .= "$('#profileUp').bubbletip($('#profile'), {";
	$head .= "	deltaDirection: 'down', bindShow: 'focus', bindHide: 'blur', offsetLeft: -10";
	$head .= "});";
	$head .= "});";
	$head .= "</script>";
	 */
} else {
	$head .= "<li><a href=\"$twitterLoginUrl\">Login with Twitter</a></li>";
	$head .= "<li><a href=\"$googleLoginUrl\">Login with Google</a></li>";
	$head .= "<li><a href=\"$facebookLoginUrl\">Login with Facebook</a></li>";
}
$head .= '</ul></nav>';
$head .= "</div>";
?>
