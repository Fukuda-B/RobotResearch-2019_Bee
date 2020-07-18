<?php
define("FILE_N", "dat.txt");

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
  if(isset($_POST['con'])) {
    file_put_contents(FILE_N, $_POST['con']."<br>".date('YmdHis').rand(10111011,99999999));
  }
}
echo file_get_contents(FILE_N);
?>