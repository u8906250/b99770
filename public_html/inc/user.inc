<?php
//facebook
require_once 'login-with/facebook-php-sdk/src/facebook.php';
//session_start();

$facebook = new Facebook(array(
	'appId'  => '472938966072801',
	'secret' => '475699be9615332bd170cf68556d27c6',
));
$user = $facebook->getUser();
$g_type = NULL;
if ($user) {
	$g_type = 0;
	$g_pic = "https://graph.facebook.com/$user/picture";
	$g_me = $facebook->api('/me');
	$g_first_name = $g_me['first_name'];
	$g_last_name = $g_me['last_name'];
	return;
} else {
	$facebookLoginUrl = $facebook->getLoginUrl();
}

//google
require_once 'login-with/google-api-php-client/src/Google_Client.php';
require_once 'login-with/google-api-php-client/src/contrib/Google_PlusService.php';
if (empty($_SESSION['status']) || $_SESSION['status'] != "verified") { //check if Twitter
	$google = new Google_Client();
	$google->setApplicationName("Better Libs");
	$google->setClientId('372560099539.apps.googleusercontent.com');
	$google->setClientSecret('3nyetCUOpYjOpETKFPMiSm3p');
	$google->setRedirectUri('http://betterlib.com');

	$plus = new Google_PlusService($google);

	if (isset($_GET['code'])) {
		$google->authenticate($_GET['code']);
		$_SESSION['access_token'] = $google->getAccessToken();
	}

	if (isset($_SESSION['access_token'])) {
		$google->setAccessToken($_SESSION['access_token']);
	}

	if ($google->getAccessToken()) {
		$me = $plus->people->get('me');
		$user = $me["id"];
		$g_type = 1;
		$g_pic = $me['image']['url'];
		return;
	} else {
		$googleLoginUrl = $google->createAuthUrl();
	}
}

//Twitter
require_once('login-with/twitteroauth-master/twitteroauth/twitteroauth.php');
require_once('login-with/twitteroauth-master/config.php');
if (empty($_SESSION['access_token']) || empty($_SESSION['access_token']['oauth_token']) || empty($_SESSION['access_token']['oauth_token_secret'])) {
	$twitterLoginUrl = "login-with/twitteroauth-master/redirect.php";
} else {
	if (!empty($_SESSION['status']) && $_SESSION['status'] == "verified") {
		$access_token = $_SESSION['access_token'];

		$connection = new TwitterOAuth(CONSUMER_KEY, CONSUMER_SECRET, $access_token['oauth_token'], $access_token['oauth_token_secret']);

		$content = $connection->get('account/verify_credentials');
		$user = $content->id;
		$g_type = 2;
		$g_pic = $content->profile_image_url;
	}
}

// handle openid/oauth


function getUserImgSrc ($type, $user)
{
	switch ($type) {
		case 0:
			return "https://graph.facebook.com/$user/picture";
		case 1:
			return "image/guest.jpg";
		case 2:
			return "image/guest.jpg";
		default:
			break;
	}
}
?>
