<?php
require 'globals.php';
require 'oauth_helper.php';

// Fill in the next 3 variables.
$old_access_token = 'A=1OZT51HMh1mCLPNP1aBEBgpdm4CHv.zmVfKf4ppxPqwg0QAg80F1XlnbmqOxY2PX_U8OCCOIQt3GlPU1WtFfMaXbjFHfCiAwKOWQMBTf3Zgtqal8K7Md7sbm6iTEJ4M2krQYn1VIXsM5GRUo0XlyLpfWHT_IQMLNJXoC1HtaILkzW6N12eH1yThH6Zs.QsEnOq61geaSzmFUwacPot7cd16nUUpou3A28giBRK8PTdy.FQLAQlzEua2B3TaStnLfaTzvw4b627fwYNhlwieP2fLQvoEXSt6IIsjSFkbUZJo2GU7LZTs9TX_QvUB_54kWZNY9PZi4VR2BJYuzJHUl9hEIeTfgyxIqJUiAHyIOivWimuE7a9Acn2Jbhd1XyGNDXv9G4W11mGkg5TPIuZC7qxQFMv8n3U.cxUnFDMxgIAkkdM5ESFR6gf5X.2BD2Dk_KlGXmGfSmf55W894dRBRMbyiZDr6rjKRSKv9dRNl_XxbCFZAsvIvsw21BsymcXggLBT4KV_3emf9zmXNryUTu3ohU0zkEBFm9W32o2ocuix621.Qc7gZC2vs3vHMs00qUpEFCc.I92qfVZNmKgbij4OPLTlHIxte2fVEjKSr7egF28bvX7QV3jf6imm7v9laRH4WkwEF4tavTaHYkypeIg2Bf3xokKAA_KsVKlW1tKmByCrZ1BCBfupX6eazClyJXEDshHiv8N5gwS.bH7QnTvsQxNDPlOlDpE.E__hdV2ux6EOHPwP4.9fPhI1xAsm4rO6UR6SfbUo-';
$old_token_secret = '4929d06837c0025f7ee0cca429f286bf5d18db5c';
$oauth_session_handle='AC37iFH1iQBpuE7rfI_Mav19jZh_j1jfQT7Vz1bYoFpZP74-';

// Refresh the access token using HTTP GET and HMAC-SHA1 signature
$retarr = refresh_access_token(OAUTH_CONSUMER_KEY, OAUTH_CONSUMER_SECRET,
                               $old_access_token, $old_token_secret,
                               $oauth_session_handle, false, true, true);
if (! empty($retarr)) {
  list($info, $headers, $body, $body_parsed) = $retarr;
  if ($info['http_code'] == 200 && !empty($body)) {
      print "Use oauth_token as the token for all of your API calls:\n" .
          rfc3986_decode($body_parsed['oauth_token']) . "\n";
  }
}

exit(0);

/**
 * Refresh an access token using an expired request token
 * @param string $consumer_key obtained when you registered your app
 * @param string $consumer_secret obtained when you registered your app
 * @param string $old_access_token obtained previously
 * @param string $old_token_secret obtained previously
 * @param string $oauth_session_handle obtained previously
 * @param bool $usePost use HTTP POST instead of GET (default false)
 * @param bool $useHmacSha1Sig use HMAC-SHA1 signature (default false)
 * @return response string with token or empty array on error
 */
function refresh_access_token($consumer_key, $consumer_secret, $old_access_token, $old_token_secret, $oauth_session_handle, $usePost=false, $useHmacSha1Sig=true, $passOAuthInHeader=true)
{
  $retarr = array();  // return value
  $response = array();

  $url = 'https://api.login.yahoo.com/oauth/v2/get_token';
  $params['oauth_version'] = '1.0';
  $params['oauth_nonce'] = mt_rand();
  $params['oauth_timestamp'] = time();
  $params['oauth_consumer_key'] = $consumer_key;
  $params['oauth_token'] = $old_access_token;
  $params['oauth_session_handle'] = $oauth_session_handle;

  // compute signature and add it to the params list
  if ($useHmacSha1Sig) {
    $params['oauth_signature_method'] = 'HMAC-SHA1';
    $params['oauth_signature'] =
      oauth_compute_hmac_sig($usePost? 'POST' : 'GET', $url, $params,
                             $consumer_secret, $old_token_secret);
  } else {
    $params['oauth_signature_method'] = 'PLAINTEXT';
    $params['oauth_signature'] =
      oauth_compute_plaintext_sig($consumer_secret, $old_token_secret);
  }

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
    logit("refacctok:INFO:request_url:$request_url");
    logit("refacctok:INFO:post_body:$query_parameter_string");
    $headers[] = 'Content-Type: application/x-www-form-urlencoded';
    $response = do_post($request_url, $query_parameter_string, 443, $headers);
  } else {
    $request_url = $url . ($query_parameter_string ?
                           ('?' . $query_parameter_string) : '' );
    logit("refacctok:INFO:request_url:$request_url");
    $response = do_get($request_url, 443, $headers);
  }

  // extract successful response
  if (! empty($response)) {
    list($info, $header, $body) = $response;
    $body_parsed = oauth_parse_str($body);
    if (! empty($body_parsed)) {
      logit("getacctok:INFO:response_body_parsed:");
      print_r($body_parsed);
    }
    $retarr = $response;
    $retarr[] = $body_parsed;
  }

  return $retarr;
}
?>
