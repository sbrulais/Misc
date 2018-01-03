<?php
// on se connecte à notre base
$base  = new PDO('mysql:host=localhost;dbname=thermostat;charset=utf8', 'root', '');
?>
<html>
<head>
<title>Thermostat Schedule</title>
<style>
table {
    font-family: arial, sans-serif;
    border-collapse: collapse;
}

td, th {
    border: 3px solid #CFD8DC;
    text-align: center;
    padding: 5px;
}

tr:nth-child(even) {
    background-color: #CFD8DC;
}
</style>
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
	$reponse = $base->query($query_h);   
}
?>
<form action="show_schedule.php" method="post">
 <p>Heure de debut : <input type="number" name="reql" /></p>
 <p>Heure de fin : <input type="number" name="reqh" /></p>
 <p>Status (ON/OFF) : <input type="text" name="reqs" /></p>
 <p><input type="submit" value="OK"></p>
</form>
<table>
<tbody>
<?php

// on recupere le resultat sous forme d'un tableau
$reponse = $base->query('SELECT * FROM hours ORDER BY Hour');

while ($data = $reponse->fetch())
{
?>
<tr>
    <td><?php echo $data['Hour']; ?></td>
	<?php 
	if ($data['ONOFF']=='ON')
	{
		?><td bgcolor="#C0392B" width=30px>  </td><?php 
	}
	else
	{
		?><td bgcolor="#1E88E5" width=30px>   </td><?php 
	}
	?>
	</tr> 
<?php
}
$reponse->closeCursor(); // Termine le traitement de la requête
?>
</tbody>
</table>
</body>
</html>