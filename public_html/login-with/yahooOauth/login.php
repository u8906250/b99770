<?php

require_once ('common.inc.php'); // contails API keys

$oauthapp = new YahooOAuthApplication(OAUTH_CONSUMER_KEY, OAUTH_CONSUMER_SECRET,
    OAUTH_APP_ID, OAUTH_DOMAIN);
//Create an object by passing your consumer key, consumer secret and application ID  as parameters.
//This class contains methods to handle oauth

if (isset($_GET['openid_mode']) && $_GET['openid_mode'] == 'id_res') {
    $request_token = new YahooOAuthRequestToken($_GET['openid_oauth_request_token'],
        '');
    $_SESSION['yahoo_oauth_request_token'] = $request_token->to_string();

    // exchange request token for access token
    $oauthapp->token = $oauthapp->getAccessToken($request_token);

    // store access token for later use
    $_SESSION['yahoo_oauth_access_token'] = $oauthapp->token->to_string();
}

if (isset($_GET['action'])) {

    if ($_GET['action'] == 'login') {
        header('Location: ' . $oauthapp->getOpenIDUrl($oauthapp->callback_url)); //go to Yahoo for authentification and authorization
        exit;
    }

} else {

    if (isset($_SESSION['yahoo_oauth_access_token'])) {
        // if session is still present ( not expired),then restore access token from session
        $oauthapp->token = YahooOAuthAccessToken::from_string($_SESSION['yahoo_oauth_access_token']);
    }
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
<meta http-equiv="X-XRDS-Location" content="xrds.xml"/>
<title>Yahoo! OAuth Login Demo - Youhack.me</title>

<link rel="stylesheet" type="text/css" href="http://yui.yahooapis.com/combo?3.0.0b1/build/cssreset/reset-min.css&3.0.0b1/build/cssfonts/fonts-min.css&3.0.0b1/build/cssgrids/grids-min.css&3.0.0b1/build/cssbase/base-min.css">

<link rel="stylesheet" type="text/css" href="css/login.css"/>

</head>
<body>
<?php if (!isset($_GET['action']) && !isset($_SESSION['yahoo_oauth_access_token'])): // display login button if there's no previous session?>
<header>
  <h1>Yahoo Oauth Login</h1>
  <h2><a href="http://youhack.me/2011/06/15/integrate-login-system-with-yahoo-connect-using-oauth/">« Back to Youhack</a></h2>
</header>

<div id="ylogin"><a href="login.php?action=login"><img class="login" src="img/loginY.png" width="150" height="30" alt="Yahoo Oauth Login"/></a></div>
<?php else:

    header('Location: content.php');
    exit;
?>



<?php

    // read yahoo user profile and fetch email add , gender , name etc
    $profile = $oauthapp->getProfile();

?>

<div id="profile" class="yui-b">



<?php endif; ?>



</body>
</html>