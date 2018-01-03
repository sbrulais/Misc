<?php
// on se connecte à notre base
$base  = new PDO('mysql:host=localhost;dbname=thermostat;charset=utf8', 'root', '');
?>
<html>
<head>
<title>Reponse auto thermostat</title>
</head>
<body>
<?php
// Fetch current Hour in 24hr format
$heure_req = date("H");
// Construct the database query
$query_h = "SELECT * FROM hours WHERE Hour={$heure_req}";
// Fetch database answer
$dbanswer = $base->query($query_h);
while ($data = $dbanswer->fetch())
{
 echo $data['ONOFF'];
}
// Close the query
$dbanswer->closeCursor(); 
?>
</body>
</html>