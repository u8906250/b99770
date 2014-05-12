<?php
function daiSqliteCreateTables ()
{
	$db = new PDO('sqlite:../sqlite/feedback.sq3');
	$db->exec("CREATE TABLE IF NOT EXISTS feedback (idx INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER DEFAULT 0 NOT NULL, id INTEGER, dt datetime default current_timestamp, todo TEXT)");
	$db = NULL;
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$db->exec("CREATE TABLE IF NOT EXISTS user (idx INTEGER PRIMARY KEY AUTOINCREMENT, dt datetime default current_timestamp, type INTEGER DEFAULT 0 NOT NULL, user INTEGER DEFAULT 0 NOT NULL, eye INTEGER DEFAULT 0 NOT NULL, heart INTEGER DEFAULT 0 NOT NULL, eyeComics TEXT DEFAULT '' NOT NULL, heartComics TEXT DEFAULT '' NOT NULL, subscription INTEGER DEFAULT 0 NOT NULL, subscriptionComics TEXT DEFAULT '' NOT NULL)");
	$db->exec("CREATE INDEX IF NOT EXISTS user_type_user_idx ON user (type, user)");

	$db->exec("CREATE TABLE IF NOT EXISTS better (idx INTEGER PRIMARY KEY AUTOINCREMENT, dt datetime default current_timestamp, useridx INTEGER DEFAULT 0 NOT NULL, eye INTEGER DEFAULT 0 NOT NULL, heart INTEGER DEFAULT 0 NOT NULL, title TEXT DEFAULT '' NOT NULL, url TEXT DEFAULT '' NOT NULL, icon TEXT DEFAULT '' NOT NULL, description TEXT DEFAULT '' NOT NULL, hide BOOLEAN DEFAULT 0 NOT NULL)");
	$db = NULL;
	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$db->exec("CREATE TABLE IF NOT EXISTS comic (idx INTEGER PRIMARY KEY AUTOINCREMENT, dt datetime default current_timestamp, id INTEGER DEFAULT 0 NOT NULL, eye INTEGER DEFAULT 0 NOT NULL, heart INTEGER DEFAULT 0 NOT NULL, subscription INTEGER DEFAULT 0 NOT NULL, name TEXT DEFAULT '' NOT NULL)");
	$db->exec("CREATE INDEX IF NOT EXISTS id_idx ON comic (id)");
	$db->exec("CREATE INDEX IF NOT EXISTS comic_name ON comic (name)");
	$db = NULL;
}

function daiSqliteAlterTables ()
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
//	$db->exec("ALTER TABLE user ADD bheart INTEGER DEFAULT 0 NOT NULL");
//	$db->exec("ALTER TABLE user ADD bheartlist TEXT DEFAULT '' NOT NULL");
//	$db->exec("DROP TABLE better");
//	$db->exec("ALTER TABLE better ADD hide BOOLEAN DEFAULT 0 NOT NULL");

	$qry = $db->prepare('UPDATE better SET url=? , icon=? WHERE idx=? ');
	$qry->execute(array("99770.php", "99770.png", 1));
//	$qry = $db->prepare('UPDATE better SET hide=? WHERE idx=?');
//	$qry->execute(array(true, '2'));
	$db = NULL;
	//$db->exec("CREATE INDEX IF NOT EXISTS comic_name ON comic (name)");
	//$db = new PDO('sqlite:../sqlite/comic.sq3');
	//$db->exec("ALTER TABLE comic ADD name TEXT DEFAULT '' NOT NULL");
	//$db->exec("CREATE INDEX IF NOT EXISTS comic_name ON comic (name)");
	//$db = NULL;
}

//daiSqliteAlterTables();
//daiSqliteCreateTables();

function daiSqliteUserAdd ($g_type, $user)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$qry = $db->prepare('INSERT INTO user (type, user) VALUES (?, ?)');
	$qry->execute(array($g_type, $user));
	$db = NULL;
}

function daiSqliteBetterSiteDel ($g_type, $user, $idx)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT idx FROM user WHERE type=$g_type AND user=$user");
	foreach ($rows as $row) {
		$useridx = $row[0];
		$rows = $db->query("SELECT icon FROM better WHERE idx=$idx AND useridx=$useridx");
		foreach ($rows as $row) {
			$iconname = $row[0];
			unlink ("image/site/$iconname");
			$qry = $db->prepare('DELETE FROM better WHERE idx=? AND useridx=?');
			$qry->execute(array($idx, $useridx));
			break;
		}
		break;
	}
	$db = NULL;
	return true;
}

//better
function daiSqliteBetterSiteSet ($g_type, $user, $title, $url, $description, $icon)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT idx FROM user WHERE type=$g_type AND user=$user");
	$user_exists=false;
	foreach ($rows as $row) {
		$user_exists = true;
		$useridx = $row[0];
		break;
	}
	if (!$user_exists) {
		daiSqliteUserAdd ($g_type, $user);
		$rows = $db->query("SELECT idx FROM user WHERE type=$g_type AND user=$user");
		foreach ($rows as $row) {
			$useridx = $row[0];
			break;
		}
	}
	if (isset($useridx)) {
		$qry = $db->prepare('INSERT INTO better (useridx, title, url, description, icon) VALUES (?, ?, ?, ?, ?)');
		if (strncasecmp($url, "http://", 7) && strncasecmp($url, "https://", 8)) {
			if ($useridx == 1) {
				$qry->execute(array($useridx, $title, $url, $description, $icon));
			} else {
				$qry->execute(array($useridx, $title, "http://".$url, $description, $icon));
			}
		} else {
			$qry->execute(array($useridx, $title, $url, $description, $icon));
		}
	}
	$db = NULL;
}
//daiSqliteBetterInsert (0, "Better 99770", "99770.php", "99770.png");
//daiSqliteBetterInsert (0, "Turn Off the Light", "offlight.php", "offlight.png");

function daiSqliteBetterGet ($g_type, $user)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	if ($user) {
		$rows = $db->query("SELECT idx FROM user WHERE type=$g_type AND user=$user");
		foreach ($rows as $row) {
			$useridx = $row[0];
			break;
		}
		$sth = $db->prepare("SELECT * FROM better WHERE useridx=?");
		$sth->execute(array($useridx));
	} else {
		$sth = $db->prepare("SELECT * FROM better");
		$sth->execute();
	}
	return $sth->fetchAll();
}

function daiSqliteBetterEyeSet ($g_type, $user, $betteridx, $eye)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$qry = $db->prepare('UPDATE better SET eye=eye+? WHERE idx=?');
	$qry->execute(array($eye, $betteridx));
	return true;
}

//feedback:feedback
function daiSqliteFeedbackInsert($g_type, $user, $datetime, $text) {
	$db = new PDO('sqlite:../sqlite/feedback.sq3');
	if ($user) {
		$qry = $db->prepare('INSERT INTO feedback (type, id, dt, todo) VALUES (?, ?, ?, ?)');
		$qry->execute(array($g_type, $user, $datetime, $text));
	} else {
		$qry = $db->prepare('INSERT INTO feedback (id, dt, todo) VALUES (?, ?, ?)');
		$qry->execute(array($user, $datetime, $text));
	}
	$idx = $db->lastInsertId();
	$db = NULL;
	return $idx;
}

//comic
function daiSqliteComicSearch($keyword)
{
	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$qry = $db->prepare('SELECT id, name FROM comic where name like ?');
	$qry->execute(array("%$keyword%"));
	return $qry->fetchAll();
}

function daiSqliteComicImport()
{
	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$rows = $db->query("SELECT id FROM comic");
	$ids = array();
	foreach ($rows as $row) {
		array_push ($ids, $row[0]);
	}
	for ($i=23; $i<=25; $i++) {
		if (!file_exists("data/$i"))
			continue;
		$lines = file ("data/$i");
		foreach ($lines as $line) {
			$line = trim($line);
			list($name, $url) = explode ("\t", $line);
			if (isset($url)) {
				list($a, $b, $c, $d, $num) = explode ("/", $url);
				if (in_array($num, $ids)) {
					//$qry = $db->prepare('UPDATE comic SET name=? WHERE id=?');
					//$qry->execute(array($name, $num));
				} else {
					$qry = $db->prepare('INSERT INTO comic (id, name) VALUES (?, ?)');
					$qry->execute(array($num, $name));
					array_push ($ids, $num);
				}
			}
		}
	}
	/*
	$lines = file ("data/tmp");
	foreach ($lines as $line) {
		$line = trim($line);
		list($name, $url) = explode ("\t", $line);
		if (isset($url)) {
			list($a, $b, $c, $d, $num) = explode ("/", $url);
			if (in_array($num, $ids)) {
				//$qry = $db->prepare('UPDATE comic SET name=? WHERE id=?');
				//$qry->execute(array($name, $num));
			} else {
				$qry = $db->prepare('INSERT INTO comic (id, name) VALUES (?, ?)');
				$qry->execute(array($num, $name));
				array_push ($ids, $num);
			}
		}
	}
	*/
}

function daiSqliteFeedbackDump() {
	global $g_pic;
	date_default_timezone_set('Asia/Taipei');
	$db = new PDO('sqlite:../sqlite/feedback.sq3');
	$rows = $db->query('SELECT idx, type, id, datetime(dt, \'localtime\'), todo from feedback');
	foreach ($rows as $row) {
		if ($row[2] == 0)
			$who = "<img src=\"image/guest.jpg\" title=\"Guest\">";
		else
			$who = "<img src=\"".getUserImgSrc($row[1], $row[2])."\" title=\"$row[2]\">";
		$text = str_replace ("\n", "<br/>", trim($row[4]));
		echo "<li>$who<span style=\"font-size: 10px; position: absolute; right: 5px; opacity: 0.5;\">$row[3]</span><span style=\"\">&nbsp;&nbsp;$text</span></li>";
	}
	$db = NULL;
}
//user::eye
function daiSqliteEyeSet ($g_type, $user, $comic, $eye)
{
	//daiSqliteCreateTables();
	//daiSqliteAlterTables();
	//daiSqliteComicImport();
	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$rows = $db->query("SELECT idx FROM comic WHERE id=$comic");
	$isin = false;
	foreach ($rows as $row) {
		$qry = $db->prepare('UPDATE comic SET eye=eye+? WHERE idx=?');
		$qry->execute(array($eye, $row[0]));
		$isin = true;
		break;
	}
	if (!$isin) {
		$qry = $db->prepare('INSERT INTO comic (id, eye) VALUES (?, ?)');
		$qry->execute(array($comic, $eye));
	}
	$db = NULL;

	if ($user) {
		$db = new PDO('sqlite:../sqlite/user.sq3');
		$rows = $db->query("SELECT idx, eyeComics FROM user WHERE type=$g_type AND user=$user");
		$isin = false;
		foreach ($rows as $row) {
			$eyeComics = explode (",", $row[1]);
			if (in_array($comic, $eyeComics)) {
				//$qry = $db->prepare('UPDATE user SET eye=? WHERE idx=?');
				//$qry->execute(array($eyeNum, $row[0]));
			} else {
				$qry = $db->prepare('UPDATE user SET eye=eye+?, eyeComics=? WHERE idx=?');
				$qry->execute(array($eye, strlen($row[1]) <= 0? $comic: $row[1].",$comic", $row[0]));
			}
			$isin = true;
			break;
		}
		if (!$isin) {
			$qry = $db->prepare('INSERT INTO user (type, user, eye, eyeComics) VALUES (?, ?, ?, ?)');
			$qry->execute(array($g_type, $user, $eye, $comic));
		}
		$db = NULL;
	}
}

function daiSqliteEyeRemove ($g_type, $user, $comic)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT idx, eyeComics FROM user WHERE type=$g_type AND user=$user");
	foreach ($rows as $row) {
		$eyeComics = explode (",", $row[1]);
		if (in_array($comic, $eyeComics)) {
			foreach ($eyeComics as $key => $eyeComic) {
				if ($comic == $eyeComic) {
					unset ($eyeComics[$key]);
					break;
				}
			}
			if (count($eyeComics) > 0) {
				$str = implode (",", $eyeComics);
				$qry = $db->prepare('UPDATE user SET eye=eye-1, eyeComics=? WHERE idx=?');
				$qry->execute(array($str, $row[0]));
			} else {
				$qry = $db->prepare('UPDATE user SET eye=?, eyeComics=? WHERE idx=?');
				$qry->execute(array(0, '', $row[0]));
			}
			$issub = true;
		}
		break;
	}
	$db = NULL;

	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$rows = $db->query("SELECT idx,eye FROM comic WHERE id=$comic");
	foreach ($rows as $row) {
		$qry = $db->prepare('UPDATE comic SET eye=eye-1 WHERE idx=?');
		$qry->execute(array($row[0]));
		break;
	}
	$db = NULL;
	return array ();
}

function daiSqliteUserEyesHeartsGet($g_type, $user, &$eye, &$heart)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT eye, heart FROM user WHERE type=$g_type AND user=$user");
	$eye=0;
	$heart=0;
	foreach ($rows as $row) {
		if ($row[0])
			$eye = $row[0];
		if ($row[1])
			$heart = $row[1];
		break;
	}
}

function daiSqliteUserEyeComicsGet($g_type, $user, &$eyeComics)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT eyeComics FROM user WHERE type=$g_type AND user=$user");
	$eyeComics='';
	foreach ($rows as $row) {
		if ($row[0])
			$eyeComics = $row[0];
		break;
	}
}

function daiSqliteUserHeartComicsGet($g_type, $user, &$heartComics)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT heartComics FROM user WHERE type=$g_type AND user=$user");
	$heartComics='';
	foreach ($rows as $row) {
		if ($row[0])
			$heartComics = $row[0];
		break;
	}
}

function daiSqliteComicHaveEyeGet ()
{
	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$rows = $db->query("SELECT id, eye FROM comic WHERE eye > 0 order by eye desc");
	$ret = array();
	foreach ($rows as $row) {
		$ret[$row[0]]= $row[1];
	}
	return $ret;
}

//user::heart
function daiSqliteHeartSet ($g_type, $user, $comic, $heart)
{
	$db = new PDO('sqlite:../sqlite/user.sq3');
	$rows = $db->query("SELECT idx, heartComics FROM user WHERE type=$g_type AND user=$user");
	$isin = false;
	$issub = false;
	foreach ($rows as $row) {
		$heartComics = explode (",", $row[1]);
		if (in_array($comic, $heartComics)) {
			foreach ($heartComics as $key => $heartComic) {
				if ($comic == $heartComic) {
					unset ($heartComics[$key]);
					break;
				}
			}
			if (count($heartComics) > 0) {
				$str = implode (",", $heartComics);
				$qry = $db->prepare('UPDATE user SET heart=heart-?, heartComics=? WHERE idx=?');
				$qry->execute(array($heart, $str, $row[0]));
			} else {
				$qry = $db->prepare('UPDATE user SET heart=?, heartComics=? WHERE idx=?');
				$qry->execute(array(0, '', $row[0]));
			}
			$issub = true;
		} else {
			$qry = $db->prepare('UPDATE user SET heart=heart+?, heartComics=? WHERE idx=?');
			$qry->execute(array($heart, strlen($row[1]) <= 0? $comic: $row[1].",$comic", $row[0]));
		}
		$isin = true;
		break;
	}
	if (!$isin) {
		$qry = $db->prepare('INSERT INTO user (type, user, heart, heartComics) VALUES (?, ?, ?, ?)');
		$qry->execute(array($g_type, $user, $heart, $comic));
	}
	$db = NULL;

	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$rows = $db->query("SELECT idx,heart FROM comic WHERE id=$comic");
	$isin = false;
	$heartNum = 0;
	foreach ($rows as $row) {
		if ($issub) {
			$qry = $db->prepare('UPDATE comic SET heart=heart-? WHERE idx=?');
			$heartNum = $row[1]-$heart;
		} else {
			$qry = $db->prepare('UPDATE comic SET heart=heart+? WHERE idx=?');
			$heartNum = $row[1]+$heart;
		}
		$qry->execute(array($heart, $row[0]));
		$isin = true;
		break;
	}
	if (!$isin) {
		$qry = $db->prepare('INSERT INTO comic (id, heart) VALUES (?, ?)');
		$qry->execute(array($comic, $heart));
		$heartNum = $heart;
	}
	$db = NULL;
	return array ($heartNum, !$issub);
}

function daiSqliteComicHaveHeartGet ()
{
	$db = new PDO('sqlite:../sqlite/comic.sq3');
	$rows = $db->query("SELECT id, heart FROM comic WHERE heart > 0 order by eye desc");
	$ret = array();
	foreach ($rows as $row) {
		$ret[$row[0]]= $row[1];
	}
	return $ret;
}
?>
