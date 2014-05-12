<?php


require_once ('common.inc.php'); // contails API keys
require_once ('database_connection.php');// contains database connection details

$oauthapp = new YahooOAuthApplication(OAUTH_CONSUMER_KEY, OAUTH_CONSUMER_SECRET,
    OAUTH_APP_ID, OAUTH_DOMAIN);
//Create an object by passing your consumer key, consumer secret and application ID  as parameters.
//This class contains methods to

// handle openid/oauth

if (isset($_GET['openid_mode']) && $_GET['openid_mode'] == 'id_res') {
    $request_token = new YahooOAuthRequestToken($_GET['openid_oauth_request_token'],
        '');
    $_SESSION['yahoo_oauth_request_token'] = $request_token->to_string();

    // exchange request token for access token
    $oauthapp->token = $oauthapp->getAccessToken($request_token);

    // store access token for later use
    $_SESSION['yahoo_oauth_access_token'] = $oauthapp->token->to_string();
}

if (isset($_GET['action'])) {

    if ($_GET['action'] == 'logout') {

        unset($_SESSION['yahoo_oauth_access_token']);
        unset($_GET['action']);
        unset($_SESSION['yahoo']);
        header('Location: ' . $oauthapp->callback_url); // go back to login.php

        exit;
    }

} else {

    if (isset($_SESSION['yahoo_oauth_access_token'])) {
        // if session is still present ( not expired),then restore access token from session
        $oauthapp->token = YahooOAuthAccessToken::from_string($_SESSION['yahoo_oauth_access_token']);
    }
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8"/>

<title>Yahoo! OAuth Login Demo - Youhack.me</title>
<link rel="stylesheet" href="css/styles.css" />


</head>

<body>
<?php if (!isset($_GET['action']) && !isset($_SESSION['yahoo_oauth_access_token'])) {
    header('Location: login.php'); // IF user not authenticated or if user try to access this page directly from browser ,redirect to login page
}
?>



<header>
  <h1>Yahoo Oauth Login</h1>
  <h2><a href="http://youhack.me/2011/06/15/integrate-login-system-with-yahoo-connect-using-oauth/"> « Back to Youhack</a></h2>
</header>

<?php if (isset($_SESSION['yahoo_oauth_request_token'])) {
    $profile = $oauthapp->getProfile(); // fetch profile information
    if ($profile) {
        if (isset($profile->profile->emails->handle)) {
            $Email = $profile->profile->emails->handle;
        } // get Email address of user

        if (isset($profile->profile->profileUrl)) {
            $profile_url = $profile->profile->profileUrl;
        } // profile url

        if (isset($profile->profile->guid)) {
            $Guid = $profile->profile->guid; // Globally Unique Identifier
        }

        if (isset($profile->profile->birthdate)) {
            $birthdate = $profile->profile->birthdate; // Birthdate of user
        }
        if (isset($profile->profile->created)) {
            $date_created = $profile->profile->created; //Date of profile created
        }
        if (isset($profile->profile->emails->handle)) {
            $Email = $profile->profile->emails->handle; //Email Address
        }
        if (isset($profile->profile->familyName)) {
            $surname = $profile->profile->familyName; //Surname
        }
        if (isset($profile->profile->gender)) {
            $gender = $profile->profile->gender; // gender
        }
        if (isset($profile->profile->givenName)) {
            $first_name = $profile_url = $profile->profile->givenName; //First Name
        }
        if (isset($profile->profile->image->imageUrl)) {
            $image_url = $profile_url = $profile->profile->image->imageUrl; //Display picture url
        }
        if (isset($profile->profile->nickname)) {
            $nickname = $profile->profile->nickname; // nickname/username
        }

    } else {
        header('Location: login.php');
    }


    $q_user_exist = "SELECT * FROM users  WHERE (Email='$Email' AND guid='$Guid')"; //Check whether user has previously login to our application


    $r_user_exist = mysqli_query($dbc, $q_user_exist);
    if (!$r_user_exist) { //If the QUery Failed
        echo 'Something went wrong with this demo. ';
        exit();
    }

    if (@mysqli_num_rows($r_user_exist) == 0)
        //if user account is not present , record info in our database
        {

        $q_add_user = "INSERT INTO `users` ( `Guid`, `username`, `Full Name`, `Email`) VALUES ('$Guid', '$nickname', '$first_name.$surname', '$Email')";
        $r_add_user = mysqli_query($dbc, $q_add_user);
        if (!$r_add_user) { //in case could not insert record
            echo $q_add_user;
            echo ' User info could not be saved in database' . mysqli_error($dbc);

            exit();
        }
    }
    // Assign member info to a multi-dimentional array
    $_SESSION['yahoo'] = array();
    if (isset($profile_url)) {
        $_SESSION['yahoo']['profileUrl'] = $profile_url;
    }

    if (isset($Guid)) {
        $_SESSION['yahoo']['guid'] = $Guid;

    }
    if (isset($birthdate)) {
        $_SESSION['yahoo']['birthdate'] = $birthdate;
    }

    if (isset($date_created)) {
        $_SESSION['yahoo']['created'] = $date_created;
    }

    if (isset($Email)) {
        $_SESSION['yahoo']['handle'] = $Email;
    }

    if (isset($surname)) {
        $_SESSION['yahoo']['familyName'] = $surname;
    }

    if (isset($gender)) {
        $_SESSION['yahoo']['gender'] = $gender;
    }


    if (isset($first_name)) {
        $_SESSION['yahoo']['givenName'] = $first_name;
    }


    if (isset($image_url)) {
        $_SESSION['yahoo']['imageUrl'] = $image_url;
    }

    if (isset($nickname)) {
        $_SESSION['yahoo']['nickname'] = $nickname;
    }


} else {
    header('Location: login.php');
}
?>
<section id="container">
  <div style="padding-left:125px;padding-top:15px">
    <table width="438" border="0">
      <tbody>
         <tr>
      <td height="12"><?php

?>Welcome , <strong><?php if (isset($_SESSION['yahoo']['nickname'])) {
    echo $_SESSION['yahoo']['nickname'];
} ?></strong> </td>
          
        </tr>
        <tr>
          <td><strong>GUD :</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['guid'])) {
    echo $_SESSION['yahoo']['guid'];
} ?></td>
        </tr>
        <tr>
          <td><strong>Name:</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['familyName'])) {
    echo $_SESSION['yahoo']['familyName'] . ' ' . $_SESSION['yahoo']['givenName'];
} ?></td>
        </tr>
        <tr>
          <td><strong>Nickname:</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['nickname'])) {
    echo $_SESSION['yahoo']['nickname'];
} ?></td>
        </tr>
        <tr>
          <td><strong>Birthdate:</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['birthdate'])) {
    echo $_SESSION['yahoo']['birthdate'];
} ?></td>
        </tr>
        <tr>
          <td><strong>Profile Created On:</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['created'])) {
    echo $_SESSION['yahoo']['created'];
} ?></td>
        </tr>
        <tr>
          <td><strong>Email Address:</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['handle'])) {
    echo $_SESSION['yahoo']['handle'];
} ?></td>
        </tr>
        <tr>
          <td><strong>Gender:</strong></td>
          <td><?php if (isset($_SESSION['yahoo']['gender'])) {
    echo $_SESSION['yahoo']['gender'];
} ?></td>
        </tr>
        
        <tr>
         <td></td>
          <td></td>
        </tr>
        
        <tr>
         <td></td>
          <td></td>
        </tr>
        
        <tr>
          <td height="29"></td>
          <td></td>
        </tr>
        
        
        <tr>
          <td>&nbsp;</td>
          <td style="text-align:right"><a href="content.php?action=logout"><img src="img/logout.png" width="150" height="30" alt="Logout"/></a></td>
        </tr>
      </tbody>
    </table>
  </div>
</section>
</body>
</html>

