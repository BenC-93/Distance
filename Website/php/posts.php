<?php

	$posts = array(
		array(
			'title' => 'Networking and Bug Hunting',
			'link' => 'blog/devblog_1.html',
			'img' => 'assets/img/demo/portfolio/4.jpg',
			'date' => 'Mar 01 2015',
			'author' => 'Ben',
			'content' => 'This week the team spent some time getting to know Unreal’s networking system.',
			'type' => 'th-list'
			)
	);

	$postsJSON = json_encode($posts);

	echo $postsJSON;

?>