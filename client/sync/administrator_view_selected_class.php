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
	  if (session_status() == PHP_SESSION_NONE) {
			session_start();
		}
	if($_SESSION["test_in_action"]!='1'){
        include("sidebars.php");
	}
      
    ?>

    <!-- Bootstrap core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet" >


    <!-- Vendor CSS-->
    <link href="vendor/select2/select2.min.css" rel="stylesheet" media="all">
    <link href="vendor/datepicker/daterangepicker.css" rel="stylesheet" media="all">

    <!-- Main CSS-->
    <link href="css/add_parent_form.css" rel="stylesheet" media="all">
    <link href="css/errors.css" rel="stylesheet" media="all">

    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>


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
    <!-- Custom styles for this template -->
    <link href="css/dashboard.css" rel="stylesheet">
    <link href="css/style.css" rel="stylesheet">
</head>
<body>
<script>


</script>
<noscript> Sorry: Your browser does not support or has disabled javascript </noscript>

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
                                </div>
                            </div>
                        </li>
                        <?php
						if($_SESSION["test_in_action"]!='1'){
                            administrator_print_sidebar("true");
						}
                        ?>
                    </ul>

                </div>
            </nav>

            <main role="main" class="col-md-9 ml-sm-auto col-lg-10 px-4">
                <div class="wrapper wrapper--w790">
                    <div class="card card-5">
                        <div class="card-heading">
                            <h2 class="title">Composition of class <?php echo $_POST['classe'] ?></h2>
                        </div>
                        <div class="card-body">

                            <h3>Students</h3><br>

                            <?php
                            $conn = mysqli_connect("localhost","root", "", "esrms");
                            $sql = "SELECT s.surname, s.name
                                        FROM students s, class c, bridge_class_students bcs
                                        WHERE s.ssn = bcs.ssn_s
                                        AND c.cid = bcs.cid
                                        AND c.name = '" . $_POST['classe'] . "'
                                        ORDER BY s.surname";
                            //die($sql);
                            if(!$result = mysqli_query($conn,$sql)) {
                                $msg = "Errore nell’inserimento del post, riprovare";
                            }
                            $temp = mysqli_num_rows($result);
							if($temp == 0){
								echo "No students enrolled in this class";
							}
							else{
								while($row = $result->fetch_assoc()) {
									echo "<p>" . $row["surname"] . " " . $row["name"] . "</p>";
								}
							}
                            ?>


                        </div>
                    </div>
                </div>



            </main>
        </div>
    </div>

    <!-- Jquery JS-->
    <script src="vendor/jquery/jquery.min.js"></script>
    <!-- Vendor JS-->
    <script src="vendor/select2/select2.min.js"></script>
    <script src="vendor/datepicker/moment.min.js"></script>
    <script src="vendor/datepicker/daterangepicker.js"></script>

    <!-- Main JS-->
    <script src="js/global.js"></script>



    <script>window.jQuery || document.write('<script src="/docs/4.3/assets/js/vendor/jquery-slim.min.js"><\/script>')</script><script src="js/bootstrap.bundle.min.js" ></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/feather-icons/4.9.0/feather.min.js"></script>
    <script src="js/dashboard.js"></script></body>
</html>
