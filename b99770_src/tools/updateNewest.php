<?php
$dataPath = "/home/jy/better/data";
$sqlitePath = "/home/jy/better/sqlite";
//newest
$newestPath = "$dataPath/newest";
if (!file_exists ($newestPath)) {
	system ("/usr/local/bin/b99770 -n http://mh.99770.cc/comicupdate/ -s $newestPath");
	chmod ($newestPath, 0666);
	return;
}

$oldlines = file ($newestPath);
$oldline="";
foreach ($oldlines as $line) {
	$line = trim($line);
	list($name, $url) = explode ("\t", $line);
	if (isset($url)) {
		$oldline = $line;
		break;
	}
}
system ("/usr/local/bin/b99770 -n http://mh.99770.cc/comicupdate/ -s $newestPath");
chmod ($newestPath, 0666);
$newlines = file ($newestPath);

$db = new PDO("sqlite:$sqlitePath/comic.sq3");
$rows = $db->query("SELECT id FROM comic");
$ids = array();
foreach ($rows as $row) {
	array_push ($ids, $row[0]);
}

foreach ($newlines as $line) {
	$line = trim($line);
	if ($line == $oldline)
		break;
	list($name, $url) = explode ("\t", $line);
	if (isset($url)) {
		list($a, $b, $c, $d, $num) = explode ("/", $url);
		$comicdir  = "$dataPath/$num";
		if (!file_exists($comicdir)) {
			mkdir ($comicdir);
			chmod ($comicdir, 0777);
		}
		$comicpath = "$comicdir/comic";
		system ("/usr/local/bin/b99770 -c $url/ -s $comicpath");
		chmod ($comicpath, 0666);
		if (in_array($num, $ids)) {
			$qry = $db->prepare('UPDATE comic SET name=? WHERE id=?');
			$qry->execute(array($name, $num));
		} else {
			$qry = $db->prepare('INSERT INTO comic (id, name) VALUES (?, ?)');
			$qry->execute(array($num, $name));
			array_push ($ids, $num);
		}
	}
}
/*
$lines = array_diff($newlines, $oldlines);
foreach ($lines as $line) {
	$line = trim($line);
	list($name, $url) = explode ("\t", $line);
	if (isset($url)) {
		list($a, $b, $c, $d, $num) = explode ("/", $url);
		$comicdir  = "$dataPath/$num";
		if (!file_exists($comicdir)) {
			mkdir ($comicdir);
			chmod ($comicdir, 0777);
		}
		$comicpath = "$comicdir/comic";
		system ("/usr/local/bin/b99770 -c $url/ -s $comicpath");
		chmod ($comicpath, 0666);
	}
}
 */

/*
$newestbookPath = "$dataPath/newestbook";
if (!file_exists ($newestbookPath)) {
	system ("/usr/local/bin/b99770 -n http://mh.99770.cc/comicnew/ -s $newestbookPath");
	chmod ($newestbookPath, 0666);
	return;
}

$oldlines = file ($newestbookPath);
system ("/usr/local/bin/b99770 -n http://mh.99770.cc/comicnew/ -s $newestbookPath");
chmod ($newestbookPath, 0666);
$newlines = file ($newestbookPath);
$lines = array_diff($newlines, $oldlines);

foreach ($lines as $line) {
	$line = trim($line);
	list($name, $url) = explode ("\t", $line);
	if (isset($url)) {
		list($a, $b, $c, $d, $num) = explode ("/", $url);
		$comicdir  = "$dataPath/$num";
		if (!file_exists($comicdir)) {
			mkdir ($comicdir);
			chmod ($comicdir, 0777);
		}
		$comicpath = "$comicdir/comic";
		system ("/usr/local/bin/b99770 -c $url/ -s $comicpath");
		chmod ($comicpath, 0666);
	}
}
?>
