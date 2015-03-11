<?php

	$posts = array(
		array(
			'title' => 'This is a post title',
			'link' => 'blog/blog_post.html',
			'img' => 'assets/img/demo/portfolio/4.jpg',
			'date' => '21 Dec 2013',
			'author' => 'Jonathan',
			'content' => 'Lorem ipsum dolor sit amet, consectetur adipisicing elit. Blanditiis, nemo, itaque voluptate reprehenderit cumque ut iusto eligendi illo possimus enim tempora adipisci.',
			'type' => 'music'
			),
		array(
			'title' => 'This is a post title',
			'link' => 'blog/blog_post.html',
			'img' => 'assets/img/demo/portfolio/5.jpg',
			'date' => '21 Dec 2013',
			'author' => 'Jonathan',
			'content' => 'Lorem ipsum dolor sit amet, consectetur adipisicing elit. Suscipit, eum, dolorem, eveniet voluptas libero distinctio maiores officiis ad quia quae hic enim tenetur harum.',
			'type' => 'link'
			),
		array(
			'title' => 'This is a post title',
			'link' => 'blog/blog_post.html',
			'img' => 'assets/img/demo/portfolio/13.jpg',
			'date' => '21 Dec 2013',
			'author' => 'Jonathan',
			'content' => 'Lorem ipsum dolor sit amet, consectetur adipisicing elit. Ullam, corporis inventore ipsam suscipit fugiat laudantium ad voluptatibus recusandae quidem non.',
			'type' => 'picture'
			)
	);

	$postsJSON = json_encode($posts);

	echo $postsJSON;

?>