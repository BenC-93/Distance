<?php
session_start();
$path = realpath(__DIR__);
require_once("twitteroauth/twitteroauth.php"); //Path to twitteroauth library
 
$twitteruser = "chaosgamer";
$notweets = 30;
$consumerkey = "0n7SERZ0ksxQitKe1UwmwRRBw";
$consumersecret = "VOLD26dvEsVwmFZbu3o42UGF2ZlbtKMTq4CBxpjU2MCIkgGCVS";
$accesstoken = "66573459-wexDUA7wbjTOaz9nF12RQZdnzWKhgE8ZAZtwjW7mm";
$accesstokensecret = "pzczBzx6xL2P5YQXsZYXDvAd35pmlU83nBceGaLC7MvHV";
 
function getConnectionWithAccessToken($cons_key, $cons_secret, $oauth_token, $oauth_token_secret) {
  $connection = new TwitterOAuth($cons_key, $cons_secret, $oauth_token, $oauth_token_secret);
  return $connection;
}
 
$connection = getConnectionWithAccessToken($consumerkey, $consumersecret, $accesstoken, $accesstokensecret);
 
$tweets = $connection->get("https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=".$twitteruser."&count=".$notweets);
 
echo json_encode($tweets);
?>