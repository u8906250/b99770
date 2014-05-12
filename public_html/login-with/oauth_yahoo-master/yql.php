<?php
require 'globals.php';
require 'oauth_helper.php';

require_once 'OAuth.php';
require_once 'JsonRpcClient.inc';

// Fill in the next 3 variables.
$querynum = 3;
//$access_token_secret='o2345w980945353478594867g3454l45lk324wrd';
$access_token = 'A=1OZT51HMh1mCLPNP1aBEBgpdm4CHv.zmVfKf4ppxPqwg0QAg80F1XlnbmqOxY2PX_U8OCCOIQt3GlPU1WtFfMaXbjFHfCiAwKOWQMBTf3Zgtqal8K7Md7sbm6iTEJ4M2krQYn1VIXsM5GRUo0XlyLpfWHT_IQMLNJXoC1HtaILkzW6N12eH1yThH6Zs.QsEnOq61geaSzmFUwacPot7cd16nUUpou3A28giBRK8PTdy.FQLAQlzEua2B3TaStnLfaTzvw4b627fwYNhlwieP2fLQvoEXSt6IIsjSFkbUZJo2GU7LZTs9TX_QvUB_54kWZNY9PZi4VR2BJYuzJHUl9hEIeTfgyxIqJUiAHyIOivWimuE7a9Acn2Jbhd1XyGNDXv9G4W11mGkg5TPIuZC7qxQFMv8n3U.cxUnFDMxgIAkkdM5ESFR6gf5X.2BD2Dk_KlGXmGfSmf55W894dRBRMbyiZDr6rjKRSKv9dRNl_XxbCFZAsvIvsw21BsymcXggLBT4KV_3emf9zmXNryUTu3ohU0zkEBFm9W32o2ocuix621.Qc7gZC2vs3vHMs00qUpEFCc.I92qfVZNmKgbij4OPLTlHIxte2fVEjKSr7egF28bvX7QV3jf6imm7v9laRH4WkwEF4tavTaHYkypeIg2Bf3xokKAA_KsVKlW1tKmByCrZ1BCBfupX6eazClyJXEDshHiv8N5gwS.bH7QnTvsQxNDPlOlDpE.E__hdV2ux6EOHPwP4.9fPhI1xAsm4rO6UR6SfbUo-';
$access_token_secret = '4929d06837c0025f7ee0cca429f286bf5d18db5c';

// Call YQL
//$retarr = call_yql(OAUTH_CONSUMER_KEY, OAUTH_CONSUMER_SECRET, $querynum, $access_token, $access_token_secret, false, true);

call_ymws (OAUTH_CONSUMER_KEY, OAUTH_CONSUMER_SECRET,$access_token, $access_token_secret);
exit(0);

function call_ymws($OAuthConsumerKey, $OAuthConsumerSecret, $oauth_token, $oauth_token_secret)
{
	$endPoint = 'http://tw.bid.yahooapis.com/v1/ws/fetch/idmapping?format=json';
	//$endPoint = 'http://mail.yahooapis.com/ws/mail/v1.1';
	$request = new OAuthRequest('POST', "$endPoint/jsonrpc", array(
				'oauth_nonce'=>mt_rand(),
				'oauth_timestamp'=>time(),
				'oauth_version'=>'1.0',
				'oauth_signature_method'=>'HMAC-SHA1',
				'oauth_consumer_key'=>$OAuthConsumerKey,
				'oauth_token'=>$oauth_token
				));
	$signature = new OAuthSignatureMethod_HMAC_SHA1();
	$request->sign_request($signature, new OAuthConsumer('', $OAuthConsumerSecret), new OAuthToken('', $oauth_token_secret));
	$oauthHeaderForJson = $request->to_header();
	$jsonClient = new JsonRpcClient("$endPoint/jsonrpc", NULL);
	$jsonClient->__setHeader(array('Content-Type: application/json','Accept: application/json', $oauthHeaderForJson));
	//$req = json_decode ('{"fid": "Inbox", "startMid": 0, "numMid": 10, "filterBy": {"isRead": 0}}');
	$req = json_decode ('{"fid": "Inbox", "startMid": 0, "numMid": 100}');
	$res = $jsonClient->ListMessages($req);

	var_dump($res);
}

/**
 * Call the Yahoo Contact API
 * @param string $consumer_key obtained when you registered your app
 * @param string $consumer_secret obtained when you registered your app
 * @param string $guid obtained from getacctok
 * @param string $access_token obtained from getacctok
 * @param string $access_token_secret obtained from getacctok
 * @param bool $usePost use HTTP POST instead of GET
 * @param bool $passOAuthInHeader pass the OAuth credentials in HTTP header
 * @return response string with token or empty array on error
 */
function call_yql($consumer_key, $consumer_secret, $querynum, $access_token, $access_token_secret, $usePost=false, $passOAuthInHeader=true)
{
  $retarr = array();  // return value
  $response = array();

  if ($querynum == 1) {
    $url = 'http://query.yahooapis.com/v1/yql';
    // Show my profile
    $params['q'] = 'select * from social.profile where guid=me';
  } elseif ($querynum == 2) {
    $url = 'http://query.yahooapis.com/v1/yql';
    // Find my friends
    $params['q'] = 'select * from social.connections where owner_guid=me';
  } else {
    // Since this information is public, use the non oauth endpoint 'public'
    //$url = 'http://query.yahooapis.com/v1/public/yql';
    $url = 'http://query.yahooapis.com/v1/yql';
    // Find all sushi restaurants in SF order by number of ratings desc
 //   $params['q'] = 'select Title,Address,Rating from local.search where query="sushi" and location="san francisco, ca"|sort(field="Rating.TotalRatings",descending="true")';
    $params['q'] = 'select message from ymail.msgcontent where (fid,mids) in (select folder.folderInfo.fid,mid from ymail.messages where messageInfo.flags.isRead=0)';
  }

  $params['format'] = 'json';
  $params['callback'] = 'cbfunc';
  $params['oauth_version'] = '1.0';
  $params['oauth_nonce'] = mt_rand();
  $params['oauth_timestamp'] = time();
  $params['oauth_consumer_key'] = $consumer_key;
  $params['oauth_token'] = $access_token;

  // compute hmac-sha1 signature and add it to the params list
  $params['oauth_signature_method'] = 'HMAC-SHA1';
  $params['oauth_signature'] =
      oauth_compute_hmac_sig($usePost? 'POST' : 'GET', $url, $params,
                             $consumer_secret, $access_token_secret);

  // Pass OAuth credentials in a separate header or in the query string
  if ($passOAuthInHeader) {
    $query_parameter_string = oauth_http_build_query($params, true);
    $header = build_oauth_header($params, "yahooapis.com");
    $headers[] = $header;
  } else {
    $query_parameter_string = oauth_http_build_query($params);
  }

  // POST or GET the request
  if ($usePost) {
    $request_url = $url;
    logit("call_yql:INFO:request_url:$request_url");
    logit("call_yql:INFO:post_body:$query_parameter_string");
    $headers[] = 'Content-Type: application/x-www-form-urlencoded';
    $response = do_post($request_url, $query_parameter_string, 80, $headers);
  } else {
    $request_url = $url . ($query_parameter_string ?
                           ('?' . $query_parameter_string) : '' );
    logit("call_yql:INFO:request_url:$request_url");
    $response = do_get($request_url, 80, $headers);
  }

  // extract successful response
  if (! empty($response)) {
    list($info, $header, $body) = $response;
    if ($body) {
      logit("call_yql:INFO:response:");
      print(json_pretty_print($body));
    }
    $retarr = $response;
  }

  return $retarr;
}
?>
