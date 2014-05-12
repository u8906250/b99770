<?php
session_start();
if (file_exists("/tmp/".session_id())) {
	system ("rm -rf /tmp/".session_id());
}
session_destroy();
header("Location: index.php");
?>
