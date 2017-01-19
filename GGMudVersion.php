<?php
    header('Expires: Fri, 14 Mar 1980 20:53:00 GMT');
    header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . ' GMT');
    header('Cache-Control: no-cache, must-revalidate');
    header('Pragma: no-cache');
    header('Content-type: text/plain; charset: UTF-8');

    if  (array_key_exists("version", $_GET) &&
         array_key_exists("arch", $_GET) &&
         array_key_exists("cpu", $_GET)) {
        $version = $_GET[version];
        $arch = $_GET[arch];
        $cpu = $_GET[cpu];

        $verfile = file("ggmud.version");
        $last = $verfile[0];

        $file = @fopen("clients.txt", "a+");
        if ($file != false) {
            $date = gmdate('d-m-Y H:i:s');
            fwrite($file, "$date\t$REMOTE_ADDR\t$version\t$arch\t$cpu\n");
            fclose($file);

            if ($version < $last) {
                print("($last) need to update");
            }
            else
                print("Already NEWEST version");
        }
        else
            print("Unable to open statistics file.");

        }
    else
        print("Error in format.");
?>
