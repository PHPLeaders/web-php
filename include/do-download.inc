<?php

// $Id$

/*
   This code redirects the user to the exact file to
   download, and logs the download if it's something
   we would like to know about (PHP binary or source).
*/

function get_actual_download_file($file)
{
    // Could be a normal download or a manual download file
    $possible_files = array($file => TRUE, "manual/$file" => FALSE); 
    
    // Find out what is the exact file requested 
    $found = FALSE;
    foreach ($possible_files as $name => $log) {
        if (@file_exists($_SERVER['DOCUMENT_ROOT'] . '/distributions/' . $name)) {
            $found = $name;
            break;
        }
    }

    return $found;
}
// Download a file from a mirror site
function download_file($mirror, $file)
{
    global $MYSITE;
    
    // Redirect to the particular file
    if (!headers_sent()) {
        status_header(302);
        header('Location: ' . $mirror . 'distributions/' . $file);
    } else {
        exit("Unable to serve you the requested file for download");
    }

    // Try to flush output, and make the browser really
    // download the file, even if the log server is down
    echo " ";
    flush();
    
    // Log download on master server (may be a registered
    // shutdown function to really run after the file is
    // started to download)
    // if ($log) { download_log($mirror, $found); }
}

// Log downloads on the master server
function download_log($mirror, $file)
{
    // Set referer value
    $referer = (isset($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER'] : '-');
    
    // String to pass on as a query to the remote log
    $log_file = "https://master.php.net/log_download.php" .
                "?download_file=" . urlencode($file) .
                "&mirror=" .        urlencode($mirror) .
                "&user_referer=" .  urlencode($referer) .
                "&user_ip=" .       urlencode(i2c_realip());

    // Open the remote log and read some bytes
    $remote_log = @fopen($log_file, 'r');
    if ($remote_log) {
        fread($remote_log, 1024);
        fclose($remote_log);
    }
}
