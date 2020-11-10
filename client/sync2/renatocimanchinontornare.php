<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="">
    <meta name="author" content="Mark Otto, Jacob Thornton, and Bootstrap contributors">
    <meta name="generator" content="Jekyll v3.8.5">
    <title>Administrative Officer Page</title>

<?php
 include("sidebars.php");
 session_start();
 if(isset($_POST["ssn_s"])){
 $_SESSION["ssn_s"] = $_POST["ssn_s"];
 }
 if(isset($_POST["name_s"])){
 $_SESSION["name_s"] = $_POST["name_s"];
 }
 if(isset($_POST["surname_s"])){
 $_SESSION["surname_s"] = $_POST["surname_s"];
 }
?>

    <!-- Bootstrap core CSS -->
<link href="css/bootstrap.min.css" rel="stylesheet" >


    <style>
      .bd-placeholder-img {
        font-size: 1.125rem;
        text-anchor: middle;
        -webkit-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
      }

      @media (min-width: 768px) {
        .bd-placeholder-img-lg {
          font-size: 3.5rem;
        }
      }
    </style>
	
	    <!-- Bootstrap core CSS -->
<link href="css/bootstrap.min.css" rel="stylesheet" >
    <!-- Vendor CSS-->
    <link href="vendor/select2/select2.min.css" rel="stylesheet" media="all">
    <link href="vendor/datepicker/daterangepicker.css" rel="stylesheet" media="all">
    
    <!-- Main CSS-->
    <!--<link href="css/add_student_form.css" rel="stylesheet" media="all">-->    
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
    
	
    <!-- Custom styles for this template -->
    <link href="css/dashboard.css" rel="stylesheet">
		<link href="css/style.css" rel="stylesheet">
  </head>
  <body>

<nav class="navbar navbar-expand-md navbar-dark bg-dark sticky-top">
  <a class="navbar-brand" href="#">Admin Officer Account</a>
	<button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNavDropdown" aria-controls="navbarNavDropdown" aria-expanded="false" aria-label="Toggle navigation">
    <span class="navbar-toggler-icon"></span>
  </button>
  <div class="collapse navbar-collapse" id="navbarNavDropdown">
		
		<ul class="navbar-nav ml-auto">
			<li class="nav-item align-left">
				<button type="submit" class="btn btn-danger" onclick="location.href='logout_post.php';">Sign out</button>
			</li>
		</ul>
	</div>
</nav>

<div>

<div class="container-fluid">
  <div class="row">
   <nav class="col-md-2 d-md-block bg-light sidebar">
      <div class="small_screen" >
        <ul class="nav flex-column">
		 <li class="nav-item">
                        <div class="user-info">
                            <div class="image"><a href="profile.html"><img src="photos/student.jpg" alt="User"></a></div>
                            <div class="detail">
                                <h4><?php echo $_SESSION["name_u"] ." ". $_SESSION["surname_u"]; ?></h4>
																<small> </small>
                            </div>
                        </div>
           </li>
            <?php
                administrator_print_sidebar("true");
            ?>
        </ul>


      </div>
    </nav>

    <main role="main" class="col-md-9 ml-sm-auto col-lg-10 px-4">
   
     
    </main>
  </div>
</div>
<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
      <script>window.jQuery || document.write('<script src="/docs/4.3/assets/js/vendor/jquery-slim.min.js"><\/script>')</script><script src="js/bootstrap.bundle.min.js" ></script>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/feather-icons/4.9.0/feather.min.js"></script>
        <script src="js/dashboard.js"></script></body>
</html>
