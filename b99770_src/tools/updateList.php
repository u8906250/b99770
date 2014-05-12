<?php
//TOP
$path = "/home/jy/better/data/top";
system ("/usr/local/bin/b99770 -n http://mh.99770.cc/top/ -s $path");
if (file_exists($path)) {
	chmod ($path, 0666);
}

//LIST
for ($i=1; $i<=25; $i++) {
	$path = "/home/jy/better/data/$i";
	$url = "http://mh.99770.cc/comiclist/$i/";
	system ("/usr/local/bin/b99770 -l $url -s $path");
	if (file_exists($path)) {
		chmod ($path, 0666);
	}
}

?>
