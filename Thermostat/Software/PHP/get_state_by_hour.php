<?php
// on se connecte à notre base
$base  = new PDO('mysql:host=localhost;dbname=thermostat;charset=utf8', 'root', '');
?>
<html>
<head>
<title>Reponse thermostat</title>
</head>
<body>
<?php
//Test si on fait une requete heure
if (isset($_GET['reqh'])) {
$heure_req = $_GET['reqh'];
$query_h = "SELECT * FROM hours WHERE Hour={$heure_req}";
$reponse = $base->query($query_h);
while ($data = $reponse->fetch())
{
 echo $data['ONOFF'];
}
$reponse->closeCursor(); // Termine le traitement de la requête

}
?>
</body>
</html>