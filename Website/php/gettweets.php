<?php
session_start();
$path = realpath(__DIR__);
require_once("twitteroauth/twitteroauth.php"); //Path to twitteroauth library
 
$twitteruser = "bootstraphunter";
$notweets = 30;
$consumerkey = "9gcrrLZKHFazQBLYwHf7wg";
$consumersecret = "FSefIUW55KakOwwcREplmoIJ4VJXpXyiNJkwAoNecc";
$accesstoken = "126665625-g8eS0VSsLj2gIRkw6y7R8EYpwl4Mu6GCtpdG1ER3";
$accesstokensecret = "e2zq5ahN35q0pX3eLGXUYl2H5JaVzAC6DtB1fkL9TYEag";
 
function getConnectionWithAccessToken($cons_key, $cons_secret, $oauth_token, $oauth_token_secret) {
  $connection = new TwitterOAuth($cons_key, $cons_secret, $oauth_token, $oauth_token_secret);
  return $connection;
}
 
$connection = getConnectionWithAccessToken($consumerkey, $consumersecret, $accesstoken, $accesstokensecret);
 
$tweets = $connection->get("https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=".$twitteruser."&count=".$notweets);
 
echo json_encode($tweets);
?>