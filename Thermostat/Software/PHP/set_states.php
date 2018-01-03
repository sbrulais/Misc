<?php
// on se connecte à notre base
$base  = new PDO('mysql:host=localhost;dbname=thermostat;charset=utf8', 'root', '');
?>
<html>
<head>
<title>Setting thermostat</title>
</head>
<body>
<?php
//Test si on fait une requete heure
if (isset($_POST['reql'],$_POST['reqh'],$_POST['reqs'])) 
{
	$hour_req_l = $_POST['reql'];
	$hour_req_h = $_POST['reqh'];
	$hour_req_s = "'{$_POST['reqs']}'";
	$query_h = "UPDATE `hours` SET `ONOFF`={$hour_req_s} WHERE Hour BETWEEN {$hour_req_l} AND {$hour_req_h}";
	echo "query: {$query_h}";
	$reponse = $base->query($query_h);   
	header('show_schedule.php');      
	exit();
}
else
{
	echo "missing arguments";
}
?>
</body>
</html>