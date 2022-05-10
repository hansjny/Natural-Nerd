
//CONFIGURATION
var fb_app_key = "fpappkeyhere";
var fb_page_id = "fbpageidhere";
var youtube_channel_id = "youtubechannelid";
var google_api_key = "googleapikey";
var twitter_username = "twitterusername here";

var yt_sound = new Audio('notification.mp3');
var fb_sound = new Audio('fb.wav');
var tw_sound = new Audio('twitter.wav');

var fb = 0;
var yt = 0;
var tw = 0;
$(document).ready(function(){

    setInterval(function() {
	getFb(fb_page_id, fb_app_key, fb_sound);
	getYt(youtube_channel_id, google_api_key, yt_sound);
	getTw(twitter_username, tw_sound);
    }, 5000);
})


function getFb(id, key, sound) {
	var query = $.ajax({
	url: "https://graph.facebook.com/v2.8/naturalnerdiy/",
	method: "GET",
	data: { fields : "fan_count",
		access_token : id+"|"+key,
		format : "json",
		method : "get" },
	dataType: "json"
	});
	
	query.done(function(msg){
		if (msg.fan_count > fb) { 
			sound.play();
		} 
		$(".FB").html(msg.fan_count);
		fb = msg.fan_count;
	});
}

function getYt(channel_id, key, sound) {
	var query = $.ajax({ 
	url: "https://www.googleapis.com/youtube/v3/channels",
	method: "GET",
	data: { part: "statistics", 
		id : channel_id, 
		key : key},
	dataType: "json"});

	query.done(function(msg) {
		if (msg.items[0].statistics.subscriberCount > yt) { 
			sound.play();
		} 
		$(".YT").html(msg.items[0].statistics.subscriberCount);
		yt = msg.items[0].statistics.subscriberCount; 
	});
} 

function getTw(uname, sound) {
	var query = $.ajax({
	url: "http://cdn.syndication.twimg.com/widgets/followbutton/info.json",
	method: "GET",
	data: { screen_names: uname }, 
	dataType: "jsonp"});
	query.done(function(msg) {
		if (msg[0].followers_count > tw) { 
			sound.play();
		} 
		$(".TW").html(msg[0].followers_count);	
		tw = msg[0].followers_count; 
	});
}

