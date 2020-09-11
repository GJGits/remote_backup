<?php 
session_start();
$conn = mysqli_connect("localhost","root", "", "esrms");			
		if (mysqli_connect_errno()) {
			echo "Connessione fallita: ".
			mysqli_connect_error();
			exit();
		}
		
		$sql = "SELECT bct.subject
                        FROM class c, bridge_class_teachers bct
                        WHERE c.cid = bct.cid AND bct.subject = '".$_POST["subject"]."'
                        AND bct.ssn_t = '" .$_POST["user_ssn"] . "'"; 
		if(!$result = mysqli_query($conn,$sql)) {
                    $msg = "Errore nell’inserimento del post, riprovare";
                    }
		
		$temp = mysqli_num_rows($result);
		
		if($temp > 0){
		$sql1 = "SELECT * FROM lessons";
		 if(!$result = mysqli_query($conn,$sql1)) {
                    $msg = "Errore nell’inserimento del post, riprovare";
                    }
                    $temp = mysqli_num_rows($result);
					$temp = $temp+1;
		$classe = $_POST['classe'];
		$sql2 = "SELECT * FROM class WHERE name = '".$classe."' "; 
		$result2 = mysqli_query($conn,$sql2);
		$cidClass = mysqli_fetch_array($result2);
		$dat = $_POST["time"];
		$hours = $dat[0].$dat[1].":00";
		$sql = "INSERT INTO lessons (lid,cid,ssn_t,date,time,description,subject,timestamp) VALUES ('".$temp."','".$cidClass["cid"]."','".$_POST["ssn_t"]."','".$_POST["date"]."','".$hours."','".$_POST["description"]."','".$_POST["subject"]."','".date("M,d,Y h:i:s A")."')";
		if(!$result =mysqli_query($conn,$sql)) {
							$msg = "Errore nell’inserimento del post, riprovare";
						}
		$_SESSION["correct_operation"] = 1;
		header("location:teacher_page.php?successful_op=The lesson has been successfully inserted");
		}
		
		else{
		header("location:teacher_page.php?successful_op=The selected subject is not correct");
		}
		
?>