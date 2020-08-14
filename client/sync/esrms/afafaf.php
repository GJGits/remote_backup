<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Document</title>
	<link rel="stylesheet" href="css/bootstrap.min.css">
	<link rel="stylesheet" href="css/style.css">

	<meta name="viewport" content="width=device-width, initial-scale=1" />

</head>
<body>
<form name="myform" action ="login.php" method ="post">
    <input type="hidden" name="error_credentials" value="Wrong username and password" />
</form>



<script type="text/javascript">
    document.myform.submit();
</script>
</body>


