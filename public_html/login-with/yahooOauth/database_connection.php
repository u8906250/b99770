<?php
/*Define constant to connect to database */
DEFINE('DATABASE_USER', ''); // enter your database username
DEFINE('DATABASE_PASSWORD', '');//database password
DEFINE('DATABASE_HOST', 'localhost');
DEFINE('DATABASE_NAME', ''); // enter database name

// Make the connection:
$dbc = @mysqli_connect(DATABASE_HOST, DATABASE_USER, DATABASE_PASSWORD,
    DATABASE_NAME);

if (!$dbc) {
    trigger_error('Could not connect to MySQL: ' . mysqli_connect_error());
}

?>