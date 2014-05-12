<?php
session_start();
if (!file_exists("/tmp/".session_id())) {
	mkdir ("/tmp/".session_id());
}

foreach ($_FILES["images"]["error"] as $key => $error) {
    if ($error == UPLOAD_ERR_OK) {
        $name = $_FILES["images"]["name"][$key];
        move_uploaded_file( $_FILES["images"]["tmp_name"][$key], "/tmp/".session_id()."/". $_FILES['images']['name'][$key]);
    }
}

