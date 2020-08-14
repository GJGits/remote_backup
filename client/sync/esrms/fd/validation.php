<!DOCTYPE  html>
<?php  
?>


<?php

		
		$role =  $_POST['role'];
		$mail = $_POST['email'];
		$psw = $_POST['password'];
		
	
		
		$conn = mysqli_connect("localhost","root", "", "esrms");
		$mail = mysqli_real_escape_string($conn,$mail); // Per evitare che un utente malevolo possa inserire qualcosa di indesiderato
		
			
		if (mysqli_connect_errno()) {
			echo "Connessione fallita: ".
			mysqli_connect_error();
			exit();
		}
			
			//echo $role;
		$sql = "SELECT * FROM ".$role." WHERE email = '".$mail."';"; 	// retrieve psw and salt
			//die($sql);
			
		if(!$result =mysqli_query($conn,$sql)) {
			$msg = "Errore nell’inserimento del post, riprovare";
		}
		 
		$user = mysqli_num_rows($result);
		$header_selected='';

		if ($user == 1 ) { 
			$r = mysqli_fetch_array($result);
			$salt = $r['salt']; 
			$ssn_u = $r['ssn']; 
			$name_u = $r['name'];
			$surname_u = $r['surname'];
		} else {
			 $header_selected ="location:wrong_credentials.php";
			 header($header_selected);

		}
			

		//echo "PW:<br>" . $psw."<br><br>";
        $pswhashed = hash("sha512",$psw);
		//echo "HASH(PW):<br>" . $pswhashed."<br><br>";
		$psw_with_salt = $pswhashed . $salt;
		//echo "HASH(PW) + SALT:<br>" . $psw_with_salt."<br><br>";
		$hashed = hash("sha512",$psw_with_salt);
		//echo "HASH(HASH(PW) + SALT):<br>" . $hashed."<br><br>";
					
		$ris = $r['password'];
		//die("HASH IN DB:<br>" . $ris . "<br><br>");
			if($ris == $hashed){
			
				session_start();
					$_SESSION["name_u"] = $name_u ;
					$_SESSION["surname_u"] = $surname_u;
					$_SESSION["user_ssn"] = $ssn_u;
					$_SESSION["test_in_action"] = '0';
				switch($role){
					case "teachers":
					// TODO
						$header_selected = "location:teacher_page.php";
						header($header_selected);
						break;
		
					case "administrators":
						$header_selected = "location:administrator_page.php";
						header($header_selected);
						break;

					case "sysadmins":
							$header_selected = "location:sysadmin_page.php";
							header($header_selected);
							break;
								
					case "parents":
						$sql = "SELECT * FROM bridge_parents_students WHERE ssn_p = '".$ssn_u."';";
						if(!$result =mysqli_query($conn,$sql)) {
							$msg = "Errore nell’inserimento del post, riprovare";
						}
						$temp = mysqli_num_rows($result);
							if($temp>1){
								$header_selected = "location:parent_selection_child.php";
								header($header_selected);
							} else if ($temp == 1){
								$header_selected = "location:one_child.php";
								header($header_selected);
							}
							else{
								$header_selected = "location:parent_page.php";
								header($header_selected);
							}
						break;
						
					default:
						$header_selected = "location:login.php";
						header($header_selected);
						break;
				}
			} 
			else{
				$header_selected ="location:wrong_credentials.php";
				header($header_selected);
			}
				
?>
