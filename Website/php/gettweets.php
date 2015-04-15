<?php
session_start();
$path = realpath(__DIR__);
require_once("twitteroauth/twitteroauth.php"); //Path to twitteroauth library
 
$twitteruser = "distance_game";
$notweets = 30;
$consumerkey = "qMzJcU5QmmVEkVwynTJE4utTQ";
$consumersecret = "buQUhophlJOUuwTQt0mGiRrd9OTKMd96U03eqA50PwQQW6MdUz";
$accesstoken = "3061055118-ImWGESGjmDyDCvA0PtXvt7tRZRsYoRbyko3u7zB";
$accesstokensecret = "ecMyCxwKZ66aG2bzA7T0VXzDyySm9eBL6N161jpUuhhxf";
 
function getConnectionWithAccessToken($cons_key, $cons_secret, $oauth_token, $oauth_token_secret) {
  $connection = new TwitterOAuth($cons_key, $cons_secret, $oauth_token, $oauth_token_secret);
  return $connection;
}
 
$connection = getConnectionWithAccessToken($consumerkey, $consumersecret, $accesstoken, $accesstokensecret);
 
$tweets = $connection->get("https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=".$twitteruser."&count=".$notweets);
 
echo json_encode($tweets);
?>