<?php
use PHPUnit\Framework\TestCase;

class TrueTest extends TestCase
{
    public function testSuccess()
    {
      #carico dati fittizi per vedere se mi ritorna cose giuste
		$conn = mysqli_connect("localhost","root", "", "esrms");
			
		if (mysqli_connect_errno()) {
			echo "Connessione fallita: ".
			mysqli_connect_error();
			exit();
		}
		$ssn = 'abde';
		$email = 'abcde@ab.it';
		$password_base = '123456789';
		$password_hashed = 'd9e6762dd1c8eaf6d61b3c6192fc408d4d6d5f1176d0c29169bc24e71c3f274ad27fcd5811b313d681f7e55ec02d73d499c95455b6b5bb503acf574fba8ffe85';
		$name = 'adelaide';
		$surname = 'parolini';
		$subject = 'storia';
		$salt = 'abd';
		$address = 'via 2';
			//echo $role;
     	 $sql = "INSERT INTO teachers(ssn,email,password,name,surname,subject,salt,address) values('$ssn','$email','$password_hashed','$name','$surname','$subject','$salt','$address')";
			//die($sql);
		#dopo aver inserito quella teacher, testo se funziona	
		if(!$result =mysqli_query($conn,$sql)) {
			$msg = "Errore nell’inserimento del post, riprovare";
		}
		
		$_POST['role'] = 'teachers';
		$_POST['email'] = 'abcde@ab.it';
		$_POST['password'] = '123456789';
		ob_start();
	    include('validation.php');
		ob_end_clean();

		echo $_SESSION["name_u"];

    }
}
?>		




