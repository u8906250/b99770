<?php

	require("yos-social-php-master//lib/Yahoo.inc");    
	define('CONSUMER_KEY', "dj0yJmk9SjFaUFB4NjEyYWdVJmQ9WVdrOVpXMXNNVEkyTjJNbWNHbzlNVGd6TXpZMU56STJNZy0tJnM9Y29uc3VtZXJzZWNyZXQmeD01Mw--"); //你的 Consumer Key define('CONSUMER_SECRET', ""); //你的 Consumer Secret define('APPID', ""); //你的 Appid
	define('CONSUMER_SECRET', "28fc56deac3dbd0b152f8cf72e4fdfd21101da72"); //你的 Consumer Secret define('APPID', ""); //你的 Appid 
	define('APPID', "eml1267c"); //你的 Appid 

	$callback = "http://betterlib.com/login-with/test.php"; 
	$sessionStore = new CookieSessionStore();
	$session = YahooSession::requireSession(CONSUMER_KEY, CONSUMER_SECRET, APPID, $callback, $sessionStore); 
	if($session == NULL) {
		echo "require session fail";
		exit;
	} 
	$user = $session->getSessionedUser();
	$query = array("format" => "json");
	$response = $user->client->get("https://tw.bid.yahooapis.com/v1/ws/d/fetch/idmapping", $query); 
	$code = $response["code"]; 
	if($code !== 200) {
		echo "http error, dump response to check error";
		var_dump($response);
		exit;
	}
	$responseBody = $response["responseBody"];
	$ret = json_decode($responseBody);
	print_r ($ret);
	echo "auid:".$ret->{'result'}->{'data'}->{'auid'}; 
