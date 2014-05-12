<?php
session_start();
if (!file_exists("/tmp/".session_id())) {
	mkdir ("/tmp/".session_id());
}
include_once ("inc/user.inc");
include_once ("inc/daiSqlite.inc");
if(isset($_POST["url"]) && isset($_POST["type"])){
	switch ($_POST["type"]) {
		case "vol":
			list($a, $b, $c, $d, $num, $vol) = explode ("/", $_POST["url"]);
			$volpath = "data/$num/$vol";
			if (!file_exists($volpath)) {
				system ("../tools/b99770 -v ".$_POST["url"]." -s $volpath");
				chmod ($volpath, 0666);
			}
			$lines = file ($volpath);
			$linesArray = array();
			if (isset($_POST["ism"])) {
				$i=1;
				foreach ($lines as &$line) {
					$linesArray [] = array("url"=>trim($line),"caption"=>$i++);
				}
			} else {
				foreach ($lines as &$line) {
					$linesArray [] = trim($line);
				}
			}
			$newviewedpath = $user? "data/$num/newviewed.$g_type.$user": "data/$num/newviewed";
			if (!file_exists($newviewedpath)) {
				file_put_contents ($newviewedpath, $vol);
			} else {
				$newviewed = file_get_contents ($newviewedpath);
				if ($newviewed < $vol) {
					file_put_contents ($newviewedpath, $vol);
				}
			}
			daiSqliteEyeSet ($g_type, $user, $num, 1);
			echo json_encode($linesArray);
			return;
			break;
		default:
			break;
	}
}

if (isset($_POST['action'])) {
	switch($_POST['action']){
	case "feedback":
		$text = htmlentities($_POST['text']);
		date_default_timezone_set('UTC');
		$ts = time();
		$datetime = date("Y-m-d H:i:s", $ts);
		$idx = daiSqliteFeedbackInsert ($g_type, $user, $datetime, $text);
		$text = str_replace ("\n", "<br/>", trim($text));
		date_default_timezone_set('Asia/Taipei');
		//date_default_timezone_set('America/Denver');
		$datetime = date("Y-m-d H:i:s", $ts);
		echo json_encode(array($idx, $user, $text, $datetime));
		return;
	case "getPage":
		$list = "";
		list($a, $b, $c, $d, $num) = explode ("/", $_POST["url"]);
		$comicdir  = "data/$num";
		if (!file_exists($comicdir)) {
			mkdir ($comicdir);
			chmod ($comicdir, 0777);
		}
		$comicpath = "$comicdir/comic";
		if (!file_exists($comicpath)) {
			system ("../tools/b99770 -c ".$_POST["url"]." -s $comicpath");
			chmod ($comicpath, 0666);
		}
		if (isset($_POST["name"]) && !file_exists("$comicdir/name")) {
			file_put_contents ("$comicdir/name", $_POST["name"]);
		}
		if (!file_exists("$comicdir/url")) {
			file_put_contents ("$comicdir/url", $_POST["url"]);
		}
		$newviewedpath = $user? "data/$num/newviewed.$g_type.$user": "data/$num/newviewed";
		if (file_exists($newviewedpath)) {
			$newviewed = file_get_contents ($newviewedpath);
		}
		$lines = file ($comicpath);
		$list .= "<ul>";
		foreach ($lines as $line) {
			$line = trim($line);
			list($name, $url) = explode ("\t", $line);
			$addstyle = "";
			if (isset($newviewed)) {
				list($a, $b, $c, $d, $num, $vol) = explode ("/", $url);
				if ($vol == $newviewed) {
					$addstyle = "color: red;";
				}
			}
			$list .= "<li style=\"text-align:left;\"><a class=\"inline\" href=\"#\" data-options='{\"url\":\"$url\",\"type\":\"vol\"}' style='display:inline-block;$addstyle'>$name</a><span class=\"page\"></span></li>";
		}
		$list .= "</ul>";
		if($list != ""){
			echo json_encode(array(true,$list));
		}else{
			echo json_encode(array(false));
		}
		break;
	case "heartAdd":
		if ($user) {
			$ret = daiSqliteHeartSet ($g_type, $user, $_POST["num"], 1);
			echo json_encode($ret);
		}
		break;
	case "eyeRemove":
		if ($user) {
			$ret = daiSqliteEyeRemove ($g_type, $user, $_POST["num"]);
			unlink ($user? "data/".$_POST["num"]."/newviewed.$g_type.$user": "data/".$_POST["num"]."/newviewed");
			echo json_encode($ret);
		}
		break;
	case "stepRemove":
		unlink ($user? "data/".$_POST["num"]."/newviewed.$g_type.$user": "data/".$_POST["num"]."/newviewed");
		echo json_encode($ret);
		break;
	case "betterEyeAdd":
		$ret = daiSqliteBetterEyeSet ($g_type, $user, $_POST["betteridx"], 1);
		echo json_encode($ret);
		break;
	case "betterSiteAdd":
		if ($user) {
			$iconname="";
			if (isset($_POST["icon"]) && strlen($_POST["icon"]) > 0 && file_exists("/tmp/".session_id()."/".$_POST["icon"])) { //FIXME multiple icons
				$iconname = $user."_icon_".rand();
				rename ("/tmp/".session_id()."/".$_POST["icon"], "image/site/$iconname");
			}

			$ret = daiSqliteBetterSiteSet ($g_type, $user, $_POST["title"], $_POST["url"], $_POST["description"], $iconname);
			echo json_encode($ret);
		}
		break;
	case "betterSiteDel":
		if ($user) {
			$ret = daiSqliteBetterSiteDel ($g_type, $user, $_POST["betteridx"]);
			echo json_encode($ret);
		}
		break;
	default:
		break;
	}
}
if (isset($_POST['search'])) {
	$rows = daiSqliteComicSearch ($_POST['search']);
	$searchfile = "/tmp/".session_id()."/search";

	$fp = fopen($searchfile, "w");
	$i=0;
	$page=1;
	foreach ($rows as $row) {
		if ($i%20 == 0)
			fwrite ($fp, ($page++)."\r\n");
		$num = $row[0];
		$name = $row[1];
		fwrite ($fp, "$name\thttp://mh.99770.cc/comic/$num\r\n");
		$i++;
	}
	fclose ($fp);
}
return;
?>
